#ifndef __DISPLAY_H_
#define __DISPLAY_H_









void Display_Clear(void);
void Display_Exter_Ico(bool sw);
void Display_Inter_Ico(bool sw);
void Display_Manual_Ico(bool sw);
void Display_Auto_Ico(bool sw);
void Display_Filte_Ico(bool sw);
void Display_PM2_5_Ico(bool sw);
void Display_CO2_Ico(bool sw);
void Display_Fan_Ico(bool sw);
void Display_WindLevel(uint8_t level);
void Display_Ex_WindLevel(uint8_t level);
void Display_In_WindLevel(uint8_t level);
void Display_PM2_5_value(uint16_t value);
void Display_CO2_value(uint16_t value);

void Ht1621WrData(uint8_t Data, uint8_t cnt);
void Ht1621WrCmd(uint8_t Cmd);
void Ht1621WrOneData(uint8_t Addr, uint8_t Data);
void Ht1621WrAllData(uint8_t Addr, uint8_t *p, uint8_t cnt);
void Ht1621_Init(void);


#endif

