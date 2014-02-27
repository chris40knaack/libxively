#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "xi_stated_sscanf.h"
#include "xi_coroutine.h"
#include "xi_layer_interface.h"
#include "xi_macros.h"

#include "errors.h"
#include "message.h"

#include "parser.h"

static layer_state_t read_string(
          mqtt_parser_t* parser
        , mqtt_buffer_t* dst
        , const_data_descriptor_t* src
        )
{
    char strpat[ 32 ]           = { '\0' };

    signed char sscanf_state    = 0;
    static uint16_t cs          = 0;
    const_data_descriptor_t pat;

    void* dst_o                 = { ( void* ) dst->data };

    BEGIN_CORO(cs)

    YIELD_ON( cs, ( (src->curr_pos - src->real_size) == 0 ), LAYER_STATE_WANT_READ );
    parser->str_length = (src->data_ptr[ src->curr_pos ] << 8);
    src->curr_pos += 1;

    YIELD_ON( cs, ( (src->curr_pos - src->real_size) == 0 ), LAYER_STATE_WANT_READ );
    parser->str_length += src->data_ptr[ src->curr_pos ];
    src->curr_pos += 1;

    YIELD_ON( cs, ( (src->curr_pos - src->real_size) == 0 ), LAYER_STATE_WANT_READ );

    dst->length     = parser->str_length;

    while( sscanf_state == 0 )
    {
        // @TODO this is not too optimal so let's figure that out how to deal with that
        sprintf( strpat, "%%%zu.", parser->str_length + 1 );
        pat.data_ptr    = strpat;
        pat.data_size   = strlen( strpat );
        pat.real_size   = strlen( strpat );
        pat.curr_pos    = 0;

        sscanf_state = xi_stated_sscanf(
                      &(parser->sscanf_state)
                    , ( const_data_descriptor_t* ) &pat
                    , ( const_data_descriptor_t* ) src
                    , &dst_o );

        YIELD_UNTIL( cs, ( sscanf_state == 0 ), LAYER_STATE_WANT_READ );
    }

    RESTART(cs, ( sscanf_state == 1 ? LAYER_STATE_OK : LAYER_STATE_ERROR ) );

    END_CORO()
}

#define READ_STRING(into) \
{ \
    do { \
        read_string_state = read_string( parser, &into, &src ); \
        YIELD_UNTIL( cs, ( read_string_state == LAYER_STATE_WANT_READ ), LAYER_STATE_WANT_READ ); \
        if( read_string_state == LAYER_STATE_ERROR ) \
        { \
            EXIT( cs, LAYER_STATE_ERROR ); \
        } \
    } while( read_string_state != LAYER_STATE_OK ); \
};

void mqtt_parser_init( mqtt_parser_t* parser )
{
    memset( parser, 0, sizeof( mqtt_parser_t ) );
}

void mqtt_parser_buffer( mqtt_parser_t* parser, uint8_t* buffer, size_t buffer_length )
{
  parser->buffer_pending    = 1;
  parser->buffer            = buffer;
  parser->buffer_length     = buffer_length;
}

/**
 * @TODO add memory management so that the fields are allocated dynamically only when required
 */
layer_state_t mqtt_parser_execute( mqtt_parser_t* parser, mqtt_message_t* message, uint8_t* data, size_t len, size_t* nread )
{
    static uint16_t cs                 = 0; // local coroutine state to prereserve that better we should keep it outside that function
    const_data_descriptor_t src        = { ( char* ) data, len, len, 0 };
    layer_state_t read_string_state    = LAYER_STATE_OK;

    BEGIN_CORO(cs)

    YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

    message->common.common_u.common_value = src.data_ptr[ src.curr_pos ];
    src.curr_pos += 1;

    // remaining length
    parser->digit_bytes      = 0;
    parser->multiplier       = 1;
    parser->remaining_length = 0;

    do {
      parser->digit_bytes += 1;

      YIELD_UNTIL( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ )

      parser->remaining_length += (src.data_ptr[ src.curr_pos ] & 0x7f) * parser->multiplier;
      parser->multiplier *= 128;
      src.curr_pos += 1;
    } while ( ( uint8_t ) src.data_ptr[ src.curr_pos ] >= 0x80 && parser->digit_bytes < 4 );

    if ( ( uint8_t ) src.data_ptr[ src.curr_pos ] >= 0x80)
    {
      parser->error = MQTT_ERROR_PARSER_INVALID_REMAINING_LENGTH;

      EXIT( cs, LAYER_STATE_ERROR )
    }

    message->common.remaining_length = parser->remaining_length;

    if ( message->common.common_u.common_bits.type == MQTT_TYPE_CONNECT )
    {
        READ_STRING( message->connect.protocol_name );

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->connect.protocol_version = src.data_ptr[ src.curr_pos ];

        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->connect.flags_u.flags_value = src.data_ptr[ src.curr_pos ];

        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->connect.keep_alive = (src.data_ptr[ src.curr_pos ] << 8);
        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->connect.keep_alive += src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        READ_STRING( message->connect.client_id );

        if ( message->connect.flags_u.flags_bits.will )
        {
            READ_STRING( message->connect.will_topic );
        }

        if ( message->connect.flags_u.flags_bits.will )
        {
            READ_STRING( message->connect.will_message );
        }

        if ( message->connect.flags_u.flags_bits.username_follows )
        {
            READ_STRING( message->connect.username );
        }

        if ( message->connect.flags_u.flags_bits.password_follows )
        {
            READ_STRING( message->connect.password );
        }

        EXIT( cs, LAYER_STATE_OK );
    }
    else if ( message->common.common_u.common_bits.type == MQTT_TYPE_CONNACK )
    {
        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->connack._unused     = src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->connack.return_code = src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        EXIT( cs, LAYER_STATE_OK );
    }
    else if ( message->common.common_u.common_bits.type == MQTT_TYPE_PUBLISH )
    {
        READ_STRING( message->publish.topic_name );

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->puback.message_id = ( src.data_ptr[ src.curr_pos ] << 8 );
        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->puback.message_id += src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        READ_STRING( message->publish.content );

        EXIT( cs, LAYER_STATE_OK );
    }
    else if ( message->common.common_u.common_bits.type == MQTT_TYPE_PUBREC )
    {
        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ )

        message->pubrec.message_id = ( src.data_ptr[ src.curr_pos ] << 8 );
        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ )

        message->pubrec.message_id += src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        EXIT( cs, LAYER_STATE_OK );
    }
    else if ( message->common.common_u.common_bits.type == MQTT_TYPE_PUBREL )
    {
        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->pubrel.message_id = ( src.data_ptr[ src.curr_pos ] << 8 );
        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->pubrel.message_id += src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        EXIT( cs, LAYER_STATE_OK );
    }
    else if ( message->common.common_u.common_bits.type == MQTT_TYPE_PUBCOMP )
    {
        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->pubcomp.message_id = (src.data_ptr[ src.curr_pos ] << 8);
        src.curr_pos += 1;

        YIELD_ON( cs, ( (src.curr_pos - src.real_size) == 0 ), LAYER_STATE_WANT_READ );

        message->pubcomp.message_id = src.data_ptr[ src.curr_pos ];
        src.curr_pos += 1;

        EXIT( cs, LAYER_STATE_OK );
    }
    else
    {
        parser->error = MQTT_ERROR_PARSER_INVALID_MESSAGE_ID;
        EXIT( cs, LAYER_STATE_ERROR );
    }

    END_CORO()
}
