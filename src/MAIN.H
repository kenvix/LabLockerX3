#include <reg52.h>
#include "kenvix/UART.h"

#define MOTOR_ROLLTATE_TIME_10MS 80
#define DOOR_RELOCK_DELAY_10MS 500
#define ENABLE_CARD_READER 1
#define ENABLE_BEEP 1


/////////////////////////////////////////////////////////////////////
//端口定义
/////////////////////////////////////////////////////////////////////
//MFRC522
sbit MF522_RST = P2 ^ 4; //RST 接口                   //RC500片选
sbit MF522_NSS = P2 ^ 0; //SDA 接口
sbit MF522_SCK = P2 ^ 1; //SCK 接口
sbit MF522_SI = P2 ^ 2; //MISI接口
sbit MF522_SO = P2 ^ 3; //MISO接口
//指示灯
sbit LED_OK = P1 ^ 0; //成功指示灯
sbit LED_WAIT = P1 ^ 1; //等待识别
sbit LED_FAIL = P1 ^ 2; //失败
sbit HEART_TEST = P1 ^ 3; //心跳检测

sbit LOCK_A = P1 ^ 4; //锁正极
sbit LOCK_B = P1 ^ 5; //负极
sbit BEEP = P2 ^ 5; //蜂鸣器

//按
/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
void InitializeSystem();

#define OSC_FREQ 11059200L

#define BAUD_115200 256 - (OSC_FREQ / 384L) / 115200L // 255
#define BAUD_57600 256 - (OSC_FREQ / 384L) / 57600L // 254
#define BAUD_38400 256 - (OSC_FREQ / 384L) / 38400L // 253
#define BAUD_28800 256 - (OSC_FREQ / 384L) / 28800L // 252
#define BAUD_19200 256 - (OSC_FREQ / 384L) / 19200L // 250
#define BAUD_14400 256 - (OSC_FREQ / 384L) / 14400L // 248
#define BAUD_9600 256 - (OSC_FREQ / 384L) / 9600L // 244
#define BAUD_2400 0XF4
//#define OSC_FREQ                22118400L
//#define  OSC_FREQ        7372800L
//#define OSC_FREQ          22118400L

//#define  RCAP2_50us      65536L - OSC_FREQ/40417L
//#define  RCAP2_1ms       65536L - OSC_FREQ/2000L
#define RCAP2_10ms 65536L - OSC_FREQ / 1200L
//#define  TIME0_500us     65536L - OSC_FREQ/8000L
//#define  TIME0_10ms      65536L - OSC_FREQ/200
#define TRUE 1
#define FALSE 0
