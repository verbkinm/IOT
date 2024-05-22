#ifndef IOTV_TYPES_H
#define IOTV_TYPES_H

#define HEADER_SIZE 24
#define IDENTIFICATION_SIZE  16
#define STATE_SIZE  15
#define READ_WRITE_SIZE 15
#define TECH_SIZE 15
#define HOST_BROADCAST_SIZE 16
#define LOG_DATA_SIZE 35

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
    HEADER_ASSIGNMENT_PING_PONG,
    HEADER_ASSIGNMENT_TECH,
    HEADER_ASSIGNMENT_HOST_BROADCAST,
    HEADER_ASSIGNMENT_LOG_DATA
} Header_ASSIGNMENT;

typedef enum
{
    HEADER_FLAGS_NONE = 0,
    HEADER_FLAGS_ERROR = 0xFF
} Header_FLAGS;

typedef enum
{
    Identification_FLAGS_NONE = 0x00,
    Identification_FLAGS_DELETE = 0x01,
    Identification_FLAGS_ERROR = 0xFF
} Identification_FLAGS;

typedef enum
{
    ReadWrite_FLAGS_NONE = 0,
    ReadWrite_FLAGS_IGNORE_CH = 0x01,
    ReadWrite_FLAGS_OPEN_STREAM = 0x02,
    ReadWrite_FLAGS_CLOSE_STREAM = 0x03,
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

typedef enum
{
    Tech_TYPE_NONE = 0,
    Tech_TYPE_EV_AC
} Tech_TYPE;

typedef enum
{
    Tech_FLAGS_NONE = 0,
    Tech_FLAGS_ERROR = 0xFF
} Tech_FLAGS;

typedef enum
{
    Host_Broadcast_FLAGS_NONE = 0,
    Host_Broadcast_FLAGS_TCP_CONN = 1,
    Host_Broadcast_FLAGS_UDP_CONN = 2,
    Host_Broadcast_FLAGS_ERROR = 0xFF
} Host_Broadcast_FLAGS;

typedef enum
{
    LOG_DATA_RW = 0,
    LOG_DATA_R,
    LOG_DATA_W
} LOG_DATA_FLAGS;

#endif // IOTV_TYPES_H
