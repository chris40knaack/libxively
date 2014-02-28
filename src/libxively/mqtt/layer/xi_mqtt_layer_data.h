// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#ifndef __MQTT_LAYER_DATA_H__
#define __MQTT_LAYER_DATA_H__

#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    mqtt_parser_t   parser;
    mqtt_message_t  msg;
} xi_mqtt_layer_data_t;

#ifdef __cplusplus
}
#endif

#endif // __MQTT_LAYER_DATA_H__