#ifndef __MAIN_H_
#define __MAIN_H_


#include "STC12C5Axx.h"


typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;


typedef bit bool;


#define FLASE 0
#define TRUE  1

#define FOSC 11059200L      //系统频率

sbit KEY_SWITCH         = P1^7;         //开机关机按键
sbit KEY_RUN_MODEL      = P1^6;         //手动/自动
sbit KEY_WIND_MODEL     = P1^4;         //全新风模式/内循环模式切换
sbit KEY_FILTE          = P3^5;         //滤网清零按键 
sbit KEY_WIND_UP        = P1^5;         //风量上调按键
sbit KEY_WIND_DOWN      = P3^4;         //风量下调按键


sbit Con_BackLight      = P3^3;         //背光
sbit Con_Beep           = P2^7;         //蜂鸣器



#endif

