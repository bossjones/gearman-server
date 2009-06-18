/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief Server function definitions
 */

#include "common.h"

/*
 * Public definitions
 */

gearman_server_function_st *
gearman_server_function_get(gearman_server_st *server,
                            const char *function_name,
                            size_t function_name_size)
{
  gearman_server_function_st *server_function;

  for (server_function= server->function_list; server_function != NULL;
       server_function= server_function->next)
  {
    if (server_function->function_name_size == function_name_size &&
        !memcmp(server_function->function_name, function_name,
                function_name_size))
    {
      return server_function;
    }
  }

  server_function= gearman_server_function_create(server, NULL);
  if (server_function == NULL)
    return NULL;

  server_function->function_name= malloc(function_name_size + 1);
  if (server_function->function_name == NULL)
  {
    gearman_server_function_free(server_function);
    return NULL;
  }

  memcpy(server_function->function_name, function_name, function_name_size);
  server_function->function_name[function_name_size]= 0;
  server_function->function_name_size= function_name_size;

  return server_function;
}

gearman_server_function_st *
gearman_server_function_create(gearman_server_st *server,
                               gearman_server_function_st *server_function)
{
  if (server_function == NULL)
  {
    server_function= malloc(sizeof(gearman_server_function_st));
    if (server_function == NULL)
      return NULL;

    memset(server_function, 0, sizeof(gearman_server_function_st));
    server_function->options|= GEARMAN_SERVER_FUNCTION_ALLOCATED;
  }
  else
    memset(server_function, 0, sizeof(gearman_server_function_st));

  server_function->server= server;
  server_function->max_queue_size= GEARMAN_DEFAULT_MAX_QUEUE_SIZE;

  GEARMAN_LIST_ADD(server->function, server_function,)

  return server_function;
}

void gearman_server_function_free(gearman_server_function_st *server_function)
{
  if (server_function->function_name != NULL)
    free(server_function->function_name);

  GEARMAN_LIST_DEL(server_function->server->function, server_function,)

  if (server_function->options & GEARMAN_SERVER_FUNCTION_ALLOCATED)
    free(server_function);
}
