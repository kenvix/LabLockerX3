
#include "main.h"
#include "mfrc522.h"
#include <intrins.h>
#include <reg52.h>
#include <string.h>
#include "kenvix/UART.h"
#include "kenvix/watchdog.h"
#include "kenvix/delay.h"

typedef unsigned int uint;
unsigned char idata ID[4];
unsigned char idata RevBuffer[30];
unsigned char status;
unsigned char R_data;
unsigned char i = 0;
unsigned char heart = 0;
void lock();
void unlock();
void beep(unsigned int x);
void detectCard();
void sendHeartBeat();
void enableCardReader();
void UART_HandleInterrupt(byte type, UARTLength length, char* payload);

/**
 * 主函数
 */
void main()
{
    UART_Init(UART_HandleInterrupt);
    enableCardReader();
    HEART_TEST = 0;

    WatchDog_Init(WATCHDOG_OVERFLOW_SCALE);
    WatchDog_Enable();

    while (1) {
        LED_OK = 1;
        LED_WAIT = 1;
        LED_FAIL = 1;

        detectCard();
        sendHeartBeat();
        WatchDog_Feed();
    }
}

/**
 * LOCK DOOR
 */
void lock()
{
    LOCK_A = 1;
    LOCK_B = 0;
    delay_10ms(MOTOR_ROLLTATE_TIME_10MS);
    LOCK_A = 1;
    LOCK_B = 1;
}

/**
 * unlock door
 */
void unlock()
{
    LOCK_A = 0;
    LOCK_B = 1;
    delay_10ms(MOTOR_ROLLTATE_TIME_10MS);
    LOCK_A = 1;
    LOCK_B = 1;
}

/**
 * 蜂鸣器发声 
 * x 时间(10ms)
 */
void beep(unsigned int x)
{ 
#if ENABLE_BEEP == 1
    unsigned int Count1 = 0, Count2 = 0, Count3 = 0, flag = 0;
    x = x * 100;

    while (1) {
        Count1++;
        Count3++;
        if (Count1 == 100) {
            Count2++;
            if (Count2 == 4) {
                Count1 = 0;
                Count2 = 0;
                flag = ~flag;
            }
        }
        if (!flag) {
            BEEP = ~BEEP;
        }
        if (Count3 == x) {
            BEEP = 1;
            return;
        }
    }
#endif     
}

void UART_HandleInterrupt(byte type, UARTLength length, byte* payload)
{
    switch (type) {
        case DATAFRAME_TYPE_PING_RESPONSE:
            heart = 0;
            return;
        
        case DATAFRAME_TYPE_PING_REQUEST:
            UART_SendPingResponse();
            return;

        case DATAFRAME_TYPE_ECHO:
            UART_SendDataFrame(DATAFRAME_TYPE_ECHO, length, payload);
            return;

        case DATAFRAME_TYPE_DOOR_UNLOCK:
            unlock();
            delay_10ms(DOOR_RELOCK_DELAY_10MS);
            lock();
            return;

        case DATAFRAME_TYPE_BEEP:
            beep(100);
            return;
        
        default:
            return;
    }
}

void detectCard()
{

#if ENABLE_CARD_READER == 1
    unsigned char tmp;
    switch (status) {
    case 0: // 寻卡
        LED_WAIT = 0;
        tmp = PcdRequest(0x52, &RevBuffer);
        if (tmp == 0) {
            status = 1;
        }
        break;

    case 1: //防冲撞获取卡id
        LED_FAIL = 0;
        tmp = PcdAnticoll(&ID);
        if (tmp == 0) {
            status = 2;
        } else {
            status = 0;
        }
        break;
    case 2: //验证卡
        verify();
        status = 3;
        break;
    case 3: //防止出错
        PcdReset();
        status = 0;
        break;

    default:
        status = 0;
        break;
    }
#endif
}

void sendHeartBeat()
{
    HEART_TEST = ~HEART_TEST;
    if (heart >= 200) {
        beep(100);
    }
    if (heart >= 100) {
        UART_SendPingRequestOld();
        status = 3;
    }
    heart++;
}

void enableCardReader() 
{
#if ENABLE_CARD_READER == 1
    PcdReset(); //重置读卡器
    PcdAntennaOff();
    PcdAntennaOn();
    M500PcdConfigISOType('A');
#endif
}