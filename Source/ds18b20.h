#ifndef __DS18B20_H_
#define __DS18B20_H_




bool DS18B20_init(void);
void DS18B20_Write_Byte(uint8_t dat);
uint8_t DS18B20_Read_Byte(void);
uint16_t DS18B20_Get_Temper(void);
uint16_t DS18B20_Temper_Convert(uint16_t temper);


#endif

