#ifndef __INC_GUARD_UART
#define __INC_GUARD_UART
#include <REG52.H>
#include <intrins.h>
#include "../UART.h"
#include <stdlib.h>
#include <math.h>

// typedef struct DataFrame {
//     byte type;
//     UARTLength length;
//     char* payload;
// } DataFrame;
void (* _UART_InterruptHandler)(byte type, UARTLength length, byte* payload) = 0;

UARTLength _UART_State = 0;
byte _UART_ReceivedType = 0;
UARTLength _UART_ReceivedLength = 0;
char* _UART_ReceivedPayload = 0;
unsigned int _UART_TimerCount = 0;

#define FOSC 11059200L
#define T1MS (65536-FOSC/12/1000)   //1ms timer calculation method in 12T mode

//超时定时器
//1毫秒@11.0592MHz
void Timer1Init(void)
{
    RCAP2L = TL2 = T1MS;            //initial timer2 low byte
    RCAP2H = TH2 = T1MS >> 8;       //initial timer2 high byte
    TR2 = 1;                        //timer2 start running
    ET2 = 1;                        //enable timer2 interrupt
    EA = 1;                         //open global interrupt switch
    _UART_TimerCount = 100;                      //initial counter
}

void UART_TimeoutTimer() interrupt 5
{
    TF2 = 0;
    
    if (_UART_TimerCount > 0) {
        _UART_TimerCount--;

        //1ms * 1000 -> 1s
        if (_UART_TimerCount == 0) {
            //_UART_TimerCount = 100;               //reset counter
            P2 = ~ P2;

            _UART_State = 0;
        }
    }
}

/**
 * 初始化 UART
 * @param handler 中断处理回调
 */
void UART_Init(void (* handler)(byte type, UARTLength length, char* payload))
{
    P2 = 0;
    TMOD = 0x20; //定时器工作在定时器1的方式2
    PCON = 0x00; //不倍频
    SCON = 0x50; //串口工作在方式1，并且启动串行接收
    TH1 = 0xFD; //设置波特率 9600
    TR1 = 1;
    TL1 = 0xFd;
    ES = 1; //开串口中断
    EA = 1; //开总中断
    _UART_InterruptHandler = handler;
    Timer1Init();
}

/**
 * 串口发送1字节数据
 */
void UART_SendByte(byte Byte)
{
    SBUF = Byte;

    //如果发送完毕，硬件会置位TI
    while (!TI) {
        _nop_();
    }

    TI = 0;
}

void UART_SendDataFrame(UARTLength type, UARTLength length, byte* payload) {
    UARTLength i;
    UART_SendByte(type);
    
    if (sizeof(UARTLength) > sizeof(byte)) {
        UART_SendByte((byte) length);

        for (i = sizeof(UARTLength); i > 0; i--) {
            UART_SendByte(length >> (UARTLength) pow(8, i));
        }

        UART_SendByte((byte) length);
    } else {
        UART_SendByte(length);
    }
    
    for (i = 0; i < length; i++) {
        UART_SendByte(payload[i]);
    }
}

void UART_SendPingRequestOld() {
    UART_SendByte(DATAFRAME_TYPE_PING_REQUEST_OLD); 
}

void UART_SendPingRequest() {
    UART_SendDataFrame(DATAFRAME_TYPE_PING_REQUEST, 0, 0); 
}

void UART_SendPingResponse() {
    UART_SendDataFrame(DATAFRAME_TYPE_PING_RESPONSE, 0, 0);
}

void UART_FreePayloadBuffer() {
    if (_UART_ReceivedPayload != 0) {
        free(_UART_ReceivedPayload);
        _UART_ReceivedPayload = 0;
    }
}

/**
 * 设置中断处理回调
 * @param handler 中断处理回调
 */
void UART_SetInterruptHandler(void (* handler)(byte type, UARTLength length, byte* payload)) {
    _UART_InterruptHandler = handler;
}

/**
 * 串口中断子函数
 */
void UART_Interrupt() interrupt 4
{
    EA = 0;
    
    //当硬件接收到一个数据时，RI会置位
    if (RI == 1) {
        
        RI = 0;

#if UART_USE_DATAFRAMED_RECEIVER == 0
        _UART_InterruptHandler(_UART_ReceivedType, 0, 0);
#else
        switch (_UART_State) {
            case 0:
                _UART_State = 1;
                _UART_ReceivedType = SBUF;
                
                _UART_TimerCount = UART_TIMEOUT_PER_BYTE_IN_MILLSECOND;
                break;

            case 1:
                _UART_State = 2;
                _UART_ReceivedLength = SBUF;
                _UART_TimerCount = UART_TIMEOUT_PER_BYTE_IN_MILLSECOND * _UART_ReceivedLength;

                if (_UART_ReceivedLength == 0) {
                    _UART_InterruptHandler(_UART_ReceivedType, 0, 0);
                    _UART_State = 0;
                } else {
                    _UART_State = 3;
                }
                break;
            
            case 3:
                _UART_State = 4;
                _UART_TimerCount = UART_TIMEOUT_PER_BYTE_IN_MILLSECOND;
                
                if (_UART_ReceivedPayload != 0)
                    UART_FreePayloadBuffer();

                _UART_ReceivedPayload = (byte*) malloc(_UART_ReceivedLength);
                _UART_ReceivedPayload[0] = SBUF;
                break;
            
            default:
                if (_UART_State - 3 < _UART_ReceivedLength) {
                    _UART_TimerCount = UART_TIMEOUT_PER_BYTE_IN_MILLSECOND * _UART_ReceivedLength;
                    _UART_State++;
                    _UART_ReceivedPayload[_UART_State - 3] = SBUF;
                }
                break;
        }

        if (_UART_State >= 3 && _UART_State - 3 == _UART_ReceivedLength) {
            _UART_InterruptHandler(_UART_ReceivedType, _UART_State - 3, _UART_ReceivedPayload);
            _UART_State = 0;
        }
    }
#endif
    EA = 1;
}
#endif