#include "CORE_LIB\CH559.H"
#include "USB_MODULE\USB_CORE.H"
#include "KEYBOARD\KEYBOARD.H"
#include "DATAFLASH\DATAFLASH.H"
#include <string.h>

void CleanUserBuf2() {
    memset(UserEp2Buf,0, 8);
}

UINT8X  Ep0Buffer[THIS_ENDP0_SIZE+2] _at_ 0x0000;    //端点0 OUT&IN缓冲区，必须是偶地址
UINT8X  Ep1Buffer[MAX_PACKET_SIZE+2] _at_ THIS_ENDP0_SIZE+2;  //端点1 IN缓冲区,必须是偶地址
UINT8X  Ep2Buffer[2*THIS_ENDP2_SIZE+4] _at_ THIS_ENDP0_SIZE+2 + MAX_PACKET_SIZE+2;//端点2 IN&OUT缓冲区,必须是偶地址
UINT8X  Ep3Buffer[2*MAX_PACKET_SIZE+4] _at_ THIS_ENDP0_SIZE+2 + MAX_PACKET_SIZE+2 + 2*THIS_ENDP2_SIZE+4;//端点3 IN&OUT缓冲区,必须是偶地址
UINT8 SetupReq,SetupLen,Ready,Count,UsbConfig;

/*******************************************************************************
* Function Name  : Enp2BlukIn()
* Description    : USB设备模式端点2的批量上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2BlukIn( )
{
    //memcpy( Ep2Buffer+MAX_PACKET_SIZE, UserEp2Buf, sizeof(UserEp2Buf));        //加载上传数据
    //UEP2_T_LEN = sizeof(UserEp2Buf);                                              //上传最大包长度
    memcpy( Ep2Buffer+MAX_PACKET_SIZE, UserEp2Buf, 8);        //加载上传数据
    UEP2_T_LEN = 8;                                              //上传最大包长度
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //有数据时上传数据并应答ACK
    while(UEP2_CTRL&MASK_UEP_T_RES == UEP_T_RES_ACK);                          //等待传输完成
}

/*******************************************************************************
* Function Name  : Enp1IntIn()
* Description    : USB设备模式端点1的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Enp1IntIn( )
{

    memcpy( Ep1Buffer, HIDKey, 0x08);                              //加载上传数据
    UEP1_T_LEN = 0x08;                                             //上传数据长度
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;      //有数据时上传数据并应答ACK

}

/*******************************************************************************
* Function Name  : Enp3IntIn()
* Description    : USB设备模式端点3的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Enp3IntIn( )
{
	
    memcpy( Ep3Buffer, HIDMouse, 0x04);                              //加载上传数据
	  UEP3_T_LEN = 0x04;                                               //上传数据长度
    UEP3_CTRL = UEP3_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;        //有数据时上传数据并应答ACK

}

PUINT8 xdata  pDescr;                                                          //USB配置标志
USB_SETUP_REQ   SetupReqBuf;                                                   //暂存Setup包

UINT8X UserEp2Buf[THIS_ENDP2_SIZE];                                            //用户数据定义
//bit dataFlag = 0;

// 设备描述符
UINT8C DevDesc[] = {0x12,//描述符长度(18字节)
                      0x01,//描述符类型
                      0x10,0x01,//本设备所用USB版本。兼容移动设备
                      //0x20,0x00,//本设备所用USB版本
                      0x00,//类代码
                      0x00,//子类代码
                      0x00,//设备所用协议
                      THIS_ENDP0_SIZE,//端点0最大包长
                      0x88,0x80,//厂商ID
                      /*
                      * 00 01 SimPad v2
                      * 00 02 SimPad v2 - Extra
                      * 00 03 SimPad v2 - Lite
                      * 00 04 SimPad v3
                      * 00 05 SimPad Nano
                      * 00 FF SimPad Boot
                      */


                      0x05,0x00,//产品ID

                      0x00, 0x01,//设备版本号

                      0x01,//描述厂商信息的字符串描述符的索引值
                      0x02,//描述产品信息的字串描述符的索引值
                      0x00,//描述设备序列号信息的字串描述符的索引值
                      0x01//可能的配置数
};

/*字符串描述符*/

// 语言描述符
//extern UINT8C	LangDescr[] = { 0x04, 0x03, 0x04, 0x08 };//后两位为语言描述符，0x0804 简体中文 0x0409 英文
UINT8C	LangDescr[] = { 0x0C, 0x03,
                        0x09, 0x04,
                        0x04, 0x08,
                        0x04, 0x0C,
                        0x04, 0x04,
                        0x11, 0x04
                     };// 英文，简中，繁中HK，繁中TW，日语

// 厂家信息
UINT8C	ManuInfo[] = { 32,//长度
                       0x03,
                       'H', 0, 'a', 0, 'n', 0, 'd', 0, 'l', 0, 'e', 0, ' ', 0,
                       'B', 0, 'Y', 0, 'S', 0, 'B', 0, '.', 0, 'n', 0, 'e', 0, 't', 0
                    };
UINT8C ManuInfo_zh_CN[26]=
{
    0x1A,0x03,0x4B,0x62,0xC4,0x67,0x1B,0x54,0x20,0x00,0x42,0x00,0x59,0x00,0x53,0x00,
    0x42,0x00,0x2E,0x00,0x6E,0x00,0x65,0x00,0x74,0x00
};
// 产品信息
UINT8C	ProdInfo[] = {
    // SimPad 3 Keypad
    0x21,0x03,0x53,0x00,0x69,0x00,0x6D,0x00,0x50,0x00,0x61,0x00,0x64,0x00,0x20,0x00,
    0x33,0x00,0x20,0x00,0x4B,0x00,0x65,0x00,0x79,0x00,
    0x70,0x00,0x61,0x00,0x64,0x00
};
UINT8C ProdInfo_zh_CN[]=
{
    // SimPad 3 迷你键盘
    0x1D,0x03,0x53,0x00,0x69,0x00,0x6D,0x00,0x50,0x00,0x61,0x00,0x64,0x00,0x20,0x00,
    0x33,0x00,0x20,0x00,0xF7,0x8F,0x60,0x4F,0x2E,0x95,
    0xD8,0x76
};
UINT8C ProdInfo_zh_TW_HK[]=
{
    // SimPad 3 迷你鍵盤
    0x1D,0x03,0x53,0x00,0x69,0x00,0x6D,0x00,0x50,0x00,0x61,0x00,0x64,0x00,0x20,0x00,
    0x33,0x00,0x20,0x00,0xF7,0x8F,0x60,0x4F,0x75,0x93,
    0xE4,0x76
};
UINT8C ProdInfo_ja_JP[]=
{
    // SimPad 3 ミニキーボード
    0x23,0x03,0x53,0x00,0x69,0x00,0x6D,0x00,0x50,0x00,0x61,0x00,0x64,0x00,0x20,0x00,
    0x33,0x00,0x20,0x00,0xDF,0x30,0xCB,0x30,0xAD,0x30,
    0xFC,0x30,0xDC,0x30,0xFC,0x30,0xC9,0x30
};

/*HID类报表描述符*/
UINT8C KeyRepDesc[62] =
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0
};
UINT8C MouseRepDesc[] =
{
    0x05,0x01,0x09,0x02,0xA1,0x01,0x09,0x01,
    0xA1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
    0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x03,
    0x81,0x02,0x75,0x05,0x95,0x01,0x81,0x01,
    0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
    0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
    0x81,0x06,0xC0,0xC0
};
/*HID类报表描述符*/
UINT8C  HIDRepDesc[]= {
    0x05,0x01,                                               //报表描述符，每个条目占一行
    0x09,0x00,
    0xa1,0x01,
    0x15,0x00,
    0x25,0xff,
    0x19,0x01,
    0x29,0x08,
    0x95,0x08,
    0x75,0x08,
    0x81,0x02,
    0x09,0x02,
    0x15,0x00,
    0x25,0xff,
    0x75,0x08,
    0x95,0x40,
    0x91,0x06,
    0xc0
};

// UINT8C CfgDesc[] =
UINT8C CfgDesc[] =
{
    //配置描述符
    0x09,//配置描述符长度，固定9
    0x02,//配置描述符类型
    0x5B,0x00,//整个描述符数据的长度．指此配置返回的配置描述符，接口描述符以及端点描述符的全部大小
    0x03,//配置所支持的接口数．指该配置配备的接口数量，也表示该配置下接口描述符数量
    0x01,//当使用SetConfiguration和GetConfiguration请求时所指定的配置索引值
    0x00,//用于描述该配置字符串描述符的索引
    0xA0,//供电模式选择．D7:总线供电，D6:自供电，D5:远程唤醒，D4～D0:保留 0xA0== 0b 1010 0000

    //0x32,//总线供电的USB设备的最大消耗电流．以2mA为单位．例如0x32为50*2=100mA
    0x96,
    //0xFA,

    //描述符类说明 http://blog.chinaunix.net/uid-28458801-id-3977853.html

    //接口描述符,键盘

    0x09,//接口描述符长度
    0x04,//接口描述表类 常量 0x04 == 接口
    0x00,//接口号
    0x00,//可选设置的索引值
    0x01,//此接口用的端点数量
    0x03,//类值
    0x01,//子类码
    0x01,//协议码
    0x00,//描述此接口的字串描述表的索引值

    //HID类描述符

    0x09,//描述符类型
    0x21,//描述符类 常量 0x21==HID
    0x10,0x01,//HID规范版本号 BCD
    0x00,//硬件设备所在国家的国家代码（不说明为0）
    0x01,//类别描述符数目（至少有一个报表描述符）
    0x22,//类别描述符类型
    sizeof(KeyRepDesc),0x00,//报表描述符的总长度
    //byte 附加描述符类型，可选
    //word 附加描述符总长度，可选

    //端点描述符

    0x07,//长度
    0x05,//类型编号
    0x81,//端点地址以及输入输出属性
    0x03,//端点的传输类型属性
    0x08,0x00,//端点收发包的最大包大小
    0x01,     //主机查询端点的时间间隔

		//接口描述符,HID设备
    0x09,//接口描述符长度
    0x04,//接口描述表类 常量 0x04 == 接口
    0x01,//接口号
    0x00,//可选设置的索引值
    0x02,//此接口用的端点数量
    0x03,//类值
    //0x01,//子类码
    //0x02,//协议码
    //0x00,//描述此接口的字串描述表的索引值

    0x00,//子类码
    0x00,//协议码
    0x00,//描述此接口的字串描述表的索引值

    //HID类描述符

    0x09,//描述符类型
    0x21,//描述符类 常量 0x21==HID
    0x10,0x01,//HID规范版本号 BCD
    0x00,//硬件设备所在国家的国家代码（不说明为0）
    0x01,//类别描述符数目（至少有一个报表描述符）
    0x22,//类别描述符类型
    //0x34,0x00,//报表描述符的总长度

    sizeof(HIDRepDesc),0x00,//报表描述符的总长度
    //byte 附加描述符类型，可选
    //word 附加描述符总长度，可选

    //端点描述符
    0x07,//长度
    0x05,//类型编号
    0x82,//端点地址以及输入输出属性
    0x03,//端点的传输类型属性
    // 0x04,0x00,//端点收发包的最大包大小
    //0x05,     //主机查询端点的时间间隔

    THIS_ENDP2_SIZE,0x00,//端点收发包的最大包大小
    0x0a,     //主机查询端点的时间间隔

    //端点描述符
    0x07,//长度
    0x05,//类型编号
    0x02,//端点地址以及输入输出属性
    0x03,//端点的传输类型属性
    THIS_ENDP2_SIZE,0x00,//端点收发包的最大包大小
    0x02,     //主机查询端点的时间间隔
		
    //接口描述符,鼠标

    0x09,//接口描述符长度
    0x04,//接口描述表类 常量 0x04 == 接口
    0x02,//接口号
    0x00,//可选设置的索引值
    0x01,//此接口用的端点数量
    0x03,//类值
    0x01,//子类码
    0x02,//协议码
    0x00,//描述此接口的字串描述表的索引值

    //HID类描述符

    0x09,//描述符类型
    0x21,//描述符类 常量 0x21==HID
    0x10,0x01,//HID规范版本号 BCD
    0x00,//硬件设备所在国家的国家代码（不说明为0）
    0x01,//类别描述符数目（至少有一个报表描述符）
    0x22,//类别描述符类型
    //0x34,0x00,//报表描述符的总长度

    sizeof(MouseRepDesc),0x00,//报表描述符的总长度
    //byte 附加描述符类型，可选
    //word 附加描述符总长度，可选

    //端点描述符
    0x07,//长度
    0x05,//类型编号
    0x83,//端点地址以及输入输出属性
    0x03,//端点的传输类型属性
    0x04,0x00,//端点收发包的最大包大小

    //THIS_ENDP2_SIZE,0x00,//端点收发包的最大包大小
    0x01,     //主机查询端点的时间间隔
};


/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB中断处理函数
*******************************************************************************/
bit getSms = 0;
void DeviceInterrupt( ) interrupt INT_NO_USB using 1                      //USB中断服务程序,使用寄存器组1
{
    UINT8 data len = 0;//,i;
    if(UIF_TRANSFER)                                                            //USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# 中断端点上传
            UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
//            UEP1_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            // FLAG = 1;
            break;
        case UIS_TOKEN_OUT | 2:                                                 //endpoint 2# 端点批量下传
            if ( U_TOG_OK )                                                     // 不同步的数据包将丢弃
            {
                UEP2_CTRL &= ~UEP_R_RES_ACK;
                UEP2_CTRL |= UEP_R_RES_NAK;
                len = USB_RX_LEN;                                               //接收数据长度，数据从Ep2Buffer首地址开始存放

							  // 写DataFlash
                if(Ep2Buffer[0]) {
                    DataToWrite[0] = Ep2Buffer[1];
                    DataToWrite[1] = Ep2Buffer[2];
                    DataToWrite[2] = Ep2Buffer[3];
                    DataToWrite[3] = Ep2Buffer[4];
                    if(Ep2Buffer[5]==(Ep2Buffer[1]^Ep2Buffer[2]^Ep2Buffer[3]^Ep2Buffer[4])) {
                        WriteDataFlash((Ep2Buffer[0]) * 4,DataToWrite,4);
                        refreshKeyCode();
                    }
                } else {
									  getSms = 1;
                }
                

                UEP2_T_LEN = 8;
                //UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;       // 允许上传
                UEP2_CTRL &= ~UEP_R_RES_NAK;
                UEP2_CTRL |= UEP_R_RES_ACK;


            }
            break;
        case UIS_TOKEN_IN | 1:                                                  //endpoint 1# 中断端点上传
            UEP1_T_LEN = 0;                                                     //预使用发送长度一定要清空
//            UEP2_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            // FLAG = 1;                                                           /*传输完成标志*/
            break;
        case UIS_TOKEN_IN | 3:                                                  //endpoint 3# 中断端点上传
            UEP3_T_LEN = 0;                                                     //预使用发送长度一定要清空
            //UEP3_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
            UEP3_CTRL = UEP3_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            // FLAG = 1;                                                           /*传输完成标志*/
            break;
        case UIS_TOKEN_SETUP | 0:                                                //SETUP事务
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = UsbSetupBuf->wLengthL;

                if(UsbSetupBuf->wLengthH || SetupLen > 0x7F )
                {
                    SetupLen = 0x7F;    // 限制总长度
                }
                len = 0;                                                        // 默认为成功并且上传0长度
                SetupReq = UsbSetupBuf->bRequest;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/* HID类命令 */
                {
                    switch( SetupReq )
                    {
                    case 0x01://GetReport
                        break;
                    case 0x02://GetIdle
                        break;
                    case 0x03://GetProtocol
                        break;
                    case 0x09://SetReport
                        Ready = 1;
                        break;
                    case 0x0A://SetIdle
                        break;
                    case 0x0B://SetProtocol
                        break;
                    default:
                        len = 0xFF;  								 					            /*命令不支持*/
                        break;
                    }
                    if ( SetupLen > len )
                    {
                        SetupLen = len;    //限制总长度
                    }
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;//本次传输长度
                    memcpy(Ep0Buffer,pDescr,len);                            //加载上传数据
                    SetupLen -= len;
                    pDescr += len;
                }
                else
                {   //标准请求
                    switch(SetupReq)                                        //请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                             //设备描述符
                            pDescr = DevDesc;                               //把设备描述符送到要发送的缓冲区
                            len = sizeof(DevDesc);
                            break;
                        case 2:                                             //配置描述符
                            pDescr = CfgDesc;                               //把设备描述符送到要发送的缓冲区
                            len = sizeof(CfgDesc);
                            break;

                            //请求字符串描述符等
                        case 3:                                             //字符串描述符
                            switch( UsbSetupBuf->wValueL ) {
                            case 1:
                                if(UsbSetupBuf->wIndexL == 0x04 && UsbSetupBuf->wIndexH == 0x04) {
                                    pDescr = ManuInfo_zh_CN;
                                    len = sizeof( ManuInfo_zh_CN );
                                    break;
                                }
                                if(UsbSetupBuf->wIndexL == 0x04 && UsbSetupBuf->wIndexH == 0x08) {
                                    pDescr = ManuInfo_zh_CN;
                                    len = sizeof( ManuInfo_zh_CN );
                                    break;
                                }
                                if(UsbSetupBuf->wIndexL == 0x04 && UsbSetupBuf->wIndexH == 0x0C) {
                                    pDescr = ManuInfo_zh_CN;
                                    len = sizeof( ManuInfo_zh_CN );
                                    break;
                                }
                                pDescr = ManuInfo;
                                len = sizeof( ManuInfo );
                                break;
                            case 2:
															  if(UsbSetupBuf->wIndexL == 0x11 && UsbSetupBuf->wIndexH == 0x04) {
                                    pDescr = ProdInfo_ja_JP;
                                    len = sizeof( ProdInfo_ja_JP );
                                    break;
                                }
                                if(UsbSetupBuf->wIndexL == 0x04 && UsbSetupBuf->wIndexH == 0x08) {
                                    pDescr = ProdInfo_zh_CN;
                                    len = sizeof( ProdInfo_zh_CN );
                                    break;
                                }
                                if(UsbSetupBuf->wIndexL == 0x04 && UsbSetupBuf->wIndexH == 0x0C) {
                                    pDescr = ProdInfo_zh_TW_HK;
                                    len = sizeof( ProdInfo_zh_TW_HK );
                                    break;
                                }
                                if(UsbSetupBuf->wIndexL == 0x04 && UsbSetupBuf->wIndexH == 0x04) {
                                    pDescr = ProdInfo_zh_TW_HK;
                                    len = sizeof( ProdInfo_zh_TW_HK );
                                    break;
                                }
                                pDescr = ProdInfo;
                                len = sizeof( ProdInfo );
                                break;
                            case 0:
                                pDescr = LangDescr;
                                len = sizeof( LangDescr );
                                break;
                            default:
                                len = 0xFF;                               // 不支持的字符串描述符
                                break;
                            }
                            break;

                        case 0x22:                                          //报表描述符
                            if(UsbSetupBuf->wIndexL == 0)                   //报表0报表描述符
                            {
                                pDescr = KeyRepDesc;                        //数据准备上传
                                len = sizeof(KeyRepDesc);
                            }
                            else if(UsbSetupBuf->wIndexL == 1)              //报表1报表描述符
                            {
                                //pDescr = MouseRepDesc;                      //数据准备上传
                                //len = sizeof(MouseRepDesc);

                                pDescr = HIDRepDesc;                                 //数据准备上传
                                len = sizeof(HIDRepDesc);
                                //Ready = 1;                                  //如果有更多报表，该标准位应该在最后一个报表配置完成后有效
                            }
                            else if(UsbSetupBuf->wIndexL == 2)              //报表2报表描述符
                            {
                                pDescr = MouseRepDesc;                      //数据准备上传
                                len = sizeof(MouseRepDesc);
                                Ready = 1;                                  //如果有更多报表，该标准位应该在最后一个报表配置完成后有效
                            }
                            else
                            {
                                len = 0xff;                                 //本程序只有3个报表，这句话正常不可能执行
                            }
                            break;
                        default:
                            len = 0xff;                                     //不支持的命令或者出错
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    //限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;                  //本次传输长度
                        memcpy(Ep0Buffer,pDescr,len);                        //加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;                     //暂存USB设备地址
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if ( SetupLen >= 1 )
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        UsbConfig = UsbSetupBuf->wValueL;
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                            //Clear Feature
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
                        {
                            switch( UsbSetupBuf->wIndexL )
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x83:
                                UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x01:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            case 0x03:
                                UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF;                                            // 不支持的端点
                                break;
                            }
                        }
                        else if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )// 设备
                        {
                            break;
                        }
                        else
                        {
                            len = 0xFF;                                                // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                                              /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )             /* 设置设备 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( CfgDesc[ 7 ] & 0x20 )
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFF;                                        /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFF;                                            /* 操作失败 */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )        /* 设置端点 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                            {
                                switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
                                    break;
                                case 0x83:
                                    UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点3 IN STALL */
                                    break;
                                case 0x03:
                                    UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点3 OUT Stall */
                                    break;
                                default:
                                    len = 0xFF;                               //操作失败
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF;                                   //操作失败
                            }
                        }
                        else
                        {
                            len = 0xFF;                                      //操作失败
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if ( SetupLen >= 2 )
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xff;                                           //操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xff;                                                   //包长度错误
            }
            if(len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len)                                                //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                               //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;                          //本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                            //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                     //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                              //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            //  UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
            // UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA0,返回应答ACK
            if(SetupReq == 0x09)
            {
                if(Ep0Buffer[0])
                {
                    //printf("Light on Num Lock LED!\n");
                }
                else if(Ep0Buffer[0] == 0)
                {
                    //printf("Light off Num Lock LED!\n");
                }
            }
            UEP0_CTRL ^= bUEP_R_TOG;                                     //同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                 //写0清空中断
    }
    if(UIF_BUS_RST)                                                       //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP3_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                                 //清中断标志
    }
    if (UIF_SUSPEND)                                                     //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                 //挂起
        {
//             while ( XBUS_AUX & bUART0_TX );                              //等待发送完成
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                      //USB或者RXD0有信号时可被唤醒
//             PCON |= PD;                                                  //睡眠
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = 0x00;
        }
    }
    else {                                                               //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF;                                               //清中断标志
//      printf("UnknownInt  N");
    }

    //检查CMD代码
    if(getSms) {
        getSms= 0;
        CheckCmdCode(Ep2Buffer);
    }
}

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
	UINT8 i;
	// Endpoints
    UEP1_DMA = Ep1Buffer;                                                      //端点1数据传输地址
    UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN;                    //端点1发送使能 64字节缓冲区
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 //端点1自动翻转同步标志位，IN事务返回NAK

    UEP2_DMA = Ep2Buffer;                                                      //端点2数据传输地址
    UEP3_DMA = Ep3Buffer;                                                      //端点3数据传输地址
    //UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;                  //端点2发送使能 64字节缓冲区
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN | bUEP3_TX_EN ;                    //端点2发送接收使能，端点3发送使能
    UEP2_3_MOD &= ~bUEP2_BUF_MOD & ~bUEP3_BUF_MOD;                             //端点2/3收发各64字节缓冲区
    //UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                               //端点2自动翻转同步标志位，IN事务返回NAK
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                 //端点2自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
    UEP3_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                 //端点3自动翻转同步标志位，IN事务返回NAK，OUT返回ACK

    UEP0_DMA = Ep0Buffer;                                                      //端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //端点0单64字节收发缓冲区
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 //OUT事务返回ACK，IN事务返回NAK

		// 初始化
    USB_CTRL = 0x00;                                                           //清空USB控制寄存器
    // 仅 CH559 拥有
		// USB_CTRL &= ~bUC_HOST_MODE;                                                //该位为选择设备模式
    USB_CTRL |=  bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                    //USB设备和内部上拉使能,在中断期间中断标志未清除前自动返回NAK
    USB_DEV_AD = 0x00;                                                         //设备地址初始化
    UDEV_CTRL &= ~bUD_RECV_DIS;                                                //使能接收器
    USB_CTRL &= ~bUC_LOW_SPEED;
    UDEV_CTRL &= ~bUD_LOW_SPEED;                                             //选择全速12M模式，默认方式
    UDEV_CTRL |= bUD_DP_PD_DIS | bUD_DM_PD_DIS;                                //禁止DM、DP下拉电阻
    UDEV_CTRL |= bUD_PORT_EN;      
	
	  // 中断
	  USB_INT_EN |= bUIE_SUSPEND;                                               //使能设备挂起中断
    USB_INT_EN |= bUIE_TRANSFER;                                              //使能USB传输完成中断
    USB_INT_EN |= bUIE_BUS_RST;                                               //使能设备模式USB总线复位中断
    USB_INT_FG |= 0xFF;                                                       //清中断标志
    IE_USB = 1;                                                               //使能USB中断
    EA = 1;       

    IP_EX |= bIP_USB;																											//USB中断使用高优先级
    UEP1_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP2_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP3_T_LEN = 0;                                                       //预使用发送长度一定要清空

		// 清空缓冲区
    // memset(Ep1Buffer,0x10,sizeof(Ep1Buffer));                                      //清空缓冲区
		for(i = 0; i < sizeof(Ep1Buffer); i++){
		  Ep1Buffer[i] = i;
		}
    memset(Ep2Buffer,0x20,sizeof(Ep2Buffer));
    memset(Ep3Buffer,0x30,sizeof(Ep3Buffer));

}