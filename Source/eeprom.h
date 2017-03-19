#ifndef __EEPROM_H_
#define __EEPROM_H_









void Eeprom_Init(void);
uint8_t Eeprom_Read_Add(uint8_t adress);
void Eeprom_Write_Add(uint8_t address, uint8_t date);


uint8_t read_byte(void);
void write_byte(uint8_t date);
void respons(void);
void stop(void);
void start(void);
void delay(void);


#endif
