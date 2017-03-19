/******************************************************************************
* @file    main.c
* @author  xxc
* @version V1.0
* @date    2017-03-14
* @brief   控制面板主程序
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 

#include "main.h"
#include "intrins.h"
#include "uart.h"
#include "eeprom.h"
#include "display.h"
#include "timer.h"


bool Status_Change = TRUE;       	//状态刷新位，TRUE:刷新，FLASE：保持

bool Status_Switch = OFF;       	//开机/关机状态位，ON:开机，OFF：关机
bool Status_RunModel = AUTO;       	//手动/自动状态位，ATUO/MANUAL
bool Status_WindModel = EXTER;      //内循环/外循环状态位，EXTER/INTER
bool Status_FilteSetTime = OFF;     //滤网时间设置状态位，ON：进入，OFF：退出
bool Status_FilteIco = OFF;			//滤网显示/关闭状态位，
uint8_t Status_Filte = NORMAL;      //滤网当前状态位


uint8_t Wind_Level = 3;      		//风量大小变量，范围：1-3
uint8_t Filte_CleanTime = 3;      	//滤网清洁时间变量，范围：1-12
uint8_t Filte_ChangeTime = 6;      	//滤网更换时间变量，范围：1-12，固定

uint8_t Filte_RunTime_Hour = 0;		//滤网运行时间，小时
uint8_t Filte_RunTime_Day = 0;		//滤网运行时间，天
uint8_t Filte_RunTime_Mon = 0;		//滤网运行时间，月

uint16_t PM25_Value = 6;
uint16_t CO2_Value = 6;
uint8_t Temp_Ex_Value = 6; 
uint8_t Temp_In_Value = 6; 

extern uint8_t Time_Base_1M, Time_Base_Model_1M;
extern bool Ico_Flash_flg, Send_Status_flg, Update_Sensor_flg, Auto_Timer_flg;
//extern uint8_t Temp_Value;

/**
* @Function ：开机初始化
* @brief    ：读取24c02的数据,初始化显示状态
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Open_Init(void)
{
	Eeprom_Init();

	Filte_RunTime_Hour = Eeprom_Read_Add(HOUR_ADDRESS);
	Filte_RunTime_Day = Eeprom_Read_Add(DAY_ADDRESS);
	Filte_RunTime_Mon = Eeprom_Read_Add(MON_ADDRESS);
	if(Filte_RunTime_Hour > 24)
	{
		Filte_RunTime_Hour = 0;
	}
	if(Filte_RunTime_Day > 30)
	{
		Filte_RunTime_Day = 0;
	}
	if(Filte_RunTime_Mon > 12)
	{
		Filte_RunTime_Mon = 0;
	}

	Send_Status_flg = OFF;
	Update_Sensor_flg = OFF;

	Status_RunModel = AUTO;
	Status_WindModel = EXTER; 
	Wind_Level = 3;

	Display_PM2_5_Ico();
    Display_CO2_Ico();
    Display_Fan_Ico();
	Display_Exter_Ico(TRUE);
	//Display_Inter_Ico(FLASE);
	Display_Auto_Ico(TRUE);
	//Display_Manual_Ico(FLASE);	
	Display_Ex_WindLevel(Wind_Level);

	Con_BackLight = 0;		//开背光
}


/**
* @Function ：开关机按键控制程序
* @brief    ：短按切换开机关机，无长按操作
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_Switch_Key(void)
{
	//uint8_t key_press_num;    

	if(!KEY_SWITCH)                //如果检测到低电平，说明按键按下
    {
		 Delay_ms(10);          //延时去抖，一般10-20ms
	     if(!KEY_SWITCH)           //再次确认按键是否按下，没有按下则退出
		 {	 	
/*	   		while(!KEY_SWITCH)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=2s 
				if(key_press_num == 150)      //大约2s
		  		{
		  			key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_SWITCH)
		       		{
					}
		    	}
			}
*/

            if(Status_Switch == OFF)
			{
				Open_Init();
			}
			else
			{
				Display_Clear();
				Con_BackLight = 1;		//关背光
			}
			Status_Switch = ~Status_Switch;

	   	}
	}
}

/**
* @Function ：运行模式按键控制程序
* @brief    ：短按切换自动/手动模式，退出滤网时间设置模式，长按进入滤网时间设置模式
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_RunModel_Key(void)
{
	uint8_t key_press_num = 0;    

	if(!KEY_RUN_MODEL)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_RUN_MODEL)           //再次确认按键是否按下，没有按下则退出
		{	 	
	   		while(!KEY_RUN_MODEL)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=1.5s 
				if(key_press_num == 150)      //大约2s
		  		{
		  			key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_RUN_MODEL)
		       		{
						Status_FilteSetTime = ON;	//进入滤网清洁时间设置模式
					}
					return;
		    	}
			}

			if(Status_FilteSetTime == ON)
			{
				Status_FilteSetTime = OFF;		//退出滤网清洁时间设置模式，并且返回
				return;
			}


			if(Status_RunModel == AUTO)
			{
				Time_Base_Model_1M = 0;
				Auto_Timer_flg = ON;				
				Display_Auto_Ico(TRUE);
				Display_Manual_Ico(FLASE);				
			}
			else
			{
				Time_Base_Model_1M = 0;
				Auto_Timer_flg = OFF;
				Display_Auto_Ico(FLASE);
				Display_Manual_Ico(TRUE);
			}

			Status_RunModel = ~Status_RunModel;

		}	
	}
}


/**
* @Function ：风机模式按键控制程序
* @brief    ：短按切换内循环/外循环（全新风）模式，长按无操作
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_WindModel_Key(void)
{
	uint8_t key_press_num = 0;    

	if(!KEY_WIND_MODEL)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_WIND_MODEL)           //再次确认按键是否按下，没有按下则退出
		{	 	
/*	   		while(!KEY_WIND_MODEL)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=2s 
				if(key_press_num == 150)      //大约2s
		  		{
		  			key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_WIND_MODEL)
		       		{
					}
		    	}
			}
*/

			if(Status_WindModel == EXTER)
			{
				Display_Exter_Ico(TRUE);
				Display_Inter_Ico(FLASE);				
			}
			else
			{
				Display_Exter_Ico(FLASE);
				Display_Inter_Ico(TRUE);
			}

			Status_WindModel = ~Status_WindModel;

		}
	}
}

/**
* @Function ：滤网按键控制程序
* @brief    ：短按无操作，长按3秒计时清零
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_Filte_Key(void)
{
	uint8_t key_press_num = 0;    

	if(!KEY_FILTE)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_FILTE)           //再次确认按键是否按下，没有按下则退出
		{	 
	   		while(!KEY_FILTE)
	    	{
				key_press_num++;
        		Delay_ms(10);
				if(key_press_num == 250)      //大约3s
		  		{
		  			key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_FILTE)
		       		{
						Filte_RunTime_Hour = 0;
						Filte_RunTime_Day = 0;
						Filte_RunTime_Mon = 0;
						Eeprom_Write_Add(HOUR_ADDRESS, Filte_RunTime_Hour);
						Eeprom_Write_Add(DAY_ADDRESS, Filte_RunTime_Day);
						Eeprom_Write_Add(MON_ADDRESS, Filte_RunTime_Mon);
						Status_Filte = NORMAL;		//切换滤网正常状态
						Display_Filte_Ico(FLASE);
						Status_FilteIco = ON;	
					}
					return;
		    	}
			}


	   	}
	}
}

/**
* @Function ：风量+按键控制程序
* @brief    ：短按调节风量、滤网时间，长按无操作
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_WindUp_Key(void)
{
	//uint8_t key_press_num = 0;    

	if(!KEY_WIND_UP)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_WIND_UP)           //再次确认按键是否按下，没有按下则退出
		{	 	
/*	   		while(!KEY_WIND_UP)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=2s 
				if(key_press_num == 150)      //大约2s
		  		{
		  			key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_WIND_UP)
		       		{
					}
					return;
		    	}
			}
*/

			if(Status_FilteSetTime == ON)	//调节滤网清洁时间
			{
				Filte_CleanTime++;
				if(Filte_CleanTime > 12)
				{
					Filte_CleanTime = 1;
				}
			}
			else	//调节风量
			{
				Wind_Level++;
				if(Wind_Level > 3)
				{
					Wind_Level = 1;
				}
			}

			if(Status_WindModel == EXTER)
			{
				Display_Ex_WindLevel(Wind_Level);
			}
			else
			{
				Display_In_WindLevel(Wind_Level);
			}

	   	}
	}
}


/**
* @Function ：风量-按键控制程序
* @brief    ：短按调节风量、滤网时间，长按无操作
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_WindDown_Key(void)
{
	//uint8_t key_press_num = 0;    

	if(!KEY_WIND_DOWN)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_WIND_DOWN)           //再次确认按键是否按下，没有按下则退出
		{	 	
/*	   		while(!KEY_WIND_DOWN)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=2s 
				if(key_press_num == 150)      //大约2s
		  		{
		  			key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_WIND_DOWN)
		       		{
					}
					return;
		    	}
			}
*/

			if(Status_FilteSetTime == ON)	//调节滤网清洁时间
			{
				if(Filte_CleanTime > 12)
				{
					Filte_CleanTime = 3;
				}
				Filte_CleanTime--;
				if(Filte_CleanTime == 0)
				{
					Filte_CleanTime = 12;
				}
			}
			else	//调节风量
			{
				if(Wind_Level > 3)
				{
					Wind_Level = 3;
				}
				Wind_Level--;
				if(Wind_Level == 0)
				{
					Wind_Level = 3;
				}
			}

			if(Status_WindModel == EXTER)
			{
				Display_Ex_WindLevel(Wind_Level);
			}
			else
			{
				Display_In_WindLevel(Wind_Level);
			}
	   	}
	}
}


/**
* @Function ：更新滤网计时状态
* @brief    ：1小时检查一次
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Filte_Update(void)
{
	if(Status_Filte == CLEAN)
	{
		if(Ico_Flash_flg == OFF)
		{
			Ico_Flash_flg = ON;

			if(Status_FilteIco == OFF)
			{
				Display_Filte_Ico(TRUE);
				Status_FilteIco = ON;
			}
			else
			{
				Display_Filte_Ico(FLASE);
				Status_FilteIco = OFF;			
			}
		}
	}

	if(Time_Base_1M > 60)
	{
		Time_Base_1M = 0;
		Filte_RunTime_Hour++;
		if(Filte_RunTime_Hour > 24)
		{
			Filte_RunTime_Hour = 0;
			Filte_RunTime_Day++;
			if(Filte_RunTime_Day >30)
			{
				Filte_RunTime_Day = 0;
				Filte_RunTime_Mon++;
				if(Filte_RunTime_Mon > 12)
				{
					Filte_RunTime_Mon = 0;
				}

				if(Filte_RunTime_Mon > Filte_CleanTime)
				{
					Status_Filte = CLEAN;
					Display_Filte_Ico(TRUE);
					Status_FilteIco = ON;
				}

				if(Filte_RunTime_Mon > Filte_ChangeTime)
				{
					Status_Filte = CHANGE;
					Display_Filte_Ico(TRUE);
					Status_FilteIco = ON;					
				}

			}
		}
		Eeprom_Write_Add(HOUR_ADDRESS, Filte_RunTime_Hour);
		Eeprom_Write_Add(DAY_ADDRESS, Filte_RunTime_Day);
		Eeprom_Write_Add(MON_ADDRESS, Filte_RunTime_Mon);
	}
}


/**
* @Function ：发送读取命令到CO2传感器
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Send_Com_CO2(void)
{
	uint8_t i;
	uint8_t buff[9] = {0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};	//发送读取二氧化碳浓度命令

	// #ifdef  LOGGER
	// 	Uart_1_SendString(" \r\n Send_Com_CO2.");
	// #endif	

	for(i = 0; i < 9; i++)
	{
		Uart_1_SendByte(buff[i]);
	}
}

/**
* @Function ：发送当前状态到控制板单片机
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Send_Status_Drive(void)
{
	uint8_t i;
	uint8_t buff[16];

	// #ifdef  LOGGER
	// 	Uart_1_SendString(" \r\n Send_Status_Drive.");
	// #endif	

	buff[0] = 0xAA;
	buff[1] = 0x00;
	buff[2] = Status_Switch;
	buff[3] = Status_RunModel;
	buff[4] = Status_WindModel;
	buff[5] = Status_Filte;
	buff[6] = Wind_Level;
	buff[7] = PM25_Value >> 8;
	buff[8] = PM25_Value;
	buff[9] = CO2_Value >> 8;
	buff[10] = CO2_Value;
	buff[11] = Temp_Ex_Value;
	buff[12] = Temp_In_Value;
	buff[13] = 0x00;
	buff[14] = 0xFF;
	buff[15] = 0x55;

	for(i = 0; i < 16; i++)
	{
		Uart_2_SendByte(buff[i]);
	}
}


/**
* @Function ：main
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void main()
{

	Uart_1_Init();
    Uart_2_Init();
	Uart_vir_Init();
	Timer_Init();

	EA = 1;


	Uart_1_SendString("STC12C5A60S2\r\nUart_1 Test !\r\n");
    Uart_2_SendString("STC12C5A60S2\r\nUart_2 Test !\r\n");

	while(1)
	{
		Uart_vir_Main();
		//Delay_ms(1000);
		// if(Temp_Value != 0)
		// {
		// 	Uart_2_SendByte(Temp_Value);
		// 	Temp_Value = 0;
		// }

		if(Status_WindModel == MANUAL)
		{
			Status_WindModel = AUTO;
			Con_BackLight = ~ Con_BackLight;
		}
		// Uart_1_SendByte(CO2_Value >> 8);
		// Uart_1_SendByte(CO2_Value);
		// Uart_2_SendByte(PM25_Value>>8);
		// Uart_2_SendByte(PM25_Value);
		if(Ico_Flash_flg == OFF)
		{
			Ico_Flash_flg = ON;
			Con_BackLight = ~ Con_BackLight;
		}

		//Uart_2_SendByte(Time_Base_1M);
		
	}

/*    while(1)
	{



		Check_Switch_Key();
		if(Status_Switch == ON)
		{
			Uart_vir_Main();

			Filte_Update();		//滤网状态监控

			Check_RunModel_Key();
			Check_WindModel_Key();
			Check_Filte_Key();
			Check_WindUp_Key();
			Check_WindDown_Key();

			if(Send_Status_flg == OFF)
			{
				Send_Status_flg = ON;
				Send_Status_Drive();
				Send_Com_CO2();
			}

            if(Update_Sensor_flg == OFF)
            {
                Update_Sensor_flg = ON;
				Display_PM2_5_value(PM25_Value);
				Display_CO2_value(CO2_Value);
            }


			if(Status_RunModel == AUTO)
			{
				if(Status_WindModel == INTER)
				{
					if(Time_Base_Model_1M > 30 && Auto_Timer_flg == ON)
					{
						Status_WindModel = EXTER;
						Time_Base_Model_1M = 0;
						//Auto_Timer_flg = ON;
					}
					if(PM25_Value < 50)
					{
						Auto_Timer_flg = ON;	
					}
				}

				if(Status_WindModel == EXTER)
				{
					if(Time_Base_Model_1M > 60)
					{
						if(PM25_Value > 75)			//每小时检测一次
						{
							Status_WindModel = INTER;
							Time_Base_Model_1M = 0;
							Auto_Timer_flg = OFF;
						}
						else
						{
							Time_Base_Model_1M = 0;
							Auto_Timer_flg = ON;
						}
					}
				}
			}
		}
	}*/

}



