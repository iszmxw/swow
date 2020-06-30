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

#ifndef SWOW_HTTP_H
#define SWOW_HTTP_H

#include "swow.h"

#include "cat_http.h"

extern SWOW_API zend_class_entry *swow_http_status_ce;

extern SWOW_API zend_class_entry *swow_http_parser_ce;
extern SWOW_API zend_object_handlers swow_http_parser_handlers;
extern SWOW_API zend_class_entry *swow_http_parser_exception_ce;

typedef struct
{
    cat_http_parser_t parser;
    size_t data_offset;
    size_t parsed_length;
    zend_object std;
} swow_http_parser_t;

/* loader */

int swow_http_module_init(INIT_FUNC_ARGS);

/* helper*/

static cat_always_inline swow_http_parser_t *swow_http_parser_get_from_handle(cat_http_parser_t *parser)
{
    return cat_container_of(parser, swow_http_parser_t, parser);
}

static cat_always_inline swow_http_parser_t *swow_http_parser_get_from_object(zend_object *object)
{
    return (swow_http_parser_t *) ((char *) object - swow_http_parser_handlers.offset);
}

#endif  /* SWOW_HTTP_H */
