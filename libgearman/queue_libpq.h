/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief libpq Queue Storage Declarations
 */

#ifndef __GEARMAN_QUEUE_LIBPQ_H__
#define __GEARMAN_QUEUE_LIBPQ_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup gearman_queue_libpq libpq Queue Storage Functions
 * @ingroup gearman_queue
 * @{
 */

/**
 * Get module configuration options.
 */
GEARMAN_API
gearman_return_t gearman_queue_libpq_conf(gearman_conf_st *conf);

/**
 * Initialize the queue.
 */
GEARMAN_API
gearman_return_t gearman_queue_libpq_init(gearman_st *gearman,
                                          gearman_conf_st *conf);

/**
 * De-initialize the queue.
 */
GEARMAN_API
gearman_return_t gearman_queue_libpq_deinit(gearman_st *gearman);

/**
 * Initialize the queue for a gearmand object.
 */
GEARMAN_API
gearman_return_t gearmand_queue_libpq_init(gearmand_st *gearmand,
                                           gearman_conf_st *conf);

/**
 * De-initialize the queue for a gearmand object.
 */
GEARMAN_API
gearman_return_t gearmand_queue_libpq_deinit(gearmand_st *gearmand);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __GEARMAN_QUEUE_LIBPQ_H__ */
