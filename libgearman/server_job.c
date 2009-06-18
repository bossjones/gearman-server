/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief Server job definitions
 */

#include "common.h"

/*
 * Private declarations
 */

/**
 * @addtogroup gearman_server_job_private Private Server Job Functions
 * @ingroup gearman_server_job
 * @{
 */

/**
 * Generate hash key for job handles and unique IDs.
 */
static uint32_t _server_job_hash(const char *key, size_t key_size);

/**
 * Get a server job structure from the unique ID. If data_size is non-zero,
 * then unique points to the workload data and not a real unique key.
 */
static gearman_server_job_st *
_server_job_get_unique(gearman_server_st *server, uint32_t unique_key,
                       gearman_server_function_st *server_function,
                       const char *unique, size_t data_size);

/** @} */

/*
 * Public definitions
 */

gearman_server_job_st *
gearman_server_job_add(gearman_server_st *server, const char *function_name,
                       size_t function_name_size, const char *unique,
                       size_t unique_size, const void *data, size_t data_size,
                       gearman_job_priority_t priority,
                       gearman_server_client_st *server_client,
                       gearman_return_t *ret_ptr)
{
  gearman_server_job_st *server_job;
  gearman_server_function_st *server_function;
  uint32_t key;

  server_function= gearman_server_function_get(server, function_name,
                                               function_name_size);
  if (server_function == NULL)
  {
    *ret_ptr= GEARMAN_MEMORY_ALLOCATION_FAILURE;
    return NULL;
  }

  if (unique_size == 0)
    server_job= NULL;
  else
  {
    if (unique_size == 1 && *unique ==  '-')
    {
      if (data_size == 0)
      {
        key= 0;
        server_job= NULL;
      }
      else
      {
        /* Look up job via unique data when unique = '-'. */
        key= _server_job_hash(data, data_size);
        server_job= _server_job_get_unique(server, key, server_function, data,
                                           data_size);
      }
    }
    else
    {
      /* Look up job via unique ID first to make sure it's not a duplicate. */
      key= _server_job_hash(unique, unique_size);
      server_job= _server_job_get_unique(server, key, server_function, unique,
                                         0);
    }
  }

  if (server_job == NULL)
  {
    if (server_function->max_queue_size > 0 &&
        server_function->job_total >= server_function->max_queue_size)
    {
      *ret_ptr= GEARMAN_JOB_QUEUE_FULL;
      return NULL;
    }

    server_job= gearman_server_job_create(server, NULL);
    if (server_job == NULL)
    {
      *ret_ptr= GEARMAN_MEMORY_ALLOCATION_FAILURE;
      return NULL;
    }

    server_job->priority= priority;

    server_job->function= server_function;
    server_function->job_total++;

    snprintf(server_job->job_handle, GEARMAN_JOB_HANDLE_SIZE, "%s:%u",
             server->job_handle_prefix, server->job_handle_count);
    snprintf(server_job->unique, GEARMAN_UNIQUE_SIZE, "%.*s",
             (uint32_t)unique_size, unique);
    server->job_handle_count++;
    server_job->data= data;
    server_job->data_size= data_size;

    server_job->unique_key= key;
    key= key % GEARMAN_JOB_HASH_SIZE;
    GEARMAN_HASH_ADD(server->unique, key, server_job, unique_)

    key= _server_job_hash(server_job->job_handle,
                          strlen(server_job->job_handle));
    server_job->job_handle_key= key;
    key= key % GEARMAN_JOB_HASH_SIZE;
    GEARMAN_HASH_ADD(server->job, key, server_job,)

    if (server->options & GEARMAN_SERVER_QUEUE_REPLAY)
      server_job->options|= GEARMAN_SERVER_JOB_QUEUED;
    else if (server_client == NULL && server->gearman->queue_add_fn != NULL)
    {
      *ret_ptr= (*(server->gearman->queue_add_fn))(server->gearman,
                                          (void *)server->gearman->queue_fn_arg,
                                          server_job->unique,
                                          unique_size,
                                          function_name,
                                          function_name_size,
                                          data, data_size, priority);
      if (*ret_ptr != GEARMAN_SUCCESS)
      {
        server_job->data= NULL;
        gearman_server_job_free(server_job);
        return NULL;
      }

      if (server->gearman->queue_flush_fn != NULL)
      {
        *ret_ptr= (*(server->gearman->queue_flush_fn))(server->gearman,
                                         (void *)server->gearman->queue_fn_arg);
        if (*ret_ptr != GEARMAN_SUCCESS)
        {
          server_job->data= NULL;
          gearman_server_job_free(server_job);
          return NULL;
        }
      }

      server_job->options|= GEARMAN_SERVER_JOB_QUEUED;
    }

    *ret_ptr= gearman_server_job_queue(server_job);
    if (*ret_ptr != GEARMAN_SUCCESS)
    {
      if (server_client == NULL && server->gearman->queue_done_fn != NULL)
      {
        /* Do our best to remove the job from the queue. */
        (void)(*(server->gearman->queue_done_fn))(server->gearman,
                                          (void *)server->gearman->queue_fn_arg,
                                          server_job->unique, unique_size, 
                                          server_job->function->function_name,
                                          server_job->function->function_name_size);
      }

      gearman_server_job_free(server_job);
      return NULL;
    }
  }
  else
    *ret_ptr= GEARMAN_JOB_EXISTS;

  if (server_client != NULL)
  {
    server_client->job= server_job;
    GEARMAN_LIST_ADD(server_job->client, server_client, job_)
  }

  return server_job;
}

gearman_server_job_st *
gearman_server_job_create(gearman_server_st *server,
                          gearman_server_job_st *server_job)
{
  if (server_job == NULL)
  {
    if (server->free_job_count > 0)
    {
      server_job= server->free_job_list;
      GEARMAN_LIST_DEL(server->free_job, server_job,)
    }
    else
    {
      server_job= malloc(sizeof(gearman_server_job_st));
      if (server_job == NULL)
        return NULL;
    }

    memset(server_job, 0, sizeof(gearman_server_job_st));
    server_job->options|= GEARMAN_SERVER_JOB_ALLOCATED;
  }
  else
    memset(server_job, 0, sizeof(gearman_server_job_st));

  server_job->server= server;

  return server_job;
}

void gearman_server_job_free(gearman_server_job_st *server_job)
{
  uint32_t key;

  if (server_job->worker != NULL)
    server_job->function->job_running--;

  server_job->function->job_total--;

  if (server_job->data != NULL)
    free((void *)(server_job->data));

  while (server_job->client_list != NULL)
    gearman_server_client_free(server_job->client_list);

  if (server_job->worker != NULL)
    server_job->worker->job= NULL;

  key= server_job->unique_key % GEARMAN_JOB_HASH_SIZE;
  GEARMAN_HASH_DEL(server_job->server->unique, key, server_job, unique_)

  key= server_job->job_handle_key % GEARMAN_JOB_HASH_SIZE;
  GEARMAN_HASH_DEL(server_job->server->job, key, server_job,)

  if (server_job->options & GEARMAN_SERVER_JOB_ALLOCATED)
  {
    if (server_job->server->free_job_count < GEARMAN_MAX_FREE_SERVER_JOB)
      GEARMAN_LIST_ADD(server_job->server->free_job, server_job,)
    else
      free(server_job);
  }
}

gearman_server_job_st *gearman_server_job_get(gearman_server_st *server,
                                              const char *job_handle)
{
  gearman_server_job_st *server_job;
  uint32_t key;

  key= _server_job_hash(job_handle, strlen(job_handle));

  for (server_job= server->job_hash[key % GEARMAN_JOB_HASH_SIZE];
       server_job != NULL; server_job= server_job->next)
  {
    if (server_job->job_handle_key == key &&
        !strcmp(server_job->job_handle, job_handle))
    {
      return server_job;
    }
  }

  return NULL;
}

gearman_server_job_st *
gearman_server_job_peek(gearman_server_con_st *server_con)
{
  gearman_server_worker_st *server_worker;
  gearman_job_priority_t priority;

  for (server_worker= server_con->worker_list; server_worker != NULL;
       server_worker= server_worker->con_next)
  {
    if (server_worker->function->job_count != 0)
    {
      for (priority= GEARMAN_JOB_PRIORITY_HIGH;
           priority != GEARMAN_JOB_PRIORITY_MAX; priority++)
      {
        if (server_worker->function->job_list[priority] != NULL)
        {
          if (server_worker->function->job_list[priority]->options &
              GEARMAN_SERVER_JOB_IGNORE)
          {
            /* This is only happens when a client disconnects from a foreground
               job. We do this because we don't want to run the job anymore. */
            server_worker->function->job_list[priority]->options&=
                                                     ~GEARMAN_SERVER_JOB_IGNORE;
            gearman_server_job_free(gearman_server_job_take(server_con));
            return gearman_server_job_peek(server_con);
          }
          return server_worker->function->job_list[priority];
        }
      }
    }
  }

  return NULL;
}

gearman_server_job_st *
gearman_server_job_take(gearman_server_con_st *server_con)
{
  gearman_server_worker_st *server_worker;
  gearman_server_job_st *server_job;
  gearman_job_priority_t priority;

  for (server_worker= server_con->worker_list; server_worker != NULL;
       server_worker= server_worker->con_next)
  {
    if (server_worker->function->job_count != 0)
      break;
  }

  if (server_worker == NULL)
    return NULL;

  for (priority= GEARMAN_JOB_PRIORITY_HIGH;
       priority != GEARMAN_JOB_PRIORITY_MAX; priority++)
  {
    if (server_worker->function->job_list[priority] != NULL)
      break;
  }

  server_job= server_worker->function->job_list[priority];
  server_job->function->job_list[priority]= server_job->function_next;
  if (server_job->function->job_end[priority] == server_job)
    server_job->function->job_end[priority]= NULL;
  server_job->function->job_count--;

  server_job->worker= server_worker;
  server_worker->job= server_job;
  server_job->function->job_running++;

  if (server_job->options & GEARMAN_SERVER_JOB_IGNORE)
  {
    gearman_server_job_free(server_job);
    return gearman_server_job_take(server_con);
  }

  return server_job;
}

gearman_return_t gearman_server_job_queue(gearman_server_job_st *server_job)
{
  gearman_server_worker_st *server_worker;
  gearman_return_t ret;

  if (server_job->worker != NULL)
  {
    server_job->function->job_running--;
    server_job->function_next= NULL;
  }

  server_job->worker= NULL;
  server_job->numerator= 0;
  server_job->denominator= 0;

  /* Queue NOOP for possible sleeping workers. */
  for (server_worker= server_job->function->worker_list; server_worker != NULL;
       server_worker= server_worker->function_next)
  {
    if (!(server_worker->con->noop_queued) &&
        !(server_worker->con->options & GEARMAN_SERVER_CON_SLEEPING))
    {
      continue;
    }

    ret= gearman_server_io_packet_add(server_worker->con, false,
                                      GEARMAN_MAGIC_RESPONSE,
                                      GEARMAN_COMMAND_NOOP, NULL);
    if (ret != GEARMAN_SUCCESS)
      return ret;

    server_worker->con->noop_queued= true;
  }

  /* Queue the job to be run. */
  if (server_job->function->job_list[server_job->priority] == NULL)
    server_job->function->job_list[server_job->priority]= server_job;
  else
  {
    server_job->function->job_end[server_job->priority]->function_next=
                                                                     server_job;
  }
  server_job->function->job_end[server_job->priority]= server_job;
  server_job->function->job_count++;

  return GEARMAN_SUCCESS;
}

/*
 * Private definitions
 */

static uint32_t _server_job_hash(const char *key, size_t key_size)
{
  const char *ptr= key;
  uint32_t value= 0;

  while (key_size--)
  {
    value += *ptr++;
    value += (value << 10);
    value ^= (value >> 6);
  }
  value += (value << 3);
  value ^= (value >> 11);
  value += (value << 15);

  return value == 0 ? 1 : value;
}

static gearman_server_job_st *
_server_job_get_unique(gearman_server_st *server, uint32_t unique_key,
                       gearman_server_function_st *server_function,
                       const char *unique, size_t data_size)
{
  gearman_server_job_st *server_job;

  for (server_job= server->unique_hash[unique_key % GEARMAN_JOB_HASH_SIZE];
       server_job != NULL; server_job= server_job->unique_next)
  {
    if (data_size == 0)
    {
      if (server_job->function == server_function &&
          server_job->unique_key == unique_key &&
          !strcmp(server_job->unique, unique))
      {
        return server_job;
      }
    }
    else
    {
      if (server_job->function == server_function &&
          server_job->unique_key == unique_key &&
          server_job->data_size == data_size &&
          !memcmp(server_job->data, unique, data_size))
      {
        return server_job;
      }
    }
  }

  return NULL;
}
