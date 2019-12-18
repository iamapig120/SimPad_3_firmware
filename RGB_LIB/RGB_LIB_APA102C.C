#define NO_XSFR_DEFINE 1

#include <intrins.h>
#include "RGB_LIB\RGB_LIB_APA102C.H"
#include "CORE_LIB\CH559.H"

#pragma  NOAREGS
#define SET_SPI0_CK( d )   { SPI0_CK_SE = d; }                                 //d>=2


/*硬件接口定义*/
/******************************************************************************
使用CH559 硬件SPI接口 
         CH559        DIR       
         P1.4        <==>       SCS
         P1.5        <==>       MOSI
         P1.6        <==>       MISO
         P1.7        <==>       SCK
*******************************************************************************/

unsigned char xdata rgbDataSet[LED_COUNT * 3]; // _at_ 0x20;
unsigned char xdata *data pointer;
/* void ResetLED()
{
	// pointer = rgbDataSet;
	// SK = 0;
}*/

void SetLED(unsigned char R, unsigned char G, unsigned char B, unsigned char offset)
{
	pointer = rgbDataSet + offset * 3;
	*(pointer) = B;
	pointer++;
	*(pointer) = G;
	pointer++;
	*(pointer) = R;
}
unsigned char* GetLED(unsigned char offset)
{
	unsigned char* pointer = rgbDataSet + offset * 3;
	return pointer;
}

/*******************************************************************************
* Function Name  : CH559SPI0Write(UINT8 dat)
* Description    : CH559硬件SPI写数据
* Input          : UINT8 dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0Write(UINT8 dat)
{
    SPI0_DATA = dat;                                                           
    while(S0_FREE == 0);													   //等待传输完成		
//如果bS0_DATA_DIR为1，此处可以直接读取一个字节的数据用于快速读写	
}

/*******************************************************************************
* Function Name  : CH559SPI0HostInit()
* Description    : CH559SPI0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0HostInit(void)
{
    PORT_CFG &= ~bP1_OC;
    P1_DIR |= (bSCK | bMOSI | bSCS);
    P1_IE |= bMISO;                                                            //引脚配置
	
    SPI0_SETUP &= ~(bS0_MODE_SLV | bS0_BIT_ORDER);                             //设置为主机模式，字节顺序为大端模式		
    SPI0_CTRL |=  bS0_MOSI_OE  | bS0_SCK_OE ;                                  //MISO输出使能，SCK输出使能
    SPI0_CTRL &= ~(bS0_MST_CLK | bS0_2_WIRE);
    SPI0_CTRL &=  ~(bS0_DATA_DIR);                                             //主机写，默认不启动写传输，如果使能bS0_DATA_DIR，
	                                                                             //那么发送数据后自动产生一个字节的时钟，用于快速数据收发	
    SET_SPI0_CK(4);                                                              //4分频
    SPI0_CTRL &= ~bS0_CLR_ALL;                                                 //清空SPI0的FIFO,默认是1，必须置零才能发送数据
}


void ShowLED()
{
		unsigned char data count = 0;
	// EA=0; // 禁用中断
  pointer = rgbDataSet;
	CH559SPI0Write(0x00);
	CH559SPI0Write(0x00);
	CH559SPI0Write(0x00);
	CH559SPI0Write(0x00);
	do
	{
		CH559SPI0Write(0xFE);
		CH559SPI0Write(*pointer);
		pointer++;
		CH559SPI0Write(*pointer);
		pointer++;
		CH559SPI0Write(*pointer);
		pointer++;
		// pointer++;
	} while (++count != LED_COUNT);
	CH559SPI0Write(0x00);
	CH559SPI0Write(0x00);
	CH559SPI0Write(0x00);
	CH559SPI0Write(0x00);
}


void LEDInit(){
    CH559SPI0HostInit();
}