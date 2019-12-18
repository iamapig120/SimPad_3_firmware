#define NO_XSFR_DEFINE 1

#include <intrins.h>
#include "RGB_LIB\RGB_LIB.H"
#include "CORE_LIB\CH559.H"

// @24Mhz
#define Delay0_3us \
	{                \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
	}
#define Delay0_5us \
	{                \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
		_nop_();       \
	}

sbit SK = P1 ^ 1; //信号输出引脚

unsigned char xdata WS2812_LED_SET[WS2812B_LED_COUNT * 3]; // _at_ 0x20;
unsigned char xdata *data WS2812B_pointer;
void ResetLED_WS2812B()
{
	// WS2812B_pointer = WS2812_LED_SET;
	SK = 0;
}

void SetLED_WS2812B(unsigned char R, unsigned char G, unsigned char B, unsigned char offset)
{
	WS2812B_pointer = WS2812_LED_SET + offset * 3;
	*(WS2812B_pointer++) = G;
	*(WS2812B_pointer++) = R;
	*(WS2812B_pointer) = B;
}
unsigned char* GetLED_WS2812B(unsigned char offset)
{
	unsigned char* WS2812B_pointer = WS2812_LED_SET + offset * 3;
	return WS2812B_pointer;
}

void ShowLED_WS2812B()
{
	unsigned char count = 0;
	// EA=0; // 禁用中断
  WS2812B_pointer = WS2812_LED_SET;
	do
	{
		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 7));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 6));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 5));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 4));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 3));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 2));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 1));
		Delay0_3us;
		SK = 0;
		Delay0_5us;

		SK = 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		SK = (*WS2812B_pointer & (1 << 0));
		Delay0_3us;
		SK = 0;
		// _nop_();
		WS2812B_pointer++;
	} while (++count != WS2812B_LED_COUNT * 3);

	// WS2812B_pointer = WS2812_LED_SET;
	SK = 0;
	// EA = 1; // 启用中断
}

void LEDInit_WS2812B(){
    // P1 推挽输出模式，禁止输入
    PORT_CFG &= ~bP1_OC;
    // P1_DIR |= (bSCK | bMOSI | bSCS);
    P1_DIR = 0xFF;
    // P1_IE |= bMISO;
    P1_IE = 0x00;
}