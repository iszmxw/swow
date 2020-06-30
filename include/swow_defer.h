/*
  +--------------------------------------------------------------------------+
  | Swow                                                                     |
  +--------------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");          |
  | you may not use this file except in compliance with the License.         |
  | You may obtain a copy of the License at                                  |
  | http://www.apache.org/licenses/LICENSE-2.0                               |
  | Unless required by applicable law or agreed to in writing, software      |
  | distributed under the License is distributed on an "AS IS" BASIS,        |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
  | See the License for the specific language governing permissions and      |
  | limitations under the License. See accompanying LICENSE file.            |
  +--------------------------------------------------------------------------+
  | Author: Twosee <twose@qq.com>                                            |
  +--------------------------------------------------------------------------+
 */

#ifndef SWOW_DEFER_H
#define SWOW_DEFER_H

#include "swow.h"

#include "cat_queue.h"

extern SWOW_API zend_class_entry *swow_defer_ce;
extern SWOW_API zend_object_handlers swow_defer_handlers;

typedef struct
{
    cat_queue_t node;
    zval zcallable;
    zend_fcall_info_cache fcc;
} swow_defer_task_t;

typedef struct
{
    cat_queue_t tasks;
    zend_object std;
} swow_defer_t;

/* loader */

int swow_defer_module_init(INIT_FUNC_ARGS);

/* functions */

SWOW_API cat_bool_t swow_defer(zval *zcallable);
SWOW_API void swow_defer_do_tasks(swow_defer_t *sdefer);
SWOW_API void swow_defer_do_main_tasks(void); SWOW_INTERNAL

/* helper*/

static cat_always_inline swow_defer_t *swow_defer_get_from_object(zend_object *object)
{
    return (swow_defer_t *) ((char *) object - swow_defer_handlers.offset);
}

#endif	/* SWOW_DEFER_H */
