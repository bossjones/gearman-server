/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief Gearmand Connection Definitions
 */

#include "common.h"

/*
 * Private declarations
 */

/**
 * @addtogroup gearmand_con_private Private Gearmand Connection Functions
 * @ingroup gearmand_con
 * @{
 */

static void _con_ready(int fd __attribute__ ((unused)), short events,
                       void *arg);

static gearman_return_t _con_add(gearmand_thread_st *thread,
                                 gearmand_con_st *con);

/** @} */

/*
 * Public definitions
 */

gearman_return_t gearmand_con_create(gearmand_st *gearmand, int fd,
                                     const char *host, const char *port)
{
  gearmand_con_st *dcon;
  gearmand_con_st *free_dcon_list;
  uint32_t free_dcon_count;

  if (gearmand->free_dcon_count > 0)
  {
    dcon= gearmand->free_dcon_list;
    GEARMAN_LIST_DEL(gearmand->free_dcon, dcon,)
  }
  else
  {
    dcon= malloc(sizeof(gearmand_con_st));
    if (dcon == NULL)
    {
      close(fd);
      GEARMAN_FATAL(gearmand, "gearmand_con_create:malloc")
      return GEARMAN_MEMORY_ALLOCATION_FAILURE;
    }
  }

  memset(dcon, 0, sizeof(gearmand_con_st));
  dcon->fd= fd;
  strncpy(dcon->host, host, NI_MAXHOST - 1);
  strncpy(dcon->port, port, NI_MAXSERV - 1);

  /* If we are not threaded, just add the connection now. */
  if (gearmand->threads == 0)
  {
    dcon->thread= gearmand->thread_list;
    return _con_add(gearmand->thread_list, dcon);
  }

  /* We do a simple round-robin connection queue algorithm here. */
  if (gearmand->thread_add_next == NULL)
    gearmand->thread_add_next= gearmand->thread_list;

  dcon->thread= gearmand->thread_add_next;

  /* We don't need to lock if the list is empty. */
  if (dcon->thread->dcon_add_count == 0 &&
      dcon->thread->free_dcon_count < gearmand->max_thread_free_dcon_count)
  {
    GEARMAN_LIST_ADD(dcon->thread->dcon_add, dcon,)
    gearmand_thread_wakeup(dcon->thread, GEARMAND_WAKEUP_CON);
  }
  else
  {
    (void ) pthread_mutex_lock(&(dcon->thread->lock));

    GEARMAN_LIST_ADD(dcon->thread->dcon_add, dcon,)

    /* Take the free connection structures back to reuse. */
    free_dcon_list= dcon->thread->free_dcon_list;
    free_dcon_count= dcon->thread->free_dcon_count;
    dcon->thread->free_dcon_list= NULL;
    dcon->thread->free_dcon_count= 0;

    (void ) pthread_mutex_unlock(&(dcon->thread->lock));

    /* Only wakeup the thread if this is the first in the queue. We don't need
       to lock around the count check, worst case it was already picked up and
       we send an extra byte. */
    if (dcon->thread->dcon_add_count == 1)
      gearmand_thread_wakeup(dcon->thread, GEARMAND_WAKEUP_CON);

    /* Put the free connection structures we grabbed on the main list. */
    while (free_dcon_list != NULL)
    {
      dcon= free_dcon_list;
      GEARMAN_LIST_DEL(free_dcon, dcon,)
      GEARMAN_LIST_ADD(gearmand->free_dcon, dcon,)
    }
  }

  gearmand->thread_add_next= gearmand->thread_add_next->next;

  return GEARMAN_SUCCESS;
}

void gearmand_con_free(gearmand_con_st *dcon)
{
  assert(event_del(&(dcon->event)) == 0);

  /* This gets around a libevent bug when both POLLIN and POLLOUT are set. */
  event_set(&(dcon->event), dcon->fd, EV_READ, _con_ready, dcon);
  event_base_set(dcon->thread->base, &(dcon->event));
  event_add(&(dcon->event), NULL);
  assert(event_del(&(dcon->event)) == 0);

  gearman_server_con_free(dcon->server_con);
  GEARMAN_LIST_DEL(dcon->thread->dcon, dcon,)
  close(dcon->fd);

  if (dcon->thread->gearmand->free_dcon_count < GEARMAN_MAX_FREE_SERVER_CON)
  {
    if (dcon->thread->gearmand->threads == 0)
      GEARMAN_LIST_ADD(dcon->thread->gearmand->free_dcon, dcon,)
    else
    {
      /* Lock here because the main thread may be emptying this. */
      (void ) pthread_mutex_lock(&(dcon->thread->lock));
      GEARMAN_LIST_ADD(dcon->thread->free_dcon, dcon,)
      (void ) pthread_mutex_unlock(&(dcon->thread->lock));
    }
  }
  else
    free(dcon);
}

void gearmand_con_check_queue(gearmand_thread_st *thread)
{
  gearmand_con_st *dcon;

  /* Dirty check is fine here, wakeup is always sent after add completes. */
  if (thread->dcon_add_count == 0)
    return;

  /* We want to add new connections inside the lock because other threads may
     walk the thread's dcon_list while holding the lock. */
  while (thread->dcon_add_list != NULL)
  {
    (void ) pthread_mutex_lock(&(thread->lock));
    dcon= thread->dcon_add_list;
    GEARMAN_LIST_DEL(thread->dcon_add, dcon,)
    (void ) pthread_mutex_unlock(&(thread->lock));

    if (_con_add(thread, dcon) != GEARMAN_SUCCESS)
      gearmand_wakeup(thread->gearmand, GEARMAND_WAKEUP_SHUTDOWN);
  }
}

gearman_return_t gearmand_con_watch(gearman_con_st *con, short events,
                                    void *arg __attribute__ ((unused)))
{
  gearmand_con_st *dcon;
  short set_events= 0;

  dcon= (gearmand_con_st *)gearman_con_data(con);
  dcon->con= con;

  if (events & POLLIN)
    set_events|= EV_READ;
  if (events & POLLOUT)
    set_events|= EV_WRITE;

  if (dcon->last_events != set_events)
  {
    if (dcon->last_events != 0)
      assert(event_del(&(dcon->event)) == 0);
    event_set(&(dcon->event), dcon->fd, set_events | EV_PERSIST, _con_ready,
              dcon);
    event_base_set(dcon->thread->base, &(dcon->event));

    if (event_add(&(dcon->event), NULL) == -1)
    {
      GEARMAN_FATAL(dcon->thread->gearmand, "_con_watch:event_add:-1")
      return GEARMAN_EVENT;
    }

    dcon->last_events= set_events;
  }

  GEARMAN_CRAZY(dcon->thread->gearmand, "[%4u] %15s:%5s Watching %8s%8s",
                dcon->thread->count, dcon->host, dcon->port,
                events & POLLIN ? "POLLIN" : "",
                events & POLLOUT ? "POLLOUT" : "")

  return GEARMAN_SUCCESS;
}

/*
 * Private definitions
 */

static void _con_ready(int fd __attribute__ ((unused)), short events,
                       void *arg)
{
  gearmand_con_st *dcon= (gearmand_con_st *)arg;
  short revents= 0;

  if (events & EV_READ)
    revents|= POLLIN;
  if (events & EV_WRITE)
    revents|= POLLOUT;

  gearman_con_set_revents(dcon->con, revents);

  GEARMAN_CRAZY(dcon->thread->gearmand, "[%4u] %15s:%5s Ready    %8s%8s",
                dcon->thread->count, dcon->host, dcon->port,
                revents & POLLIN ? "POLLIN" : "",
                revents & POLLOUT ? "POLLOUT" : "")

  gearmand_thread_run(dcon->thread);
}

static gearman_return_t _con_add(gearmand_thread_st *thread,
                                 gearmand_con_st *dcon)
{
  dcon->server_con= gearman_server_con_add(&(thread->server_thread), dcon->fd,
                                           dcon);
  if (dcon->server_con == NULL)
  {
    close(dcon->fd);
    free(dcon);
    return GEARMAN_MEMORY_ALLOCATION_FAILURE;
  }

  gearman_server_con_set_host(dcon->server_con, dcon->host);
  gearman_server_con_set_port(dcon->server_con, dcon->port);

  GEARMAN_INFO(thread->gearmand, "[%4u] %15s:%5s Connected", thread->count,
               dcon->host, dcon->port)

  GEARMAN_LIST_ADD(thread->dcon, dcon,)

  return GEARMAN_SUCCESS;
}
