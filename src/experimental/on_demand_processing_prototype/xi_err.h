// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_err.h
 * \author  Olgierd Humenczuk
 * \brief   Error handling (POSIX-like)
 *
 *     * Every function should return a value
 *     * There are special values (usually `0` or `-1`) which indicate occurrence of an error
 *     * User can detect and lookup errors using declarations below
 */

#ifndef __XI_ERR_H__
#define __XI_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
      XI_NO_ERR = 0
    , XI_OUT_OF_MEMORY
    , XI_HTTP_STATUS_PARSE_ERROR
    , XI_HTTP_HEADER_PARSE_ERROR
    , XI_HTTP_PARSE_ERROR
    , XI_HTTP_ENCODE_CREATE_DATASTREAM
    , XI_HTTP_ENCODE_UPDATE_DATASTREAM
    , XI_HTTP_ENCODE_GET_DATASTREAM
    , XI_HTTP_ENCODE_DELETE_DATASTREAM
    , XI_HTTP_ENCODE_DELETE_DATAPOINT
    , XI_HTTP_ENCODE_DELETE_RANGE_DATAPOINT
    , XI_HTTP_ENCODE_UPDATE_FEED
    , XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN
    , XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN
    , XI_CSV_ENCODE_DATAPOINT_BUFFER_OVERRUN
    , XI_CSV_ENCODE_DATASTREAM_BUFFER_OVERRUN
    , XI_CSV_DECODE_FEED_PARSER_ERROR
    , XI_CSV_DECODE_DATAPOINT_PARSER_ERROR
    , XI_CSV_TIME_CONVERTION_ERROR
    , XI_SOCKET_INITIALIZATION_ERROR
    , XI_SOCKET_GETHOSTBYNAME_ERROR
    , XI_SOCKET_CONNECTION_ERROR
    , XI_SOCKET_SHUTDOWN_ERROR
    , XI_SOCKET_WRITE_ERROR
    , XI_SOCKET_READ_ERROR
    , XI_SOCKET_CLOSE_ERROR
    , XI_DATAPOINT_VALUE_BUFFER_OVERFLOW
    , XI_ERR_COUNT
} xi_err_t;

#ifndef XI_OPT_NO_ERROR_STRINGS
/**
 * \brief   Error description lookup table
 */
extern const char* xi_err_string[ XI_ERR_COUNT ];

#endif
/**
 * \brief   Error description string getter for a given value of `xi_err_t`
 */
extern const char* xi_get_error_string( xi_err_t e );

/**
 * \brief   Error getter for the user
 * \return  The `xi_err_t` structure which can be converted to a string using `xi_get_error_string()` method.
 *
 * \warning It resets the last error value, so it's always a good idea to make a copy of it!
 */
extern xi_err_t xi_get_last_error( void );

/**
 * \brief   Error setter for the library itself
 * \note    Current implementation used a global state variable (_errno_), which is not thread-safe.
 *          If thread-safety is required, than _errno_ should be made thread-local.
 */
extern void xi_set_err( xi_err_t e );

#ifdef __cplusplus
}
#endif

#endif // __XI_ERR_H__
