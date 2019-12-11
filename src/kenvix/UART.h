#ifndef __INC_GUARD_UART

#define UART_TIMEOUT_PER_BYTE_IN_MILLSECOND 4
#define UART_USE_DATAFRAMED_RECEIVER 0

#define DATAFRAME_TYPE_ERROR         0x00
#define DATAFRAME_TYPE_MESSAGE       0x01
#define DATAFRAME_TYPE_PART_DATA     0x02
#define DATAFRAME_TYPE_ECHO          0x03
#define DATAFRAME_TYPE_PING_REQUEST  0x06
#define DATAFRAME_TYPE_PING_RESPONSE 0x07
#define DATAFRAME_TYPE_RESTART       0x09

#define DATAFRAME_TYPE_TEST_WATCHDOG 0x08

#define DATAFRAME_TYPE_BEEP          0x79
#define DATAFRAME_TYPE_CARD_DETECTED 0x70
#define DATAFRAME_TYPE_CARD_INVAILD  0x71
#define DATAFRAME_TYPE_DOOR_UNLOCK   0x72
#define DATAFRAME_TYPE_DOOR_FORCE_LOCK   0x75
#define DATAFRAME_TYPE_DOOR_FORCE_UNLOCK 0x76
#define DATAFRAME_TYPE_PING_REQUEST_OLD  0xF2

#include "types.h"
#include "impl/UART.c"

#endif

#ifndef UARTLength
#define UARTLength byte
#endif

/**********************************************************************
               Strcture of UART data frame

Without begin header:
|-------|-------------|--------------------------------------|
| type  | data-length |               Payload                |
| 1Byte |    1Byte    |      Depends on UARTLength-4         |
|-------|-------------|--------------------------------------|
0       1             2                                      |

With begin header:
| ----- |-------|-------------|--------------------------------------|
| begin | type  | data-length |               Payload                |
|  0xA3 | 1Byte |    1Byte    |      Depends on UARTLength-4         |
|-------|-------|-------------|--------------------------------------|
0       1       2             3                                      |
**********************************************************************/

/**
 * 准备数据帧
 */
void UART_SendDataFrame(UARTLength type, UARTLength length, byte* payload);
void UART_SendByte(byte Byte);
void UART_SetInterruptHandler(void (* handler)(byte type, UARTLength length, byte* payload));
void UART_Init(void (* handler)(byte type, UARTLength length, byte* payload));
void UART_Interrupt();
void UART_SendPingRequest();
void UART_SendPingRequestOld();
void UART_FreePayloadBuffer();
void UART_SendPingResponse();