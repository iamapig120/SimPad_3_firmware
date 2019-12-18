

#ifndef	__RGB_LED_APA102_H__
#define __RGB_LED_APA102_H__

#define LED_COUNT 12 //LED灯数量

// void ResetLED(void);

void ShowLED();
void SetLED(unsigned char R, unsigned char G, unsigned char B, unsigned char offset);
unsigned char* GetLED(unsigned char offset);
void LEDInit();

#endif