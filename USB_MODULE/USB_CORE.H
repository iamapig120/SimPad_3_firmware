#include "CORE_LIB\CH559.H"

#ifndef __USB_CORE_DEFINE__
#define __USB_CORE_DEFINE__

#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE
#define THIS_ENDP2_SIZE 64

#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
/*
// extern UINT8X  Ep0Buffer[]; // _at_ 0x0000;    //端点0 OUT&IN缓冲区，必须是偶地址
// extern UINT8X  Ep1Buffer[]; // _at_ 0x000a;  //端点1 IN缓冲区,必须是偶地址
// extern UINT8X  Ep2Buffer[64>(MAX_PACKET_SIZE+2)?64:(MAX_PACKET_SIZE+2)]; // _at_ 0x0050;  //端点2 IN缓冲区,必须是偶地址
// extern UINT8X  Ep2Buffer[]; // _at_ 0x0050;//端点2 IN&OUT缓冲区,必须是偶地址
// extern UINT8X  Ep3Buffer[]; // _at_ 0x00D4;//端点3 IN&OUT缓冲区,必须是偶地址
*/
extern UINT8 SetupReq,SetupLen,Ready,Count,UsbConfig;

extern PUINT8 xdata  pDescr;                                                          //USB配置标志
extern USB_SETUP_REQ   SetupReqBuf;                                                   //暂存Setup包

extern UINT8X UserEp2Buf[THIS_ENDP2_SIZE];                                            //用户数据定义
//bit dataFlag = 0;

// 清除 UserBuf2
void CleanUserBuf2();

// USB设备模式端点2的批量上传
void Enp2BlukIn( );

// USB设备模式端点1的中断上传
void Enp1IntIn( );

// 端点3
void Enp3IntIn( );

void USBDeviceInit();

void DeviceInterrupt( );
#endif