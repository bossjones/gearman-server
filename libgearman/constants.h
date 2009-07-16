/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief Defines, typedefs, and enums
 */

#ifndef __GEARMAN_CONSTANTS_H__
#define __GEARMAN_CONSTANTS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup gearman_constants Gearman Constants
 * @{
 */

/* Defines. */
#define GEARMAN_DEFAULT_TCP_HOST "127.0.0.1"
#define GEARMAN_DEFAULT_TCP_PORT 4730
#define GEARMAN_DEFAULT_SOCKET_TIMEOUT 10
#define GEARMAN_DEFAULT_SOCKET_SEND_SIZE 32768
#define GEARMAN_DEFAULT_SOCKET_RECV_SIZE 32768
#define GEARMAN_DEFAULT_BACKLOG 64
#define GEARMAN_DEFAULT_MAX_QUEUE_SIZE 0

#define GEARMAN_MAX_ERROR_SIZE 1024
#define GEARMAN_PACKET_HEADER_SIZE 12
#define GEARMAN_JOB_HANDLE_SIZE 64
#define GEARMAN_OPTION_SIZE 64
#define GEARMAN_UNIQUE_SIZE 64
#define GEARMAN_MAX_COMMAND_ARGS 8
#define GEARMAN_ARGS_BUFFER_SIZE 128
#define GEARMAN_SEND_BUFFER_SIZE 8192
#define GEARMAN_RECV_BUFFER_SIZE 8192
#define GEARMAN_SERVER_CON_ID_SIZE 128
#define GEARMAN_JOB_HASH_SIZE 383
#define GEARMAN_MAX_FREE_SERVER_CON 1000
#define GEARMAN_MAX_FREE_SERVER_PACKET 2000
#define GEARMAN_MAX_FREE_SERVER_JOB 1000
#define GEARMAN_MAX_FREE_SERVER_CLIENT 1000
#define GEARMAN_MAX_FREE_SERVER_WORKER 1000
#define GEARMAN_TEXT_RESPONSE_SIZE 8192
#define GEARMAN_WORKER_WAIT_TIMEOUT (10 * 1000) /* Milliseconds */
#define GEARMAN_PIPE_BUFFER_SIZE 256
#define GEARMAN_CONF_MAX_OPTION_SHORT 128
#define GEARMAN_CONF_DISPLAY_WIDTH 80

/* Types. */
typedef struct gearman_st gearman_st;
typedef struct gearman_con_st gearman_con_st;
typedef struct gearman_packet_st gearman_packet_st;
typedef struct gearman_command_info_st gearman_command_info_st;
typedef struct gearman_task_st gearman_task_st;
typedef struct gearman_client_st gearman_client_st;
typedef struct gearman_job_st gearman_job_st;
typedef struct gearman_worker_st gearman_worker_st;
typedef struct gearman_worker_function_st gearman_worker_function_st;
typedef struct gearman_server_st gearman_server_st;
typedef struct gearman_server_thread_st gearman_server_thread_st;
typedef struct gearman_server_con_st gearman_server_con_st;
typedef struct gearman_server_packet_st gearman_server_packet_st;
typedef struct gearman_server_function_st gearman_server_function_st;
typedef struct gearman_server_client_st gearman_server_client_st;
typedef struct gearman_server_worker_st gearman_server_worker_st;
typedef struct gearman_server_job_st gearman_server_job_st;
typedef struct gearmand_st gearmand_st;
typedef struct gearmand_port_st gearmand_port_st;
typedef struct gearmand_con_st gearmand_con_st;
typedef struct gearmand_thread_st gearmand_thread_st;
typedef struct gearman_conf_st gearman_conf_st;
typedef struct gearman_conf_option_st gearman_conf_option_st;
typedef struct gearman_conf_module_st gearman_conf_module_st;

/**
 * Return codes.
 */
typedef enum
{
  GEARMAN_SUCCESS,
  GEARMAN_IO_WAIT,
  GEARMAN_SHUTDOWN,
  GEARMAN_SHUTDOWN_GRACEFUL,
  GEARMAN_ERRNO,
  GEARMAN_EVENT,
  GEARMAN_TOO_MANY_ARGS,
  GEARMAN_NO_ACTIVE_FDS,
  GEARMAN_INVALID_MAGIC,
  GEARMAN_INVALID_COMMAND,
  GEARMAN_INVALID_PACKET,
  GEARMAN_UNEXPECTED_PACKET,
  GEARMAN_GETADDRINFO,
  GEARMAN_NO_SERVERS,
  GEARMAN_LOST_CONNECTION,
  GEARMAN_MEMORY_ALLOCATION_FAILURE,
  GEARMAN_JOB_EXISTS,
  GEARMAN_JOB_QUEUE_FULL,
  GEARMAN_SERVER_ERROR,
  GEARMAN_WORK_ERROR,
  GEARMAN_WORK_DATA,
  GEARMAN_WORK_WARNING,
  GEARMAN_WORK_STATUS,
  GEARMAN_WORK_EXCEPTION,
  GEARMAN_WORK_FAIL,
  GEARMAN_NOT_CONNECTED,
  GEARMAN_COULD_NOT_CONNECT,
  GEARMAN_SEND_IN_PROGRESS,
  GEARMAN_RECV_IN_PROGRESS,
  GEARMAN_NOT_FLUSHING,
  GEARMAN_DATA_TOO_LARGE,
  GEARMAN_INVALID_FUNCTION_NAME,
  GEARMAN_INVALID_WORKER_FUNCTION,
  GEARMAN_NO_REGISTERED_FUNCTIONS,
  GEARMAN_NO_JOBS,
  GEARMAN_ECHO_DATA_CORRUPTION,
  GEARMAN_NEED_WORKLOAD_FN,
  GEARMAN_PAUSE,
  GEARMAN_UNKNOWN_STATE,
  GEARMAN_PTHREAD,
  GEARMAN_PIPE_EOF,
  GEARMAN_QUEUE_ERROR,
  GEARMAN_FLUSH_DATA,
  GEARMAN_SEND_BUFFER_TOO_SMALL,
  GEARMAN_IGNORE_PACKET,
  GEARMAN_UNKNOWN_OPTION,
  GEARMAN_MAX_RETURN /* Always add new error code before */
} gearman_return_t;

/**
 * Verbosity levels.
 */
typedef enum
{
  GEARMAN_VERBOSE_FATAL,
  GEARMAN_VERBOSE_ERROR,
  GEARMAN_VERBOSE_INFO,
  GEARMAN_VERBOSE_DEBUG,
  GEARMAN_VERBOSE_CRAZY,
  GEARMAN_VERBOSE_MAX
} gearman_verbose_t;

/** @} */

/**
 * @ingroup gearman
 * Options for gearman_st.
 */
typedef enum
{
  GEARMAN_ALLOCATED=          (1 << 0),
  GEARMAN_NON_BLOCKING=       (1 << 1),
  GEARMAN_DONT_TRACK_PACKETS= (1 << 2)
} gearman_options_t;

/**
 * @ingroup gearman_con
 * Options for gearman_con_st.
 */
typedef enum
{
  GEARMAN_CON_ALLOCATED=              (1 << 0),
  GEARMAN_CON_READY=                  (1 << 1),
  GEARMAN_CON_PACKET_IN_USE=          (1 << 2),
  GEARMAN_CON_EXTERNAL_FD=            (1 << 3),
  GEARMAN_CON_IGNORE_LOST_CONNECTION= (1 << 4),
  GEARMAN_CON_CLOSE_AFTER_FLUSH=      (1 << 5)
} gearman_con_options_t;

/**
 * @ingroup gearman_con
 * States for gearman_con_st.
 */
typedef enum
{
  GEARMAN_CON_STATE_ADDRINFO,
  GEARMAN_CON_STATE_CONNECT,
  GEARMAN_CON_STATE_CONNECTING,
  GEARMAN_CON_STATE_CONNECTED
} gearman_con_state_t;

/**
 * @ingroup gearman_con
 * Send states for gearman_con_st.
 */
typedef enum
{
  GEARMAN_CON_SEND_STATE_NONE,
  GEARMAN_CON_SEND_STATE_PRE_FLUSH,
  GEARMAN_CON_SEND_STATE_FORCE_FLUSH,
  GEARMAN_CON_SEND_STATE_FLUSH,
  GEARMAN_CON_SEND_STATE_FLUSH_DATA
} gearman_con_send_state_t;

/**
 * @ingroup gearman_con
 * Recv states for gearman_con_st.
 */
typedef enum
{
  GEARMAN_CON_RECV_STATE_NONE,
  GEARMAN_CON_RECV_STATE_READ,
  GEARMAN_CON_RECV_STATE_READ_DATA
} gearman_con_recv_state_t;

/**
 * @ingroup gearman_packet
 * Options for gearman_packet_st.
 */
typedef enum
{
  GEARMAN_PACKET_ALLOCATED= (1 << 0),
  GEARMAN_PACKET_COMPLETE=  (1 << 1),
  GEARMAN_PACKET_FREE_DATA= (1 << 2)
} gearman_packet_options_t;

/**
 * @ingroup gearman_packet
 * Magic types.
 */
typedef enum
{
  GEARMAN_MAGIC_TEXT,
  GEARMAN_MAGIC_REQUEST,
  GEARMAN_MAGIC_RESPONSE
} gearman_magic_t;

/**
 * @ingroup gearman_packet
 * Command types. When you add a new entry, update gearman_command_info_list in
 * packet.c as well.
 */
typedef enum
{
  GEARMAN_COMMAND_TEXT,
  GEARMAN_COMMAND_CAN_DO,
  GEARMAN_COMMAND_CANT_DO,
  GEARMAN_COMMAND_RESET_ABILITIES,
  GEARMAN_COMMAND_PRE_SLEEP,
  GEARMAN_COMMAND_UNUSED,
  GEARMAN_COMMAND_NOOP,
  GEARMAN_COMMAND_SUBMIT_JOB,
  GEARMAN_COMMAND_JOB_CREATED,
  GEARMAN_COMMAND_GRAB_JOB,
  GEARMAN_COMMAND_NO_JOB,
  GEARMAN_COMMAND_JOB_ASSIGN,
  GEARMAN_COMMAND_WORK_STATUS,
  GEARMAN_COMMAND_WORK_COMPLETE,
  GEARMAN_COMMAND_WORK_FAIL,
  GEARMAN_COMMAND_GET_STATUS,
  GEARMAN_COMMAND_ECHO_REQ,
  GEARMAN_COMMAND_ECHO_RES,
  GEARMAN_COMMAND_SUBMIT_JOB_BG,
  GEARMAN_COMMAND_ERROR,
  GEARMAN_COMMAND_STATUS_RES,
  GEARMAN_COMMAND_SUBMIT_JOB_HIGH,
  GEARMAN_COMMAND_SET_CLIENT_ID,
  GEARMAN_COMMAND_CAN_DO_TIMEOUT,
  GEARMAN_COMMAND_ALL_YOURS,
  GEARMAN_COMMAND_WORK_EXCEPTION,
  GEARMAN_COMMAND_OPTION_REQ,
  GEARMAN_COMMAND_OPTION_RES,
  GEARMAN_COMMAND_WORK_DATA,
  GEARMAN_COMMAND_WORK_WARNING,
  GEARMAN_COMMAND_GRAB_JOB_UNIQ,
  GEARMAN_COMMAND_JOB_ASSIGN_UNIQ,
  GEARMAN_COMMAND_SUBMIT_JOB_HIGH_BG,
  GEARMAN_COMMAND_SUBMIT_JOB_LOW,
  GEARMAN_COMMAND_SUBMIT_JOB_LOW_BG,
  GEARMAN_COMMAND_SUBMIT_JOB_SCHED,
  GEARMAN_COMMAND_SUBMIT_JOB_EPOCH,
  GEARMAN_COMMAND_MAX /* Always add new commands before this. */
} gearman_command_t;

/**
 * @ingroup gearman_task
 * Options for gearman_task_st.
 */
typedef enum
{
  GEARMAN_TASK_ALLOCATED=   (1 << 0),
  GEARMAN_TASK_SEND_IN_USE= (1 << 1)
} gearman_task_options_t;

/**
 * @ingroup gearman_task
 * States for gearman_task_st.
 */
typedef enum
{
  GEARMAN_TASK_STATE_NEW,
  GEARMAN_TASK_STATE_SUBMIT,
  GEARMAN_TASK_STATE_WORKLOAD,
  GEARMAN_TASK_STATE_WORK,
  GEARMAN_TASK_STATE_CREATED,
  GEARMAN_TASK_STATE_DATA,
  GEARMAN_TASK_STATE_WARNING,
  GEARMAN_TASK_STATE_STATUS,
  GEARMAN_TASK_STATE_COMPLETE,
  GEARMAN_TASK_STATE_EXCEPTION,
  GEARMAN_TASK_STATE_FAIL,
  GEARMAN_TASK_STATE_FINISHED
} gearman_task_state_t;

/**
 * @ingroup gearman_job
 * Options for gearman_job_st.
 */
typedef enum
{
  GEARMAN_JOB_ALLOCATED=       (1 << 0),
  GEARMAN_JOB_ASSIGNED_IN_USE= (1 << 1),
  GEARMAN_JOB_WORK_IN_USE=     (1 << 2),
  GEARMAN_JOB_FINISHED=        (1 << 3)
} gearman_job_options_t;

/**
 * @ingroup gearman_job
 * Priority levels for a job.
 */
typedef enum
{
  GEARMAN_JOB_PRIORITY_HIGH,
  GEARMAN_JOB_PRIORITY_NORMAL,
  GEARMAN_JOB_PRIORITY_LOW,
  GEARMAN_JOB_PRIORITY_MAX /* Always add new commands before this. */
} gearman_job_priority_t;

/**
 * @ingroup gearman_client
 * Options for gearman_client_st.
 */
typedef enum
{
  GEARMAN_CLIENT_ALLOCATED=         (1 << 0),
  GEARMAN_CLIENT_NON_BLOCKING=      (1 << 1),
  GEARMAN_CLIENT_TASK_IN_USE=       (1 << 2),
  GEARMAN_CLIENT_UNBUFFERED_RESULT= (1 << 3),
  GEARMAN_CLIENT_NO_NEW=            (1 << 4),
  GEARMAN_CLIENT_FREE_TASKS=        (1 << 5)
} gearman_client_options_t;

/**
 * @ingroup gearman_client
 * States for gearman_client_st.
 */
typedef enum
{
  GEARMAN_CLIENT_STATE_IDLE,
  GEARMAN_CLIENT_STATE_NEW,
  GEARMAN_CLIENT_STATE_SUBMIT,
  GEARMAN_CLIENT_STATE_PACKET
} gearman_client_state_t;

/**
 * @ingroup gearman_worker
 * Options for gearman_worker_st.
 */
typedef enum
{
  GEARMAN_WORKER_ALLOCATED=        (1 << 0),
  GEARMAN_WORKER_NON_BLOCKING=     (1 << 1),
  GEARMAN_WORKER_PACKET_INIT=      (1 << 2),
  GEARMAN_WORKER_GRAB_JOB_IN_USE=  (1 << 3),
  GEARMAN_WORKER_PRE_SLEEP_IN_USE= (1 << 4),
  GEARMAN_WORKER_WORK_JOB_IN_USE=  (1 << 5),
  GEARMAN_WORKER_CHANGE=           (1 << 6),
  GEARMAN_WORKER_GRAB_UNIQ=        (1 << 7)
} gearman_worker_options_t;

/**
 * @ingroup gearman_worker
 * States for gearman_worker_st.
 */
typedef enum
{
  GEARMAN_WORKER_STATE_START,
  GEARMAN_WORKER_STATE_FUNCTION_SEND,
  GEARMAN_WORKER_STATE_CONNECT,
  GEARMAN_WORKER_STATE_GRAB_JOB_SEND,
  GEARMAN_WORKER_STATE_GRAB_JOB_RECV,
  GEARMAN_WORKER_STATE_PRE_SLEEP
} gearman_worker_state_t;

/**
 * @ingroup gearman_worker
 * Options for gearman_worker_function_st.
 */
typedef enum
{
  GEARMAN_WORKER_FUNCTION_PACKET_IN_USE= (1 << 0),
  GEARMAN_WORKER_FUNCTION_CHANGE=        (1 << 1),
  GEARMAN_WORKER_FUNCTION_REMOVE=        (1 << 2)
} gearman_worker_function_options_t;

/**
 * @ingroup gearman_worker
 * Work states for gearman_worker_st.
 */
typedef enum
{
  GEARMAN_WORKER_WORK_STATE_GRAB_JOB,
  GEARMAN_WORKER_WORK_STATE_FUNCTION,
  GEARMAN_WORKER_WORK_STATE_COMPLETE,
  GEARMAN_WORKER_WORK_STATE_FAIL
} gearman_worker_work_state_t;

/**
 * @ingroup gearman_server
 * Options for gearman_server_st.
 */
typedef enum
{
  GEARMAN_SERVER_ALLOCATED=    (1 << 0),
  GEARMAN_SERVER_PROC_THREAD=  (1 << 1),
  GEARMAN_SERVER_QUEUE_REPLAY= (1 << 2)
} gearman_server_options_t;

/**
 * @ingroup gearman_server_thread
 * Options for gearman_server_thread_st.
 */
typedef enum
{
  GEARMAN_SERVER_THREAD_ALLOCATED= (1 << 0)
} gearman_server_thread_options_t;

/**
 * @ingroup gearman_server_con
 * Options for gearman_server_con_st.
 */
typedef enum
{
  GEARMAN_SERVER_CON_SLEEPING=   (1 << 0),
  GEARMAN_SERVER_CON_EXCEPTIONS= (1 << 1),
  GEARMAN_SERVER_CON_DEAD=       (1 << 2)
} gearman_server_con_options_t;

/**
 * @ingroup gearman_server_function
 * Options for gearman_server_function_st.
 */
typedef enum
{
  GEARMAN_SERVER_FUNCTION_ALLOCATED= (1 << 0)
} gearman_server_function_options_t;

/**
 * @ingroup gearman_server_client
 * Options for gearman_server_client_st.
 */
typedef enum
{
  GEARMAN_SERVER_CLIENT_ALLOCATED= (1 << 0)
} gearman_server_client_options_t;

/**
 * @ingroup gearman_server_worker
 * Options for gearman_server_worker_st.
 */
typedef enum
{
  GEARMAN_SERVER_WORKER_ALLOCATED= (1 << 0)
} gearman_server_worker_options_t;

/**
 * @ingroup gearman_server_job
 * Options for gearman_server_job_st.
 */
typedef enum
{
  GEARMAN_SERVER_JOB_ALLOCATED= (1 << 0),
  GEARMAN_SERVER_JOB_QUEUED=    (1 << 1),
  GEARMAN_SERVER_JOB_IGNORE=    (1 << 2)
} gearman_server_job_options_t;

/**
 * @ingroup gearmand
 * Options for gearmand_st.
 */
typedef enum
{
  GEARMAND_LISTEN_EVENT= (1 << 0),
  GEARMAND_WAKEUP_EVENT= (1 << 1)
} gearmand_options_t;

/**
 * @ingroup gearmand
 * Wakeup events for gearmand_st.
 */
typedef enum
{
  GEARMAND_WAKEUP_PAUSE=             (1 << 0),
  GEARMAND_WAKEUP_SHUTDOWN=          (1 << 1),
  GEARMAND_WAKEUP_SHUTDOWN_GRACEFUL= (1 << 2),
  GEARMAND_WAKEUP_CON=               (1 << 3),
  GEARMAND_WAKEUP_RUN=               (1 << 4)
} gearmand_wakeup_t;

/**
 * @ingroup gearmand_thread
 * Options for gearmand_thread_st.
 */
typedef enum
{
  GEARMAND_THREAD_WAKEUP_EVENT= (1 << 0),
  GEARMAND_THREAD_LOCK=         (1 << 1)
} gearmand_thread_options_t;

/**
 * @ingroup gearman_conf
 * Options for gearman_conf_st.
 */
typedef enum
{
  GEARMAN_CONF_ALLOCATED= (1 << 0)
} gearman_conf_options_t;

/**
 * @ingroup gearman_conf_module
 * Options for gearman_conf_module_st.
 */
typedef enum
{
  GEARMAN_CONF_MODULE_ALLOCATED= (1 << 0)
} gearman_conf_module_options_t;


/**
 * @addtogroup gearman_constants Gearman Constants
 * @{
 */

/* Function types. */
typedef gearman_return_t (gearman_workload_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_created_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_data_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_warning_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_status_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_complete_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_exception_fn)(gearman_task_st *task);
typedef gearman_return_t (gearman_fail_fn)(gearman_task_st *task);

typedef gearman_return_t (gearman_parse_server_fn)(const char *host,
                                                   in_port_t port, void *data);

typedef void* (gearman_worker_fn)(gearman_job_st *job, void *fn_arg,
                                  size_t *result_size,
                                  gearman_return_t *ret_ptr);

typedef gearman_return_t (gearman_event_watch_fn)(gearman_con_st *con,
                                                  short events, void *arg);

typedef void* (gearman_malloc_fn)(size_t size, void *arg);
typedef void (gearman_free_fn)(void *ptr, void *arg);

typedef void (gearman_task_fn_arg_free_fn)(gearman_task_st *task, void *fn_arg);

typedef void (gearman_log_fn)(gearman_st *gearman, gearman_verbose_t verbose,
                              const char *line, void *fn_arg);
typedef void (gearman_server_log_fn)(gearman_server_st *server,
                                     gearman_verbose_t verbose,
                                     const char *line, void *fn_arg);
typedef void (gearman_server_thread_log_fn)(gearman_server_thread_st *thread,
                                            gearman_verbose_t verbose,
                                            const char *line, void *fn_arg);
typedef void (gearmand_log_fn)(gearmand_st *gearmand, gearman_verbose_t verbose,
                               const char *line, void *fn_arg);

typedef void (gearman_server_thread_run_fn)(gearman_server_thread_st *thread,
                                            void *fn_arg);

typedef gearman_return_t (gearman_con_add_fn)(gearman_con_st *con);

typedef void (gearman_con_protocol_data_free_fn)(gearman_con_st *con,
                                                 void *data);

typedef gearman_packet_st* (gearman_con_recv_fn)(gearman_con_st *con,
                                                 gearman_packet_st *packet,
                                                 gearman_return_t *ret_ptr,
                                                 bool recv_data);
typedef size_t (gearman_con_recv_data_fn)(gearman_con_st *con, void *data,
                                          size_t data_size,
                                          gearman_return_t *ret_ptr);

typedef gearman_return_t (gearman_con_send_fn)(gearman_con_st *con,
                                               gearman_packet_st *packet,
                                               bool flush);
typedef size_t (gearman_con_send_data_fn)(gearman_con_st *con, const void *data,
                                          size_t data_size,
                                          gearman_return_t *ret_ptr);

typedef size_t (gearman_packet_pack_fn)(gearman_packet_st *packet,
                                        gearman_con_st *con,
                                        void *data, size_t data_size,
                                        gearman_return_t *ret_ptr);
typedef size_t (gearman_packet_unpack_fn)(gearman_packet_st *packet,
                                          gearman_con_st *con, const void *data,
                                          size_t data_size,
                                          gearman_return_t *ret_ptr);

typedef gearman_return_t (gearman_queue_add_fn)(gearman_st *gearman,
                                                void *fn_arg,
                                                const void *unique,
                                                size_t unique_size,
                                                const void *function_name,
                                                size_t function_name_size,
                                                const void *data,
                                                size_t data_size,
                                               gearman_job_priority_t priority);
typedef gearman_return_t (gearman_queue_flush_fn)(gearman_st *gearman,
                                                  void *fn_arg);
typedef gearman_return_t (gearman_queue_done_fn)(gearman_st *gearman,
                                                 void *fn_arg,
                                                 const void *unique,
                                                 size_t unique_size,
                                                 const void *function_name,
                                                 size_t function_name_size);
typedef gearman_return_t (gearman_queue_replay_fn)(gearman_st *gearman,
                                                   void *fn_arg,
                                                   gearman_queue_add_fn *add_fn,
                                                   void *add_fn_arg);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __GEARMAN_CONSTANTS_H__ */
