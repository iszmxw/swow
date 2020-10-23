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
  | Author: Twosee <twosee@php.net>                                          |
  +--------------------------------------------------------------------------+
 */

#include "swow_websocket.h"

SWOW_API zend_class_entry *swow_websocket_opcode_ce;

SWOW_API zend_class_entry *swow_websocket_status_ce;

SWOW_API zend_class_entry *swow_websocket_frame_ce;
SWOW_API zend_object_handlers swow_websocket_frame_handlers;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_opcode_getName, ZEND_RETURN_VALUE, 1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, opcode, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_opcode, getName)
{
    zend_long opcode;
    const char *name;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(opcode)
    ZEND_PARSE_PARAMETERS_END();

    name = cat_websocket_opcode_name(opcode);

    RETURN_STRING(name);
}

static const zend_function_entry swow_websocket_opcode_methods[] = {
    PHP_ME(swow_websocket_opcode, getName, arginfo_swow_websocket_opcode_getName, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_status_getDescription, ZEND_RETURN_VALUE, 1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, code, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_status, getDescription)
{
    zend_long code;
    const char *description;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(code)
    ZEND_PARSE_PARAMETERS_END();

    description = cat_websocket_status_get_description(code);

    RETURN_STRING(description);
}

static const zend_function_entry swow_websocket_status_methods[] = {
    PHP_ME(swow_websocket_status, getDescription, arginfo_swow_websocket_status_getDescription, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

static zend_object *swow_websocket_frame_create_object(zend_class_entry *ce)
{
    swow_websocket_frame_t *sframe = swow_object_alloc(swow_websocket_frame_t, ce, swow_websocket_frame_handlers);

    (void) cat_websocket_header_init(&sframe->header);
    sframe->payload_data = NULL;

    return &sframe->std;
}

#define getThisFrame() (swow_websocket_frame_get_from_object(Z_OBJ_P(ZEND_THIS)))

#define SWOW_WEBSOCKET_FRAME_GETTER(_sframe, _header) \
    swow_websocket_frame_t *_sframe = getThisFrame(); \
    cat_websocket_header_t *_header = &_sframe->header

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_frame_unpackHeader, ZEND_RETURN_VALUE, 1, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, buffer, Swow\\Buffer, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, unpackHeader)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zval *zbuffer;
    swow_buffer_t *sbuffer;
    const char *data;
    size_t length;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zbuffer, swow_buffer_ce)
    ZEND_PARSE_PARAMETERS_END();

    sbuffer = swow_buffer_get_from_object(Z_OBJ_P(zbuffer));
    length = swow_buffer_get_readable_space(sbuffer, &data);

    length = cat_websocket_header_unpack(header, data, length);

    swow_buffer_virtual_read(sbuffer, length);

    RETURN_LONG(length);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_resetHeader, ZEND_RETURN_VALUE, 0, Swow\\WebSocket\\Frame, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, resetHeader)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    cat_websocket_header_init(header);

    RETURN_THIS();
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_frame_getLong, ZEND_RETURN_VALUE, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_swow_websocket_frame_getOpcode arginfo_swow_websocket_frame_getLong

static PHP_METHOD(swow_websocket_frame, getOpcode)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(header->opcode);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setOpcode, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, opcode, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setOpcode)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_long opcode;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(opcode)
    ZEND_PARSE_PARAMETERS_END();

    header->opcode = opcode;

    RETURN_THIS();
}

#define arginfo_swow_websocket_frame_getFin arginfo_swow_websocket_frame_getBool

static PHP_METHOD(swow_websocket_frame, getFin)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(header->fin);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setFin, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, fin, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setFin)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_bool fin;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(fin)
    ZEND_PARSE_PARAMETERS_END();

    header->fin = fin;

    RETURN_THIS();
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_frame_getBool, ZEND_RETURN_VALUE, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_swow_websocket_frame_getRSV1 arginfo_swow_websocket_frame_getBool

static PHP_METHOD(swow_websocket_frame, getRSV1)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(header->rsv1);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setRSV1, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, rsv1, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setRSV1)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_bool rsv1;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(rsv1)
    ZEND_PARSE_PARAMETERS_END();

    header->rsv1 = rsv1;

    RETURN_THIS();
}

#define arginfo_swow_websocket_frame_getRSV2 arginfo_swow_websocket_frame_getBool

static PHP_METHOD(swow_websocket_frame, getRSV2)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(header->rsv2);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setRSV2, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, rsv2, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setRSV2)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_bool rsv2;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(rsv2)
    ZEND_PARSE_PARAMETERS_END();

    header->rsv2 = rsv2;

    RETURN_THIS();
}

#define arginfo_swow_websocket_frame_getRSV3 arginfo_swow_websocket_frame_getBool

static PHP_METHOD(swow_websocket_frame, getRSV3)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(header->rsv3);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setRSV3, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, rsv3, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setRSV3)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_bool rsv3;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(rsv3)
    ZEND_PARSE_PARAMETERS_END();

    header->rsv3 = rsv3;

    RETURN_THIS();
}

#define arginfo_swow_websocket_frame_getHeaderLength arginfo_swow_websocket_frame_getLong

static PHP_METHOD(swow_websocket_frame, getHeaderLength)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_LONG(cat_websocket_header_get_length(header));
}

#define arginfo_swow_websocket_frame_getPayloadLength arginfo_swow_websocket_frame_getLong

static PHP_METHOD(swow_websocket_frame, getPayloadLength)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    if (sframe->payload_data != NULL) {
        swow_buffer_t *sbuffer = swow_buffer_get_from_object(sframe->payload_data);
        RETURN_LONG(sbuffer->buffer.length);
    }

    RETURN_LONG(header->payload_length);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setPayloadLength, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, payloadLength, IS_LONG, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setPayloadLength)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_long payload_length;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(payload_length)
    ZEND_PARSE_PARAMETERS_END();

    header->payload_length =  payload_length;

    RETURN_THIS();
}

#define arginfo_swow_websocket_frame_getMask arginfo_swow_websocket_frame_getBool

static PHP_METHOD(swow_websocket_frame, getMask)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(header->mask);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setMask, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, mask, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setMask)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_bool mask;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(mask)
    ZEND_PARSE_PARAMETERS_END();

    header->mask = mask;

    RETURN_THIS();
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_frame_getString, ZEND_RETURN_VALUE, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_swow_websocket_frame_getMaskKey arginfo_swow_websocket_frame_getString

static PHP_METHOD(swow_websocket_frame, getMaskKey)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);

    ZEND_PARSE_PARAMETERS_NONE();

    if (!header->mask) {
        RETURN_EMPTY_STRING();
    }

    RETURN_STRINGL(header->mask_key, sizeof(header->mask_key));
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setMaskKey, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_TYPE_INFO(0, maskKey, IS_STRING, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setMaskKey)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_string *mask_key;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(mask_key)
    ZEND_PARSE_PARAMETERS_END();

    if (ZSTR_LEN(mask_key) == 0) {
        header->mask = 0;
    } else {
        if (UNEXPECTED(ZSTR_LEN(mask_key) != CAT_WEBSOCKET_MASK_KEY_LENGTH)) {
            zend_argument_value_error(1, "length should be %u", CAT_WEBSOCKET_MASK_KEY_LENGTH);
        }
        header->mask = 1;
        memcpy(header->mask_key, ZSTR_VAL(mask_key), CAT_WEBSOCKET_MASK_KEY_LENGTH);
    }

    RETURN_THIS();
}

#define arginfo_swow_websocket_frame_hasPayloadData arginfo_swow_websocket_frame_getBool

static PHP_METHOD(swow_websocket_frame, hasPayloadData)
{
    swow_websocket_frame_t *sframe = getThisFrame();

    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_BOOL(sframe->payload_data != NULL);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_getPayloadData, ZEND_RETURN_VALUE, 0, Swow\\Buffer, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, getPayloadData)
{
    swow_websocket_frame_t *sframe = getThisFrame();

    ZEND_PARSE_PARAMETERS_NONE();

    if (UNEXPECTED(sframe->payload_data == NULL)) {
        sframe->payload_data = swow_object_create(swow_buffer_ce);
    }
    GC_ADDREF(sframe->payload_data);

    RETURN_OBJ(sframe->payload_data);
}

#define arginfo_swow_websocket_frame_getPayloadDataAsString arginfo_swow_websocket_frame_getString

static PHP_METHOD(swow_websocket_frame, getPayloadDataAsString)
{
    swow_websocket_frame_t *sframe = getThisFrame();
    zend_string *payload_data;

    ZEND_PARSE_PARAMETERS_NONE();

    if (sframe->payload_data == NULL) {
        RETURN_EMPTY_STRING();
    }

    payload_data = swow_buffer_fetch_string(swow_buffer_get_from_object(sframe->payload_data));
    GC_ADDREF(payload_data);

    RETURN_STR(payload_data);
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_setPayloadData, ZEND_RETURN_VALUE, 1, Swow\\WebSocket\\Frame, 0)
    ZEND_ARG_OBJ_INFO(0, buffer, Swow\\Buffer, 1)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, setPayloadData)
{
    swow_websocket_frame_t *sframe = getThisFrame();
    zval *zbuffer;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS_EX(zbuffer, swow_buffer_ce, 1, 0)
    ZEND_PARSE_PARAMETERS_END();

    if (UNEXPECTED(sframe->payload_data != NULL)) {
        OBJ_RELEASE(sframe->payload_data);
    }

    if (zbuffer) {
        Z_ADDREF_P(zbuffer);
        sframe->payload_data = Z_OBJ_P(zbuffer);
    } else {
        sframe->payload_data = NULL;
    }

    RETURN_THIS();
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_swow_websocket_frame_unmaskPayloadData, ZEND_RETURN_VALUE, 0, Swow\\WebSocket\\Frame, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, unmaskPayloadData)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    swow_buffer_t *payload_data;

    ZEND_PARSE_PARAMETERS_NONE();

    if (!header->mask) {
        return;
    }
    if (UNEXPECTED(sframe->payload_data == NULL)) {
        return;
    }
    payload_data = swow_buffer_get_from_object(sframe->payload_data);

    cat_websocket_unmask(
        payload_data->buffer.value,
        payload_data->buffer.length,
        header->mask_key
    );
    header->mask = 0;

    RETURN_THIS();
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_frame_toString, ZEND_RETURN_VALUE, 0, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, headerOnly, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, toString)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zend_bool header_only = 0;
    zend_string *string;
    uint8_t header_length;
    char *payload_data;
    uint64_t payload_length;
    char *p;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(header_only)
    ZEND_PARSE_PARAMETERS_END();

    if (sframe->payload_data != NULL) {
        swow_buffer_t *sbuffer = swow_buffer_get_from_object(sframe->payload_data);
        payload_data = sbuffer->buffer.value;
        payload_length = sbuffer->buffer.length;
    } else {
        payload_data = NULL;
        payload_length = 0;
    }

    /* we must update the payload length first
     * it may change the header length */
    header->payload_length = payload_length;
    header_length = cat_websocket_header_get_length(header);

    if (header_only) {
        payload_length = 0;
    }

    string = zend_string_alloc(header_length + payload_length, 0);
    p = ZSTR_VAL(string);

    cat_websocket_header_pack(header, p, header_length);
    p += header_length;

    if (payload_length != 0) {
        if (!header->mask) {
            memcpy(p, payload_data, payload_length);
        } else {
            cat_websocket_mask(p, payload_data, payload_length, header->mask_key);
        }
        p += payload_length;
    }

    *p = '\0';

    RETURN_STR(string);
}

#define arginfo_swow_websocket_frame___toString arginfo_swow_websocket_frame_getString

#define zim_swow_websocket_frame___toString zim_swow_websocket_frame_toString

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_swow_websocket_frame___debugInfo, ZEND_RETURN_VALUE, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(swow_websocket_frame, __debugInfo)
{
    SWOW_WEBSOCKET_FRAME_GETTER(sframe, header);
    zval zdebug_info;

    ZEND_PARSE_PARAMETERS_NONE();

    array_init(&zdebug_info);
    add_assoc_long(&zdebug_info, "opcode", header->opcode);
    add_assoc_bool(&zdebug_info, "fin", header->fin);
    add_assoc_bool(&zdebug_info, "rsv1", header->rsv1);
    add_assoc_bool(&zdebug_info, "rsv2", header->rsv2);
    add_assoc_bool(&zdebug_info, "rsv3", header->rsv3);
    add_assoc_bool(&zdebug_info, "mask", header->mask);
    if (sframe->payload_data == NULL) {
        add_assoc_long(&zdebug_info, "payload_length", header->payload_length);
    } else {
        swow_buffer_t *sbuffer = swow_buffer_get_from_object(sframe->payload_data);
        add_assoc_long(&zdebug_info, "payload_length", sbuffer->buffer.length);
    }
    if (header->mask) {
        char *mask_key = cat_hexprint(header->mask_key, CAT_WEBSOCKET_MASK_KEY_LENGTH);
        if (mask_key != NULL) {
            add_assoc_string(&zdebug_info, "mask_key", mask_key);
            cat_free(mask_key);
        }
    }
    if (sframe->payload_data != NULL) {
        GC_ADDREF(sframe->payload_data);
        add_assoc_object(&zdebug_info, "payload_data", sframe->payload_data);
    }

    RETURN_DEBUG_INFO_WITH_PROPERTIES(&zdebug_info);
}

static const zend_function_entry swow_websocket_frame_methods[] = {
    PHP_ME(swow_websocket_frame, unpackHeader,           arginfo_swow_websocket_frame_unpackHeader,           ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, resetHeader,            arginfo_swow_websocket_frame_resetHeader,            ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getOpcode,              arginfo_swow_websocket_frame_getOpcode,              ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setOpcode,              arginfo_swow_websocket_frame_setOpcode,              ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getFin,                 arginfo_swow_websocket_frame_getFin,                 ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setFin,                 arginfo_swow_websocket_frame_setFin,                 ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getRSV1,                arginfo_swow_websocket_frame_getRSV1,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setRSV1,                arginfo_swow_websocket_frame_setRSV1,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getRSV2,                arginfo_swow_websocket_frame_getRSV2,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setRSV2,                arginfo_swow_websocket_frame_setRSV2,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getRSV3,                arginfo_swow_websocket_frame_getRSV3,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setRSV3,                arginfo_swow_websocket_frame_setRSV3,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getHeaderLength,        arginfo_swow_websocket_frame_getHeaderLength,        ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getPayloadLength,       arginfo_swow_websocket_frame_getPayloadLength,       ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setPayloadLength,       arginfo_swow_websocket_frame_setPayloadLength,       ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getMask,                arginfo_swow_websocket_frame_getMask,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setMask,                arginfo_swow_websocket_frame_setMask,                ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getMaskKey,             arginfo_swow_websocket_frame_getMaskKey,             ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setMaskKey,             arginfo_swow_websocket_frame_setMaskKey,             ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, hasPayloadData,         arginfo_swow_websocket_frame_hasPayloadData,         ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getPayloadData,         arginfo_swow_websocket_frame_getPayloadData,         ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, getPayloadDataAsString, arginfo_swow_websocket_frame_getPayloadDataAsString, ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, setPayloadData,         arginfo_swow_websocket_frame_setPayloadData,         ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, unmaskPayloadData,      arginfo_swow_websocket_frame_unmaskPayloadData,      ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, toString,               arginfo_swow_websocket_frame_toString,               ZEND_ACC_PUBLIC)
    /* magic */
    PHP_ME(swow_websocket_frame, __toString,             arginfo_swow_websocket_frame___toString,             ZEND_ACC_PUBLIC)
    PHP_ME(swow_websocket_frame, __debugInfo,            arginfo_swow_websocket_frame___debugInfo,            ZEND_ACC_PUBLIC)
    PHP_FE_END
};

int swow_websocket_module_init(INIT_FUNC_ARGS)
{
#define SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(name) \
    REGISTER_LONG_CONSTANT("Swow\\WebSocket\\" #name, CAT_WEBSOCKET_##name, CONST_CS | CONST_PERSISTENT)

#define SWOW_WEBSOCKET_REGISTER_STRING_CONSTANT(name) \
    REGISTER_STRING_CONSTANT("Swow\\WebSocket\\" #name, CAT_WEBSOCKET_##name, CONST_CS | CONST_PERSISTENT)

    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(VERSION);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(SECRET_KEY_LENGTH);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(SECRET_KEY_ENCODED_LENGTH);
    SWOW_WEBSOCKET_REGISTER_STRING_CONSTANT(GUID);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(HEADER_LENGTH);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(EXT16_LENGTH);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(EXT64_LENGTH);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(EXT8_MAX_LENGTH);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(EXT16_MAX_LENGTH);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(MASK_KEY_LENGTH);
    SWOW_WEBSOCKET_REGISTER_STRING_CONSTANT(EMPTY_MASK_KEY);
    SWOW_WEBSOCKET_REGISTER_LONG_CONSTANT(HEADER_BUFFER_SIZE);

    swow_websocket_opcode_ce = swow_register_internal_class(
        "Swow\\WebSocket\\Opcode", NULL, swow_websocket_opcode_methods,
        NULL, NULL, cat_false, cat_false, cat_false,
        swow_create_object_deny, NULL, 0
    );
#define SWOW_WEBSOCKET_OPCODE_GEN(name, value) zend_declare_class_constant_long(swow_websocket_opcode_ce, ZEND_STRL(#name), value);
    CAT_WEBSOCKET_OPCODE_MAP(SWOW_WEBSOCKET_OPCODE_GEN)
#undef SWOW_WEBSOCKET_OPCODE_GEN

    swow_websocket_status_ce = swow_register_internal_class(
        "Swow\\WebSocket\\Status", NULL, swow_websocket_status_methods,
        NULL, NULL, cat_false, cat_false, cat_false,
        swow_create_object_deny, NULL, 0
    );
#define SWOW_WEBSOCKET_STATUS_GEN(name, value, description) zend_declare_class_constant_long(swow_websocket_status_ce, ZEND_STRL(#name), value);
    CAT_WEBSOCKET_STATUS_MAP(SWOW_WEBSOCKET_STATUS_GEN)
#undef SWOW_WEBSOCKET_STATUS_GEN

    swow_websocket_frame_ce = swow_register_internal_class(
        "Swow\\WebSocket\\Frame", NULL, swow_websocket_frame_methods,
        &swow_websocket_frame_handlers, NULL,
        cat_true, cat_false, cat_false,
        swow_websocket_frame_create_object, NULL,
        XtOffsetOf(swow_websocket_frame_t, std)
    );
    do {
        cat_websocket_header_t frame = {};
        frame.fin = 1;
        frame.opcode = CAT_WEBSOCKET_OPCODE_PING;
        zend_declare_class_constant_stringl(swow_websocket_frame_ce, ZEND_STRL("PING"), (const char *) &frame, CAT_WEBSOCKET_HEADER_LENGTH);
        frame.opcode = CAT_WEBSOCKET_OPCODE_PONG;
        zend_declare_class_constant_stringl(swow_websocket_frame_ce, ZEND_STRL("PONG"), (const char *) &frame, CAT_WEBSOCKET_HEADER_LENGTH);
    } while (0);

    return SUCCESS;
}