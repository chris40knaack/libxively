#include <string.h>
#include <stdio.h>

#include "message.h"

void mqtt_message_init(mqtt_message_t* message) {
  memset(message, 0, sizeof (mqtt_message_t));
}

void mqtt_message_dump(mqtt_message_t* message) {
  printf("message\n");
  printf("  type:              %d\n", message->common.common_u.common_bits.type);
  printf("  qos:               %d\n", message->common.common_u.common_bits.qos);
  printf("  dup:               %s\n", message->common.common_u.common_bits.dup    ? "true" : "false");
  printf("  retain:            %s\n", message->common.common_u.common_bits.retain ? "true" : "false");

  if ( message->common.common_u.common_bits.type == MQTT_TYPE_CONNECT )
  {
    printf("  protocol name:     ");
    mqtt_buffer_dump(&(message->connect.protocol_name));
    printf("\n");

    printf("  protocol version:  %d\n", message->connect.protocol_version);

    printf("  has username:      %s\n", message->connect.flags_u.flags_bits.username_follows ? "true": "false");
    printf("  has password:      %s\n", message->connect.flags_u.flags_bits.password_follows ? "true": "false");
    printf("  has will:          %s\n", message->connect.flags_u.flags_bits.will ? "true": "false");
    printf("  will qos:          %d\n", message->connect.flags_u.flags_bits.will_qos);
    printf("  retains will:      %s\n", message->connect.flags_u.flags_bits.will_retain ? "true": "false");
    printf("  clean session:     %s\n", message->connect.flags_u.flags_bits.clean_session ? "true": "false");

    printf("  keep alive:        %d\n", message->connect.keep_alive);

    printf("  client id:         ");
    mqtt_buffer_dump(&(message->connect.client_id));
    printf("\n");

    printf("  will topic:        ");
    mqtt_buffer_dump(&(message->connect.will_topic));
    printf("\n");
    printf("  will message:      ");
    mqtt_buffer_dump(&(message->connect.will_message));
    printf("\n");

    printf("  username:          ");
    mqtt_buffer_dump(&(message->connect.username));
    printf("\n");
    printf("  password:          ");
    mqtt_buffer_dump(&(message->connect.password));
    printf("\n");
  }
}
