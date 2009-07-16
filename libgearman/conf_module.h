/* Module configuration library
 * Copyright (C) 2009 Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief Gearman conf module declarations
 */

#ifndef __GEARMAN_CONF_MODULE_H__
#define __GEARMAN_CONF_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup gearman_conf_module Gearman conf module interface
 * @{
 */

/**
 * Initialize a gearman conf module structure.
 */
GEARMAN_API
gearman_conf_module_st *gearman_conf_module_create(gearman_conf_st *conf,
                                                 gearman_conf_module_st *module,
                                                 const char *name);

/**
 * Free a gearman conf module structure.
 */
GEARMAN_API
void gearman_conf_module_free(gearman_conf_module_st *module);

/**
 * Find a gearman conf module structure by name.
 */
GEARMAN_API
gearman_conf_module_st *gearman_conf_module_find(gearman_conf_st *conf,
                                                 const char *name);

/**
 * Add option for a module.
 */
GEARMAN_API
void gearman_conf_module_add_option(gearman_conf_module_st *module,
                                    const char *name, int short_name,
                                    const char *value_name, const char *help);

/**
 * Loop through all values that were given for a set of module options.
 */
GEARMAN_API
bool gearman_conf_module_value(gearman_conf_module_st *module,
                               const char **name, const char **value);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __GEARMAN_CONF_MODULE_H__ */
