#include <xively.h>
#include <xi_helpers.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_usage()
{
    static const char usage[] = "This is example_mqtt_publish of xi library\n"
    "to get publish value at certain topic: \n"
    "mqtt_connect_and_publish topic msg\n";

    printf( "%s", usage );
}

#define REQUIRED_ARGS 3

int main( int argc, char* argv[] )
{

#ifdef XI_NOB_ENABLED
    XI_UNUSED( argc );
    XI_UNUSED( argv );
#else
    if( argc < REQUIRED_ARGS )
    {
        print_usage();
        exit( 0 );
    }

    // create the xi library context
    xi_context_t* xi_context
        = xi_create_context( XI_MQTT, 0, 0 );

    // check if everything works
    if( xi_context == 0 )
    {
        return -1;
    }

    const xi_response_t* response = xi_nob_mqtt_publish( xi_context, argv[ 1 ], argv[ 2 ] );

    xi_delete_context( xi_context );

    if( response )
    {

    }
    else
    {

    }

#endif

    return 0;
}
