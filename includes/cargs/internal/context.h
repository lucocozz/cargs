/**
 * cargs/internal/context.h - Internal context management
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines functions for managing the internal context
 * state during parsing.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef CARGS_INTERNAL_CONTEXT_H
#define CARGS_INTERNAL_CONTEXT_H

#include "cargs/types.h"

/**
 * context_init - Initialize the context
 *
 * @param cargs  Cargs context
 */
void context_init(cargs_t *cargs);

/**
 * Subcommand stack management
 */
void                  context_init_subcommands(cargs_t *cargs);
const cargs_option_t *context_get_subcommand(cargs_t *cargs);
void                  context_push_subcommand(cargs_t *cargs, const cargs_option_t *cmd);
const cargs_option_t *context_pop_subcommand(cargs_t *cargs);

/**
 * Option context management
 */
void context_set_option(cargs_t *cargs, cargs_option_t *option);
void context_unset_option(cargs_t *cargs);

/**
 * Group context management
 */
void context_set_group(cargs_t *cargs, cargs_option_t *group);
void context_unset_group(cargs_t *cargs);

#endif /* CARGS_INTERNAL_CONTEXT_H */
