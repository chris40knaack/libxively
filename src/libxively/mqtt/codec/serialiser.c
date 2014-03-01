#include <string.h>

#include "message.h"

#include "serialiser.h"

#define WRITE_STRING(name) { \
  buffer[offset++] = name.length / 0xff; \
  buffer[offset++] = name.length & 0xff; \
  memcpy(&(buffer[offset]), name.data, name.length); \
  offset += name.length; \
}

void mqtt_serialiser_init( mqtt_serialiser_t* serialiser )
{
  memset( serialiser, 0, sizeof ( mqtt_serialiser_t ) );
}

size_t mqtt_serialiser_size(
      mqtt_serialiser_t* serialiser
    , const mqtt_message_t* message )
{
    size_t len = 1;

    if ( message->common.common_u.common_bits.type == MQTT_TYPE_CONNECT )
    {
        len += 8; // protocol name
        len += 1; // protocol version
        len += 1; // connect flags
        len += 2; // keep alive timer

        len += 2; // size
        len += message->connect.client_id.length;

        if ( message->connect.flags_u.flags_bits.username_follows )
        {
            len += 2;
            len += message->connect.username.length;
        }

        if ( message->connect.flags_u.flags_bits.password_follows )
        {
            len += 2;
            len += message->connect.password.length;
        }

        if ( message->connect.flags_u.flags_bits.will )
        {
            len += 4;
            len += message->connect.will_topic.length;
            len += message->connect.will_message.length;
        }
    }
    else if ( message->common.common_u.common_bits.type == MQTT_TYPE_CONNACK )
    {
        len += 2;
    }

    int32_t remaining_length = len - 1;

    if ( remaining_length <= 127 )
    {
        len += 1;
    }
    else if ( remaining_length <= 16383 )
    {
        len += 2;
    }
    else if ( remaining_length <= 2097151 )
    {
        len += 3;
    }
    else if ( remaining_length <= 268435455 )
    {
        len += 4;
    }

    return len;
}

mqtt_serialiser_rc_t mqtt_serialiser_write(
      mqtt_serialiser_t* serialiser
    , const mqtt_message_t* message
    , uint8_t* buffer
    , size_t len )
{
    unsigned int offset = 0;

    buffer[ offset++ ] = message->common.common_u.common_value;

    uint32_t remaining_length = len - 2;//message->common.remaining_length;

    do
    {
        buffer[ offset++ ] = remaining_length & 0x7f;
        remaining_length >>= 7;
    } while ( remaining_length > 0 );

    switch ( message->common.common_u.common_bits.type )
    {
        case MQTT_TYPE_CONNECT:
        {
            WRITE_STRING( message->connect.protocol_name );

            buffer[ offset++ ] = message->connect.protocol_version;
            buffer[ offset++ ] = message->connect.flags_u.flags_value;
            buffer[ offset++ ] = message->connect.keep_alive >> 8;
            buffer[ offset++ ] = message->connect.keep_alive & 0xff;

            WRITE_STRING( message->connect.client_id );

            if ( message->connect.flags_u.flags_bits.will )
            {
                WRITE_STRING( message->connect.will_topic );
                WRITE_STRING( message->connect.will_message );
            }

            if ( message->connect.flags_u.flags_bits.username_follows )
            {
                WRITE_STRING( message->connect.username );
            }

            if ( message->connect.flags_u.flags_bits.password_follows )
            {
                WRITE_STRING( message->connect.password );
            }

            break;
        }

        case MQTT_TYPE_CONNACK:
        {
            buffer[ offset++ ] = message->connack._unused;
            buffer[ offset++ ] = message->connack.return_code;

            break;
        }

        default:
        {
            serialiser->error = MQTT_ERROR_SERIALISER_INVALID_MESSAGE_ID;

            return MQTT_SERIALISER_RC_ERROR;
        }
    }

    return MQTT_SERIALISER_RC_SUCCESS;
}
