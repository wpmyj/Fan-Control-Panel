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


//bool Status_Change = TRUE;       	//状态刷新位，TRUE:刷新，FLASE：保持

bool Status_Switch = OFF;       	//开机/关机状态位，ON:开机，OFF：关机
bool Status_RunModel = AUTO;       	//手动/自动状态位，ATUO/MANUAL
uint8_t Status_WindModel = EXTER;   //内循环/外循环/旁通状态位，EXTER/INTER/BYPASS
bool Status_FilteSetCleanTime = OFF;     //滤网清洁时间设置状态位，ON：进入，OFF：退出
bool Status_FilteSetChangeTime = OFF;     //滤网更换时间设置状态位，ON：进入，OFF：退出
bool Status_PM2_5SetUpperLimit = OFF;     //PM2.5上限设置状态位，ON：进入，OFF：退出

bool Status_FilteIco = OFF;			//滤网显示/关闭状态位
bool Status_PM2_5Ico = ON;			//PM2.5显示/关闭状态位，
uint8_t Status_Filte = NORMAL;      //滤网当前状态位


uint8_t Ico_Flash_count = 0;      	//PM2_5数值闪烁次数，每秒加一


uint8_t Wind_Level = 3;      		//风量大小变量，范围：1-3
uint8_t Filte_CleanTime = 3;      	//滤网清洁时间变量，范围：1-12
uint8_t Filte_ChangeTime = 6;      	//滤网更换时间变量，范围：1-12
uint8_t PM2_5_UpperLimit = 75;    //PM2.5上限变量，范围：30-150
// uint8_t PM2_5_UpperLimit_H = 0;     //PM2.5上限变量，范围：1-400
// uint8_t PM2_5_UpperLimit_L = 0;     //PM2.5上限变量，范围：1-400

uint8_t Filte_CleanRunTime_Hour = 0;	//滤网清洁运行时间，小时
uint8_t Filte_CleanRunTime_Day = 0;		//滤网清洁运行时间，天
uint8_t Filte_CleanRunTime_Mon = 0;		//滤网清洁运行时间，月

uint8_t Filte_ChangeRunTime_Hour = 0;	//滤网更换运行时间，小时
uint8_t Filte_ChangeRunTime_Day = 0;	//滤网更换运行时间，天
uint8_t Filte_ChangeRunTime_Mon = 0;	//滤网更换运行时间，月

uint16_t PM25_Value = 6;
uint16_t CO2_Value = 6;
uint8_t Temp_Ex_Value = 6; 
uint8_t Temp_In_Value = 6; 

extern uint8_t Time_Base_1M, Time_Base_Model_1M;
extern bool Ico_Flash_flg;
extern bool PM2_5_Flash_flg;
extern bool Update_Sensor_flg;
extern bool Auto_Timer_flg;
extern bool Send_Status_flg;
extern bool Send_co2_flg;

extern bool respons_flg;
extern bool send_respons;
extern bool Receive_Status_flag;

// extern uint8_t RX_1_buf[RX_1_BUF_SIZE];
// extern uint8_t RX_2_buf[RX_2_BUF_SIZE];
extern uint8_t RX_vir_buf[RX_VIR_BUF_SIZE];

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
	Ht1621_Init();
	Display_Clear();
	

	Send_Status_flg = OFF;
	Update_Sensor_flg = OFF;
	Status_PM2_5Ico = ON;


	Status_RunModel = AUTO;
	Status_WindModel = EXTER; 
	Wind_Level = 3;

	Display_PM2_5_Ico(TRUE);
    Display_CO2_Ico(TRUE);
    //Display_Fan_Ico(TRUE);
	Display_Inter_Ico(FLASE);
	Display_Manual_Ico(FLASE);

	Display_Exter_Ico(TRUE);
	Display_Auto_Ico(TRUE);

	Display_WindLevel(Wind_Level);

	BL_ON;		//开背光

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
	uint8_t key_press_num;    

	if(!KEY_SWITCH)                //如果检测到低电平，说明按键按下
    {
		 Delay_ms(10);          //延时去抖，一般10-20ms
	     if(!KEY_SWITCH)           //再次确认按键是否按下，没有按下则退出
		 {	 	
	   		while(!KEY_SWITCH)
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
			key_press_num = 0;

            if(Status_Switch == OFF)
			{
				Open_Init();
				//Uart_1_SendString("open !\r\n");
			}
			else
			{
				Display_Clear();
				BL_OFF;		//关背光
				//Uart_1_SendString("close !\r\n");		
			}
			Status_Switch = ~Status_Switch;
			SEND_STATUS;
	   	}
	}
}

/**
* @Function ：运行模式按键控制程序
* @brief    ：短按切换自动/手动模式，退出滤网时间设置模式，长按进入滤网清洗时间设置模式
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
		  			//key_press_num = 0;        //如果达到长按键标准,则进入长按键动作

					if(!KEY_WIND_MODEL)
					{
						Status_PM2_5SetUpperLimit = ON;		//进入PM2.5上限值设置
						Status_FilteSetChangeTime = OFF;	//退出滤网更换时间设置模式
						Status_FilteSetCleanTime = OFF;		//退出滤网清洁时间设置模式
						Status_PM2_5Ico = ON;				//PM2_5显示状态
						Display_PM2_5_value(PM2_5_UpperLimit);
						while(!KEY_RUN_MODEL);						
						return;
					}

           			if(!KEY_RUN_MODEL)
		       		{
						Status_FilteSetCleanTime = ~Status_FilteSetCleanTime;  	//进入/退出滤网清洁时间设置模式
						Status_FilteSetChangeTime = OFF;						//退出滤网更换时间设置模式
						//UPDATE_PM2_5_VALUE;						//刷新PM2.5数码管
						Display_PM2_5_value(Filte_CleanTime);
						while(!KEY_RUN_MODEL);
						return;											
					}	
					return;
		    	}
			}

			if(Status_PM2_5SetUpperLimit == ON)
			{
				Status_PM2_5SetUpperLimit = OFF;		//退出PM2.5上限值设置模式，并且返回
				UPDATE_PM2_5_VALUE;						//刷新PM2.5数码管			
				return;
			}

			if(Status_FilteSetCleanTime == ON)
			{
				Status_FilteSetCleanTime = OFF;			//退出滤网清洁时间设置模式，并且返回
				UPDATE_PM2_5_VALUE;						//刷新PM2.5数码管			
				return;
			}

			Status_RunModel = ~Status_RunModel;

			Time_Base_Model_1M = 0;
			if(Status_RunModel == AUTO)
			{
				Auto_Timer_flg = ON;				
				Display_Auto_Ico(TRUE);
				Display_Manual_Ico(FLASE);				
			}
			else
			{
				Auto_Timer_flg = OFF;
				Display_Auto_Ico(FLASE);
				Display_Manual_Ico(TRUE);
			}

			SEND_STATUS;
		}	
	}
}


/**
* @Function ：风机模式按键控制程序
* @brief    ：短按切换内循环/外循环（全新风）/旁通模式，长按清空滤网更换时间
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
	   		while(!KEY_WIND_MODEL)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=2s 
				if(key_press_num == 150)      //大约2s
		  		{
		  			//key_press_num = 0;        //如果达到长按键标准,则进入长按键动作
           			
					if(!KEY_RUN_MODEL)
					{
						Status_PM2_5SetUpperLimit = ON;		//进入PM2.5上限值设置
						Status_FilteSetChangeTime = OFF;	//退出滤网更换时间设置模式
						Status_FilteSetCleanTime = OFF;		//退出滤网清洁时间设置模式
						Status_PM2_5Ico = ON;				//PM2_5显示状态
						Display_PM2_5_value(PM2_5_UpperLimit);
						while(!KEY_WIND_MODEL);						
						return;
					}
					
					
					if(!KEY_WIND_MODEL)
		       		{
						if(Status_Filte == CHANGE)
						{
							Filte_ChangeRunTime_Hour = 0;
							Filte_ChangeRunTime_Day = 0;
							Filte_ChangeRunTime_Mon = 0;
							Eeprom_Write_Add(CHANGE_HOUR_ADDRESS, Filte_ChangeRunTime_Hour);
							Delay_ms(100);						
							Eeprom_Write_Add(CHANGE_DAY_ADDRESS, Filte_ChangeRunTime_Day);
							Delay_ms(100);												
							Eeprom_Write_Add(CHANGE_MON_ADDRESS, Filte_ChangeRunTime_Mon);
							Status_Filte = NORMAL;		//切换滤网正常状态
							Display_Filte_Ico(FLASE);
							//Status_FilteIco = ON;
							SEND_STATUS;
							return;
						}
						else
							return;	
					}						
					return;
		    	}
			}

			if(Status_PM2_5SetUpperLimit == ON)
			{
				Status_PM2_5SetUpperLimit = OFF;			//退出PM2.5上限值设置模式，并且返回
				UPDATE_PM2_5_VALUE;						//刷新PM2.5数码管			
				return;
			}

			// if(Status_FilteSetChangeTime == ON)
			// {
			// 	Status_FilteSetChangeTime = OFF;		//退出滤网更换时间设置模式，并且返回
			// 	UPDATE_PM2_5_VALUE;		//刷新PM2.5数码管
			// 	return;
			// }

			//Status_WindModel = ~Status_WindModel;
			Status_WindModel++;
			if(Status_WindModel > 2)
			{
				Status_WindModel = 0;
			}

			if(Status_WindModel == EXTER)
			{
				Display_Exter_Ico(TRUE);
				Display_Inter_Ico(FLASE);				
			}
			else if(Status_WindModel == INTER)
			{
				Display_Exter_Ico(FLASE);
				Display_Inter_Ico(TRUE);
			}
			else
			{
				Display_Exter_Ico(TRUE);
				Display_Inter_Ico(TRUE);			
			}

			SEND_STATUS;
		}
	}
}

/**
* @Function ：滤网按键控制程序
* @brief    ：短按进入/退出滤网更换时间设置，长按3秒清洁计时清零
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
				if(key_press_num == 150)      //大约2s
		  		{
           			while(!KEY_FILTE)
		       		{
						if(Status_Filte == CLEAN)
						{
							Filte_CleanRunTime_Hour = 0;
							Filte_CleanRunTime_Day = 0;
							Filte_CleanRunTime_Mon = 0;
							Eeprom_Write_Add(CLEAN_HOUR_ADDRESS, Filte_CleanRunTime_Hour);
							Delay_ms(100);						
							Eeprom_Write_Add(CLEAN_DAY_ADDRESS, Filte_CleanRunTime_Day);
							Delay_ms(100);												
							Eeprom_Write_Add(CLEAN_MON_ADDRESS, Filte_CleanRunTime_Mon);
							Status_Filte = NORMAL;		//切换滤网正常状态
							Display_Filte_Ico(FLASE);
							//Status_FilteIco = ON;
							SEND_STATUS;
							return;	
						}
						else
							return;
					}
					return;
		    	}
			}


			Status_FilteSetChangeTime = ~Status_FilteSetChangeTime;		//进入/退出滤网更换时间设置模式
			Status_FilteSetCleanTime = OFF;								//退出滤网清洁时间设置模式
			UPDATE_PM2_5_VALUE;						//刷新PM2.5数码管

	   	}
	}
}

/**
* @Function ：风量+按键控制程序
* @brief    ：短按调节风量、滤网时间，长按无操作,并返回
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_WindUp_Key(void)
{
	uint8_t key_press_num = 0;    

	if(!KEY_WIND_UP)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_WIND_UP)           //再次确认按键是否按下，没有按下则退出
		{	 
			Ico_Flash_count = 0;
				
	   		while(!KEY_WIND_UP)
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
			key_press_num = 0; 

			if(Status_FilteSetCleanTime == ON)	//调节滤网清洁时间
			{
				Filte_CleanTime++;
				if(Filte_CleanTime > 12)
				{
					Filte_CleanTime = 1;
				}
				Eeprom_Write_Add(FILTE_CLEAN_ADDRESS, Filte_CleanTime);
				Delay_ms(100);	
				UPDATE_PM2_5_VALUE;			
			}
			else if(Status_FilteSetChangeTime == ON)	//调节滤网更换时间
			{
				Filte_ChangeTime++;
				if(Filte_ChangeTime > 12)
				{
					Filte_ChangeTime = 1;
				}
				Eeprom_Write_Add(FILTE_CHANGE_ADDRESS, Filte_ChangeTime);
				Delay_ms(100);		
				UPDATE_PM2_5_VALUE;									
			}
			else if(Status_PM2_5SetUpperLimit == ON)	//设置PM2.5上限
			{
				PM2_5_UpperLimit++;
				if(PM2_5_UpperLimit > 150)
				{
					PM2_5_UpperLimit = 30;
				}
				Eeprom_Write_Add(PM2_5_UPPER_H_ADDRESS, PM2_5_UpperLimit);
				Delay_ms(100);		
				UPDATE_PM2_5_VALUE;																	
			}
			else	//调节风量
			{
				Wind_Level++;
				if(Wind_Level > 3)
				{
					Wind_Level = 1;
				}
				Display_WindLevel(Wind_Level);
				SEND_STATUS;
			}

	   	}
	}
}


/**
* @Function ：风量-按键控制程序
* @brief    ：短按调节风量、滤网时间，长按无操作,并返回
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Check_WindDown_Key(void)
{
	uint8_t key_press_num = 0;    

	if(!KEY_WIND_DOWN)                //如果检测到低电平，说明按键按下
    {
		Delay_ms(10);          //延时去抖，一般10-20ms
		if(!KEY_WIND_DOWN)           //再次确认按键是否按下，没有按下则退出
		{	 	
			Ico_Flash_count = 0;
			
	   		while(!KEY_WIND_DOWN)
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
			key_press_num = 0;


			if(Status_FilteSetCleanTime == ON)	//调节滤网清洁时间
			{
				if(Filte_CleanTime > 12)
				{
					Filte_CleanTime = 12;
				}
				Filte_CleanTime--;
				if(Filte_CleanTime <= 0)
				{
					Filte_CleanTime = 12;
				}
				Eeprom_Write_Add(FILTE_CLEAN_ADDRESS, Filte_CleanTime);
				Delay_ms(100);		
				UPDATE_PM2_5_VALUE;													
			}
			else if(Status_FilteSetChangeTime == ON)	//调节滤网更换时间
			{
				if(Filte_ChangeTime > 12)
				{
					Filte_ChangeTime = 12;
				}
				Filte_ChangeTime--;
				if(Filte_ChangeTime <= 0)
				{
					Filte_ChangeTime = 12;
				}
				Eeprom_Write_Add(FILTE_CHANGE_ADDRESS, Filte_ChangeTime);
				Delay_ms(100);		
				UPDATE_PM2_5_VALUE;													
			}
			else if(Status_PM2_5SetUpperLimit == ON)		//设置PM2.5上限
			{
				if(PM2_5_UpperLimit > 150)
				{
					PM2_5_UpperLimit = 150;
				}
				PM2_5_UpperLimit--;
				if(PM2_5_UpperLimit <= 0)
				{
					PM2_5_UpperLimit = 30;
				}
				Eeprom_Write_Add(PM2_5_UPPER_H_ADDRESS, PM2_5_UpperLimit);
				Delay_ms(100);	
				UPDATE_PM2_5_VALUE;																								
			}
			else	//调节风量
			{
				if(Wind_Level > 3)
				{
					Wind_Level = 3;
				}
				Wind_Level--;
				if(Wind_Level <= 0)
				{
					Wind_Level = 3;
				}
				Display_WindLevel(Wind_Level);
				SEND_STATUS;
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
	if(Status_Filte == CHANGE)
	{
		if(Ico_Flash_flg == OFF)
		{
			Ico_Flash_flg = ON;
			if(Status_FilteIco == OFF)
			{
				Display_Filte_Ico(TRUE);
			}
			else
			{
				Display_Filte_Ico(FLASE);	
			}
			Status_FilteIco = ~Status_FilteIco;
		}
	}
	// else if(Status_Filte == CHANGE)
	// {
	// 	if(Status_FilteIco == OFF)
	// 	{
	// 		Display_Filte_Ico(TRUE);
	// 		Status_FilteIco = ON;
	// 	}	
	// }

	if(Time_Base_1M > 60)
	{
		Time_Base_1M = 0;
		Filte_CleanRunTime_Hour++;
		if(Filte_CleanRunTime_Hour > 24)
		{
			Filte_CleanRunTime_Hour = 0;
			Filte_CleanRunTime_Day++;
			if(Filte_CleanRunTime_Day >30)
			{
				Filte_CleanRunTime_Day = 0;
				Filte_CleanRunTime_Mon++;
				if(Filte_CleanRunTime_Mon > 12)
				{
					Filte_CleanRunTime_Mon = 0;
				}
			}
		}

		Filte_ChangeRunTime_Hour++;
		if(Filte_ChangeRunTime_Hour > 24)
		{
			Filte_ChangeRunTime_Hour = 0;
			Filte_ChangeRunTime_Day++;
			if(Filte_ChangeRunTime_Day >30)
			{
				Filte_ChangeRunTime_Day = 0;
				Filte_ChangeRunTime_Mon++;
				if(Filte_ChangeRunTime_Mon > 12)
				{
					Filte_ChangeRunTime_Mon = 0;
				}
			}
		}

		if(Filte_ChangeRunTime_Mon > Filte_ChangeTime)
		{
			Status_Filte = CHANGE;
			Display_Filte_Ico(TRUE);
			Status_FilteIco = ON;					
		}

		if(Filte_CleanRunTime_Mon > Filte_CleanTime && Status_Filte != CHANGE)
		{
			Status_Filte = CLEAN;
			Display_Filte_Ico(TRUE);
			Status_FilteIco = ON;
		}


		SEND_STATUS;
		Eeprom_Write_Add(CLEAN_HOUR_ADDRESS, Filte_CleanRunTime_Hour);
		Delay_ms(100);								
		Eeprom_Write_Add(CLEAN_DAY_ADDRESS, Filte_CleanRunTime_Day);
		Delay_ms(100);								
		Eeprom_Write_Add(CLEAN_MON_ADDRESS, Filte_CleanRunTime_Mon);
		Delay_ms(100);								
		Eeprom_Write_Add(CHANGE_HOUR_ADDRESS, Filte_ChangeRunTime_Hour);
		Delay_ms(100);								
		Eeprom_Write_Add(CHANGE_DAY_ADDRESS, Filte_ChangeRunTime_Day);
		Delay_ms(100);								
		Eeprom_Write_Add(CHANGE_MON_ADDRESS, Filte_ChangeRunTime_Mon);
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

	for(i = 0; i < 9; i++)
	{
		Uart_1_SendByte(buff[i]);
	}
}

/**
* @Function ：发送当前状态到驱动板
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Send_Status_Drive(void)
{
	uint8_t i;
	uint8_t buff[17];

	buff[0] = 0xAA;
	buff[1] = 0x00;
	buff[2] = 0xBA;
	buff[3] = Status_Switch;
	buff[4] = Status_RunModel;
	buff[5] = Status_WindModel;
	buff[6] = Status_Filte;
	buff[7] = Wind_Level;
	buff[8] = PM25_Value >> 8;
	buff[9] = PM25_Value;
	buff[10] = CO2_Value >> 8;
	buff[11] = CO2_Value;
	buff[12] = Temp_Ex_Value;
	buff[13] = Temp_In_Value;
	buff[14] = 0x00;
	buff[15] = 0xFF;
	buff[16] = 0x55;

	for(i = 0; i < 17; i++)
	{
		//Uart_2_SendByte(buff[i]);
		Uart_1_SendByte(buff[i]);		//测试
		
	}

}

/**
* @Function ：发送回应包
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Send_Respons_Drive(void)
{
	uint8_t i;
	uint8_t buff[17];

	buff[0] = 0xAA;
	buff[1] = 0x00;
	buff[2] = 0xAB;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x00;
	buff[7] = 0x00;
	buff[8] = 0x00;
	buff[9] = 0x00;
	buff[10] = 0x00;
	buff[11] = 0x00;
	buff[12] = 0x00;
	buff[13] = 0x00;
	buff[14] = 0x00;
	buff[15] = 0xFF;
	buff[16] = 0x55;

	for(i = 0; i < 17; i++)
	{
		//Uart_2_SendByte(buff[i]);
		Uart_1_SendByte(buff[i]);		//测试
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
	uint16_t i;
	
	Uart_1_Init();
    Uart_2_Init();
	Uart_vir_Init();
	Timer_Init();

	EA = 1;

	Eeprom_Init();

	Filte_CleanRunTime_Hour = Eeprom_Read_Add(CLEAN_HOUR_ADDRESS);
	Filte_CleanRunTime_Day = Eeprom_Read_Add(CLEAN_DAY_ADDRESS);
	Filte_CleanRunTime_Mon = Eeprom_Read_Add(CLEAN_MON_ADDRESS);
	if(Filte_CleanRunTime_Hour > 24)
	{
		Filte_CleanRunTime_Hour = 0;
	}
	if(Filte_CleanRunTime_Day > 30)
	{
		Filte_CleanRunTime_Day = 0;
	}
	if(Filte_CleanRunTime_Mon > 12)
	{
		Filte_CleanRunTime_Mon = 0;
	}

	Filte_ChangeRunTime_Hour = Eeprom_Read_Add(CHANGE_HOUR_ADDRESS);
	Filte_ChangeRunTime_Day = Eeprom_Read_Add(CHANGE_DAY_ADDRESS);
	Filte_ChangeRunTime_Mon = Eeprom_Read_Add(CHANGE_MON_ADDRESS);
	if(Filte_ChangeRunTime_Hour > 24)
	{
		Filte_ChangeRunTime_Hour = 0;
	}
	if(Filte_ChangeRunTime_Day > 30)
	{
		Filte_ChangeRunTime_Day = 0;
	}
	if(Filte_ChangeRunTime_Mon > 12)
	{
		Filte_ChangeRunTime_Mon = 0;
	}

	PM2_5_UpperLimit = Eeprom_Read_Add(PM2_5_UPPER_H_ADDRESS);
	if(PM2_5_UpperLimit > 150)
	{
		PM2_5_UpperLimit = 75;
	}

	Filte_CleanTime = Eeprom_Read_Add(FILTE_CLEAN_ADDRESS);
	if(Filte_CleanTime > 12)
	{
		Filte_CleanTime = 3;
	}

	Filte_ChangeTime = Eeprom_Read_Add(FILTE_CHANGE_ADDRESS);
	if(Filte_ChangeTime > 12)
	{
		Filte_ChangeTime = 6;
	}

	//Status_Filte = CLEAN;

	//Uart_1_SendString("STC12C5A60S2\r\nUart_1 Test !\r\n");
    //Uart_2_SendString("STC12C5A60S2\r\nUart_2 Test !\r\n");
	// Filte_CleanRunTime_Mon = 12;
	// Time_Base_1M = 61;

    while(1)
	{

		Check_Switch_Key();

		if(Status_Switch == ON)
		{
			Uart_vir_Main();

			if(Receive_Status_flag == ON)
			{
				Receive_Status_flag = OFF;
				if(RX_vir_buf[15] == 0xFF && RX_vir_buf[16] == 0x55)
				{

					if(RX_vir_buf[2] == 0xBA)
					{
						// Status_Switch = RX_vir_buf[3];
						// Status_RunModel = RX_vir_buf[4];
						Status_WindModel = RX_vir_buf[5];
						// Status_Filte = RX_vir_buf[6];
						// Wind_Level = RX_vir_buf[7];
						Temp_Ex_Value = RX_vir_buf[12];
						Temp_In_Value = RX_vir_buf[13];

						if(Status_WindModel == EXTER)
						{
							Display_Exter_Ico(TRUE);
							Display_Inter_Ico(FLASE);				
						}
						else if(Status_WindModel == INTER)
						{
							Display_Exter_Ico(FLASE);
							Display_Inter_Ico(TRUE);
						}
						else
						{
							Display_Exter_Ico(TRUE);
							Display_Inter_Ico(TRUE);			
						}

						send_respons = ON;				//发送回应包
					}
					else if(RX_vir_buf[2] == 0xAB)
					{
						respons_flg = ON;	//停止重发
					}
				}
			}

			Filte_Update();		//滤网状态监控

			if(Status_PM2_5SetUpperLimit == ON)
			{
				if(PM2_5_Flash_flg == OFF)
				{
					Ico_Flash_count++;
					PM2_5_Flash_flg = ON;
					if(Status_PM2_5Ico == OFF)
					{
						Display_PM2_5_value(PM2_5_UpperLimit);					
					}
					else
					{
						Display_PM2_5_clean();	
					}
					Status_PM2_5Ico = ~Status_PM2_5Ico;
					if(Ico_Flash_count > 9)		//10秒无操作，恢复
					{
						Ico_Flash_count = 0;
						Status_PM2_5SetUpperLimit = OFF;
						UPDATE_PM2_5_VALUE;	
					}
				}	
			}

			Check_RunModel_Key();
			Check_WindModel_Key();
			Check_Filte_Key();
			Check_WindUp_Key();
			Check_WindDown_Key();


			if(Send_Status_flg == OFF)
			{
				Send_Status_flg = ON;
				if(respons_flg == OFF)
				{				
					//Send_Status_Drive();	//若无返回，每秒重发1次
					//Uart_1_SendString("Send_Status_Drive !\r\n");
				}
				//Send_Com_CO2();			//发送读取CO2值命令
			}

      		if(Send_co2_flg == OFF)
			{     
				Send_co2_flg = ON;
				Send_Com_CO2();			//发送读取CO2值命令
			}

            if(Update_Sensor_flg == OFF)
            {
                Update_Sensor_flg = ON;

				if(Status_PM2_5SetUpperLimit == OFF && Status_FilteSetCleanTime == OFF && Status_FilteSetChangeTime == OFF)
				{
					Display_PM2_5_value(PM25_Value);
				}
				// else if(Status_PM2_5SetUpperLimit == ON)
				// {
				// 	Display_PM2_5_value(PM2_5_UpperLimit);					
				// }
				else if(Status_FilteSetCleanTime == ON)
				{
					Display_PM2_5_value(Filte_CleanTime);					
				}
				else if(Status_FilteSetChangeTime == ON)
				{
					Display_PM2_5_value(Filte_ChangeTime);					
				}

				Display_CO2_value(CO2_Value);
            }

			if(send_respons == ON)
			{
				send_respons = OFF;
				//Send_Respons_Drive();
				//Uart_1_SendString("Send_Respons_Drive !\r\n");				
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
						SEND_STATUS;						
					}
					if(PM25_Value < 50)
					{
						Auto_Timer_flg = ON;	
					}
				}

				if(Status_WindModel == EXTER || Status_WindModel == BYPASS)
				{
					// if(Temp_Ex_Value > Temp_In_Value)
					// {
					// 	if((Temp_Ex_Value - Temp_In_Value) < 15)
					// 	{
					// 		if()
					// 		Status_WindModel = BYPASS;
					// 		SEND_STATUS;												
					// 	}
					// 	else
					// 	{
					// 		Status_WindModel = EXTER;							
					// 	}
					// }
					// else
					// {
					// 	if((Temp_In_Value - Temp_Ex_Value) < 15)
					// 	{
					// 		Status_WindModel = BYPASS;
					// 		SEND_STATUS;																			
					// 	}
					// 	else
					// 	{
					// 		Status_WindModel = EXTER;							
					// 	}
					// }
					if(Time_Base_Model_1M > 60)
					{
						if(PM25_Value > PM2_5_UpperLimit)			//每小时检测一次
						{
							Status_WindModel = INTER;
							Time_Base_Model_1M = 0;
							Auto_Timer_flg = OFF;
							SEND_STATUS;												
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
	}

}



