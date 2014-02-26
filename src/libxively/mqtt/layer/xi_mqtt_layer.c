// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include "xi_mqtt_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

layer_state_t xi_mqtt_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    return LAYER_STATE_OK;
}

layer_state_t xi_mqtt_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    return LAYER_STATE_OK;
}

layer_state_t xi_mqtt_layer_close(
    layer_connectivity_t* context )
{
    return LAYER_STATE_OK;
}

layer_state_t xi_mqtt_layer_on_close(
    layer_connectivity_t* context )
{
    return LAYER_STATE_OK;
}

#ifdef __cplusplus
}
#endif
