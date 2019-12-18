
/********************************** (C) COPYRIGHT *******************************
* File Name          : RGB_DEMO.C
* Author             : Handle
* Version            : 
* Date               : 2019/10/03
* Description        : CH559点亮WS2812B DEMO
*******************************************************************************/

#include "DEBUG.H"                                                          //调试信息打印
#include "RGB_LIB\RGB_LIB.H"

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

UINT8 lightMode = 2;
UINT8 lightDelay = 0x04;

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
bit refreshLed = 0;

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
        UDTR = !UDTR;                                                          //P0.2电平指示监控
			
			// Led Delay Counter
			
			if(lightDelay){
			  lightDelay--;
			}else{
			  lightDelay = 0x04;
			}
			
			// LED Modes

			if(lightMode == 1 || (lightMode == 2 && lightDelay == 0) || lightMode == 3){
				refreshLed = 1;
				for(index = 0; index < 12; index++){
					rgbPointer = GetLED(index);
					if(rgbPointer[0] == 0xFF){
						if(rgbPointer[1]){
							if(rgbPointer[1] == 0xFF){
								rgbPointer[0]--;
							}else{
								rgbPointer[1]++;
							}
						}else if(rgbPointer[2]){
								rgbPointer[2]--;
							} else{
							rgbPointer[1]++;
						}
					}else if(rgbPointer[1] == 0xFF){
						if(rgbPointer[2]){
							if(rgbPointer[2] == 0xFF){
								rgbPointer[1]--;
							}else{
								rgbPointer[2]++;
							}
						}else if(rgbPointer[0]){
								rgbPointer[0]--;
						} else{
								rgbPointer[2]++;
						}
					}else if(rgbPointer[2] == 0xFF){
						if(rgbPointer[0]){
								rgbPointer[0]++;
						} else if(rgbPointer[1]){
								rgbPointer[1]--;
						} else {
								rgbPointer[0]++;
						}
					}
				}
			}
			if(lightMode == 4 || (lightMode == 5 && lightDelay == 0) || lightMode == 6){
				refreshLed = 1;
				for(index = 0 ; index < 12; index++){
					rgbPointer = GetLED(index);
					if(rgbPointer[0] == 0xFF){
						if(rgbPointer[2]){
							if(rgbPointer[2] == 0xFF){
								rgbPointer[0]--;
							}else{
								rgbPointer[2]++;
							}
						} else if(rgbPointer[1]){
								rgbPointer[1]--;
						} else{
							rgbPointer[2]++;
						}
					} else if(rgbPointer[1] == 0xFF){
						if(rgbPointer[0]){
							if(rgbPointer[0] == 0xFF){
								rgbPointer[1]--;
							}else{
								rgbPointer[0]++;
							}
						} else if(rgbPointer[2]){
								rgbPointer[2]--;
						} else{
								rgbPointer[0]++;
						}
					} else if(rgbPointer[2] == 0xFF){
						if(rgbPointer[1]){
								rgbPointer[1]++;
						} else if(rgbPointer[0]){
								rgbPointer[0]--;
						} else {
								rgbPointer[1]++;
						}
					}
				}
			}

			if(refreshLed){
			  refreshLed = 0;
				ShowLED();
				ResetLED();
			}
			// LED Modes End
		}
		mTimer2RunCTL( 1 );                                                        //开定时器
}

// Timer2 End

void main()
{

    mDelaymS(10);                                                              //上电延时,等待内部晶振稳定,必加 
    CfgFsys();
    mDelaymS(10);	//等待晶振稳定
    
	  PORT_CFG &= ~bP1_OC;
    P1_DIR |= (bSCK | bMOSI | bSCS);
    P1_IE |= bMISO;  

	  // Timer 2 Init Start
	
	  mTimer2Clk12DivFsys( );                                                    //时钟选择Fsys定时器方式
	  mTimer2Setup(0);                                                           //定时器功能演示
    mTimer2Init(0x0FA0);                                                       //定时器赋初值 （十进制 4000）
    ET2 = 1;                                                                   //使能定时计数器2中断
    EA = 1;                                                                    //使能全局中断
    mTimer2RunCTL( 1 );                                                        //启动定时器

	  // Timer 2 Init End

    // Init LED Start
		
		if(lightMode == 1 || lightMode == 4 ){

			SetLED(0xFF, 0, 0, 0);
			SetLED(0xFF, 0x7F, 0, 1);
			SetLED(0xFF, 0xFF, 0, 2);
			SetLED(0x7F, 0xFF, 0, 3);
			SetLED(0, 0xFF, 0, 4);
			SetLED(0, 0xFF, 0x7F, 5);
			SetLED(0, 0xFF, 0xFF, 6);
			SetLED(0, 0x7F, 0xFF, 7);
			SetLED(0, 0, 0xFF, 8);
			SetLED(0x7F, 0, 0xFF, 9);
			SetLED(0xFF, 0, 0xFF, 10);
			SetLED(0xFF, 0, 0x7F, 11);

		} else if (lightMode == 2 || lightMode == 5){

			SetLED(0xFF, 0, 0x60, 0);
			SetLED(0xFF, 0, 0x50, 1);
			SetLED(0xFF, 0, 0x40, 2);
			SetLED(0xFF, 0, 0x30, 3);
			SetLED(0xFF, 0, 0x20, 4);
			SetLED(0xFF, 0, 0, 5);
			SetLED(0xFF, 0x10, 0, 6);
			SetLED(0xFF, 0x20, 0, 7);
			SetLED(0xFF, 0x30, 0, 8);
			SetLED(0xFF, 0x40, 0, 9);
			SetLED(0xFF, 0x50, 0, 10);
			SetLED(0xFF, 0x60, 0, 11);

		} else if (lightMode == 3 || lightMode == 6){

			SetLED(0xFF, 0, 0, 0);
			SetLED(0xFF, 0x7F, 0, 1);
			SetLED(0xFF, 0xFF, 0, 2);
			SetLED(0x7F, 0xFF, 0, 3);
			SetLED(0, 0xFF, 0, 4);
			SetLED(0, 0xFF, 0x7F, 5);
			SetLED(0, 0xFF, 0x7F, 6);
			SetLED(0, 0xFF, 0, 7);
			SetLED(0x7F, 0xFF, 0, 8);
			SetLED(0xFF, 0xFF, 0, 9);
			SetLED(0xFF, 0x7F, 0, 10);
			SetLED(0xFF, 0, 0, 11);

		}
		
		ShowLED();
		ResetLED();
		mDelaymS(1);
			
    // Init LED End
		
    while(1)
    {
			// Bob, Do something!
    }
}