

#ifndef	__RGB_LED_WS2812B_H__
#define __RGB_LED_WS2812B_H__

#define WS2812B_LED_COUNT 12 //LED

void ResetLED(void);

void ShowLED_WS2812B();
void SetLED_WS2812B(unsigned char R, unsigned char G, unsigned char B, unsigned char offset);
unsigned char* GetLED_WS2812B(unsigned char offset);
void LEDInit_WS2812B();

#endif