
/********************************** (C) COPYRIGHT *******************************
* File Name          : SIMPAD_CORE.C
* Author             : Handle
* Version            :
* Date               : 2019/10/03
* Description        : SimPad 3 源代码
*******************************************************************************/
#define NO_XSFR_DEFINE 1

#include "CORE_LIB\DEBUG.H"
// #include "RGB_LIB\RGB_LIB.H"
#include "RGB_LIB\RGB_LIB_APA102C.H"
#include "KEYBOARD\KEYBOARD.H"
#include "USB_MODULE\USB_CORE.H"
#include <string.h>

#pragma  NOAREGS

// Timer2 Start

//CH559 Timer2时钟选择
//bTMR_CLK同时影响Timer0&1&2,使用时要注意
#define mTimer2ClkFsys( )      {T2MOD |= (bTMR_CLK | bT2_CLK);C_T2=0;}         //定时器,时钟=Fsys
#define mTimer2Clk4DivFsys( )  {T2MOD &= ~bTMR_CLK;T2MOD |=  bT2_CLK;C_T2 = 0;}//定时器,时钟=Fsys/4
#define mTimer2Clk12DivFsys( ) {T2MOD &= ~(bTMR_CLK | bT2_CLK);C_T2 = 0;}      //定时器,时钟=Fsys/12

//CH559 Timer2 开始(SS=1)/结束(SS=0)
#define mTimer2RunCTL( SS )    {TR2 = SS ? START : STOP;}

// UINT8 FLAG;
// UINT16 Cap[8] = {0};

#define LIGHT_DALAY_MAX 0xFF

UINT8 lightMode = 1;
UINT8 lightDelay = LIGHT_DALAY_MAX;

/*******************************************************************************
* Function Name  : mTimer2Setup(UINT8 T2Out)
* Description    : CH559定时2初始化
* Input          : UINT8 T2Out,是否允许T2输出时钟
                   0：不允许输出
                   1：允许输出
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer2Setup(UINT8 T2Out)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 0;                                                                //启动自动重载定时器功能
    if(T2Out)
    {
        T2MOD |= T2OE;                                                        //是否允许T2输出时钟,如果允许时钟=1/2定时器2溢出率
    }
    else
    {
        T2MOD &= ~T2OE;
    }
}

/*******************************************************************************
* Function Name  : mTimer2Init(UINT16 Tim)
* Description    : CH559 T2定时器赋初值
* Input          : UINT16 Tim,定时器初值
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer2Init(UINT16 Tim)
{
    UINT16 tmp;
    tmp = 65536 - Tim;
    RCAP2L = TL2 = tmp & 0xff;
    RCAP2H = TH2 = (tmp >> 8) & 0xff;
}

unsigned char* rgbPointer;
unsigned char index;

/*******************************************************************************
* Function Name  : mTimer2Interrupt()
* Description    : CH559定时计数器2定时计数器中断处理函数
*******************************************************************************/
void	mTimer2Interrupt( void ) interrupt INT_NO_TMR2 using 2                   //timer2中断服务程序,使用寄存器组1
{
    mTimer2RunCTL( 0 );                                                        //关定时器
    if(TF2)
    {
        TF2 = 0;                                                               //清空定时器2溢出中断
        // UDTR = !UDTR;                                                          //P0.2电平指示监控

        // Unti-Delay
        // 去抖计算
        if(BT1keyState) {
            BT1keyState--;
            if(BT1keyState == 0) {
                //keyNum = 1;
                UpKey();
            }
        }
        if(BT2keyState) {
            BT2keyState--;
            if(BT2keyState == 0) {
                //keyNum = 2;
                UpKey();
            }
        }
        if(BT3keyState) {
            BT3keyState--;
            if(BT3keyState == 0) {
                //keyNum = 3;
                UpKey();
            }
        }
        if(BT4keyState) {
            BT4keyState--;
            if(BT4keyState == 0) {
                //keyNum = 3;
                UpKey();
            }
        }
        if(BT5keyState) {
            BT5keyState--;
            if(BT5keyState == 0) {
                //keyNum = 3;
                UpKey();
            }
        }
        if(BT6keyState) {
            BT6keyState--;
            if(BT6keyState == 0) {
                //keyNum = 1;
                UpKey();
            }
        }
        if(BT7keyState) {
            BT7keyState--;
            if(BT7keyState == 0) {
                //keyNum = 2;
                UpKey();
            }
        }
        if(BT8keyState) {
            BT8keyState--;
            if(BT8keyState == 0) {
                //keyNum = 3;
                UpKey();
            }
        }
        if(BT9keyState) {
            BT9keyState--;
            if(BT9keyState == 0) {
                //keyNum = 3;
                UpKey();
            }
        }
        if(BT10keyState) {
            BT10keyState--;
            if(BT10keyState == 0) {
                //keyNum = 3;
                UpKey();
            }
        }
    }

    mTimer2RunCTL( 1 );                                                        //开定时器
}

// Timer2 End

void main()
{
    // UINT8 xdata index;
    // CfgXTlpll( );                                                              //启用外部晶振
    mDelaymS(40);                                                              //上电延时,等待内部晶振稳定,必加
    CfgFsys();
    mDelaymS(40);                                                              //上电延时,等待内部晶振稳定,必加

    /*
    // 配置睡眠控制寄存器来节省电量
    SAFE_MOD = 0x55;                                                           //开启安全模式
    SAFE_MOD = 0xAA;
    if(CHIP_ID == 0x59){
    	// CH559
      SLEEP_CTRL = 0x7F;
    }else if(CHIP_ID == 0x58){
    	// CH558
      SLEEP_CTRL = 0x6E;
    }
    SAFE_MOD = 0xFF;                                                           //关闭安全模式
    */

    LEDInit();
    // LEDInit_WS2812B();

    // Timer 2 Init Start

    mTimer2Clk12DivFsys( );                                                    //时钟选择Fsys定时器方式为十二分之一时钟
    mTimer2Setup(0);                                                           //定时器初始化
    mTimer2Init(0x0190);                                                       //定时器赋初值 （十进制 400，即五分之一毫秒触发一次）
    // ET2 = 1;                                                                //使能定时计数器2中断
    EA = 1;                                                                    //使能全局中断
    mTimer2RunCTL( 1 );                                                        //启动定时器

    // Timer 2 Init End

    // ShowLED();
    // ReSetLED();

    USBDeviceInit();
    keyboardInit();
    mDelaymS(1);

    // mInitSTDIO( );

    while(1)
    {
        if(Ready) {
            // 清空键盘上传Buffer
            // 清空鼠标上传Buffer
            memset(HIDKey,0,sizeof(HIDKey));                                      //清空缓冲区
            memset(HIDMouse,0,sizeof(HIDMouse));
					
            ET2 = 1;                                                                   //使能定时计数器2中断

            while(1) {
                KeyScan();
            }
        }
        // Bob, Do something!

    }
}