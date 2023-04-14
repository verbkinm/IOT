#ifndef IOTV_TYPES_H
#define IOTV_TYPES_H

#define HEADER_SIZE 20
#define IDENTIFICATION_SIZE  16
#define STATE_SIZE  15
#define READ_WRITE_SIZE 15

typedef enum {
    HEADER_TYPE_NONE = 0,
    HEADER_TYPE_REQUEST,
    HEADER_TYPE_RESPONSE
} Header_TYPE;

typedef enum
{
    HEADER_ASSIGNMENT_NONE = 0,
    HEADER_ASSIGNMENT_IDENTIFICATION,
    HEADER_ASSIGNMENT_STATE,
    HEADER_ASSIGNMENT_READ,
    HEADER_ASSIGNMENT_WRITE,
    HEADER_ASSIGNMENT_PING_PONG
} Header_ASSIGNMENT;

typedef enum
{
    HEADER_FLAGS_NONE = 0,
    HEADER_FLAGS_ERROR = 0xFF
} Header_FLAGS;

typedef enum
{
    Identification_FLAGS_NONE = 0,
    Identification_FLAGS_ERROR = 0xFF
} Identification_FLAGS;

typedef enum
{
    ReadWrite_FLAGS_NONE = 0,
    ReadWrite_FLAGS_ERROR = 0xFF
} ReadWrite_FLAGS;

typedef enum
{
    STATE_FLAGS_NONE = 0,
    STATE_FLAGS_ERROR = 0xFF
} State_FLAGS;

typedef enum
{
    State_STATE_OFFLINE = 0,
    State_STATE_ONLINE
} State_STATE;


#endif // IOTV_TYPES_H
