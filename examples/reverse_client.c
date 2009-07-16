/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief Example Client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libgearman/gearman.h>

static void usage(char *name);

int main(int argc, char *argv[])
{
  int c;
  char *host= NULL;
  in_port_t port= 0;
  gearman_return_t ret;
  gearman_client_st client;
  char *result;
  size_t result_size;
  uint32_t numerator;
  uint32_t denominator;

  while ((c = getopt(argc, argv, "h:p:")) != -1)
  {
    switch(c)
    {
    case 'h':
      host= optarg;
      break;

    case 'p':
      port= (in_port_t)atoi(optarg);
      break;

    default:
      usage(argv[0]);
      exit(1);
    }
  }

  if (argc != (optind + 1))
  {
    usage(argv[0]);
    exit(1);
  }

  if (gearman_client_create(&client) == NULL)
  {
    fprintf(stderr, "Memory allocation failure on client creation\n");
    exit(1);
  }

  ret= gearman_client_add_server(&client, host, port);
  if (ret != GEARMAN_SUCCESS)
  {
    fprintf(stderr, "%s\n", gearman_client_error(&client));
    exit(1);
  }

  while (1)
  {
    result= (char *)gearman_client_do(&client, "reverse", NULL,
                                      (void *)argv[optind],
                                      (size_t)strlen(argv[optind]),
                                      &result_size, &ret);
    if (ret == GEARMAN_WORK_DATA)
    {
      printf("Data=%.*s\n", (int)result_size, result);
      free(result);
      continue;
    }
    else if (ret == GEARMAN_WORK_STATUS)
    {
      gearman_client_do_status(&client, &numerator, &denominator);
      printf("Status: %u/%u\n", numerator, denominator);
      continue;
    }
    else if (ret == GEARMAN_SUCCESS)
    {
      printf("Result=%.*s\n", (int)result_size, result);
      free(result);
    }
    else if (ret == GEARMAN_WORK_FAIL)
      fprintf(stderr, "Work failed\n");
    else
      fprintf(stderr, "%s\n", gearman_client_error(&client));

    break;
  }

  gearman_client_free(&client);

  return 0;
}

static void usage(char *name)
{
  printf("\nusage: %s [-h <host>] [-p <port>] <string>\n", name);
  printf("\t-h <host> - job server host\n");
  printf("\t-p <port> - job server port\n");
}
