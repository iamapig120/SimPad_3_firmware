// #include "CORE_LIB\CH559.H"
#include "CORE_LIB\DEBUG.H"
#include "CORE_LIB\FIRMWARE_INFO.H"
#include "USB_MODULE\USB_CORE.H"
#include "DATAFLASH\DATAFLASH.H"
#include "KEYBOARD\KEYBOARD.H"
#include "CORE_LIB\CHIP_ID.H"

#define mTimer2RunCTL( SS )    {TR2 = SS ? START : STOP;}

/*鼠标数据*/
UINT8 data HIDMouse[4] = {0x0,0x0,0x0,0x0};
/*键盘数据*/
UINT8 data HIDKey[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

UINT8 xdata keyCode1[] = {0x00,0x1D,0x00,0x00}; // 左键，默认Z
UINT8 xdata keyCode2[] = {0x00,0x1B,0x00,0x00}; // 右键，默认X

UINT8 xdata keyCode3[] = {0x00,0x00,0x00,0x00}; // 辅助按钮1
UINT8 xdata keyCode4[] = {0x00,0x00,0x00,0x00}; // 辅助按钮2
UINT8 xdata keyCode5[] = {0x00,0x00,0x00,0x00}; // 辅助按钮3

UINT8 xdata keyCode6[] = {0x00,0x00,0x04,0x00}; // 侧键 按下，默认鼠标中键

UINT8 xdata keyCode7[] = {0x00,0x00,0x00,0x78}; // 侧键 上拉 高，默认上拉120
UINT8 xdata keyCode8[] = {0x00,0x00,0x00,0x3C}; // 侧键 上拉 低，默认上拉60
UINT8 xdata keyCode9[] = {0x00,0x00,0x00,0xC4}; // 侧键 下拉 低，默认下拉60
UINT8 xdata keyCode10[] = {0x00,0x00,0x00,0x88}; // 侧键 下拉 高，默认下拉120

UINT8 data hiSpeedMode = 0x00; // 极速模式标识符

sbit BT1 = P3^3;//默认设置Z
sbit BT2 = P3^2;//默认设置X

sbit BT3 = P3^4;//默认设置Esc
sbit BT4 = P3^5;//默认设置F1
sbit BT5 = P3^6;//默认设置F2

/* BT6 侧键按下
 * BT7 侧上拉到顶
 * BT8 侧上拉
 * BT9 侧下拉
 * BT10 侧下拉到底
 */

// sbit BT6 = P4_IN^4;//默认设置 鼠标中键
// bit vBT6, vBT7, vBT8, vBT9, vBT10; // 虚拟的BT7-10

sbit P37 = P3^7;
sbit P30 = P3^0;
sbit P31 = P3^1;

#define vBT6 (~P4_IN & 0x10)
#define vBT10 (P31 == 0)
#define vBT9 (P31 == 1 && P37 == 0)
#define vBT7 (~P4_IN & 0x20)
#define vBT8 (~(bit)vBT7 && ~P30)

UINT16 data KEY_DELAY;
UINT16 data KEY_DELAY_MIN;

UINT16 data BT1keyState = 0;
UINT16 data BT2keyState = 0;

UINT16 data BT3keyState = 0;
UINT16 data BT4keyState = 0;
UINT16 data BT5keyState = 0;

UINT16 data BT6keyState = 0;

UINT16 data BT7keyState = 0;
UINT16 data BT8keyState = 0;
UINT16 data BT9keyState = 0;
UINT16 data BT10keyState = 0;

void CheckCmdCode(UINT8X* buffer) {
    if(buffer[1]) {
        CleanUserBuf2();
        switch (buffer[1]) {
            //上位机请求flash
        case 0x01: {
            if(buffer[2] == 0x1C) {
                buffer[2] = 0x00;
            }
            ReadDataFlash(buffer[2]*4,UserEp2Buf, 4);
            UserEp2Buf[4] = buffer[2];
            Enp2BlukIn();
            break;
        }
        //单片机自身上传指令
        case 0x02: {
            switch(buffer[2]) {
            case 0x00: {
                // 发送固件版本
                UserEp2Buf[0] = VERSION_YEAR;
                UserEp2Buf[1] = VERSION_YEAR_S;
                UserEp2Buf[2] = VERSION_MONTH;
                UserEp2Buf[3] = VERSION_DAY;
                Enp2BlukIn();
                break;

            }
            case 0x01: {
                // 发送单片机ID
                UserEp2Buf[0] = (chipId >> 24) & 0xFF;
                UserEp2Buf[1] = (chipId >> 16) & 0xFF;
                UserEp2Buf[2] = (chipId >> 8) & 0xFF;
                UserEp2Buf[3] = chipId & 0xFF;
                Enp2BlukIn();
                break;
            }
            }
            break;
        }
        // 直接操控灯光
        case 0x03: {

            break;
        }
        //跳转到IAP刷机模式
        case 0x0B: {
            DataToWrite[0]=hiSpeedMode;
            DataToWrite[1]=0xAA;
            DataToWrite[2]=0x00;
            DataToWrite[3]=0x00;
            WriteDataFlash(0x28,DataToWrite,4);
            //mDelaymS( 1000 );
            EA = 0; //禁用中断
            SoftReset( );
            break;
        }
        }
    }
}

// 重新读取KeyCode
void refreshKeyCode() {}

void SendKeyPack() {

    HIDKey[0] = 0x00;

    //HIDKey[1] = 0x00;

    HIDKey[2] = 0x00;
    HIDKey[3] = 0x00;
    HIDKey[4] = 0x00;
    HIDKey[5] = 0x00;
    HIDKey[6] = 0x00;
    //HIDKey[7] = 0x00;

    if(BT1keyState) {
        HIDKey[0] |= keyCode1[0];
        HIDKey[2] = keyCode1[1];
    }
    if(BT2keyState) {
        HIDKey[0] |= keyCode2[0];
        HIDKey[3] = keyCode2[1];
    }
    if(BT3keyState) {
        HIDKey[0] |= keyCode3[0];
        HIDKey[4] = keyCode3[1];
    }
    if(BT4keyState) {
        HIDKey[0] |= keyCode4[0];
        HIDKey[5] = keyCode4[1];
    }
    if(BT5keyState) {
        HIDKey[0] |= keyCode5[0];
        HIDKey[6] = keyCode5[1];
    }
    if(BT6keyState) {
        HIDKey[0] |= keyCode6[0];
        HIDKey[7] = keyCode6[1];
    } else if(BT7keyState) {
        HIDKey[0] |= keyCode7[0];
        HIDKey[7] = keyCode7[1];
    } else if(BT8keyState) {
        HIDKey[0] |= keyCode8[0];
        HIDKey[7] = keyCode8[1];
    } else if(BT9keyState) {
        HIDKey[0] |= keyCode9[0];
        HIDKey[7] = keyCode9[1];
    } else if(BT10keyState) {
        HIDKey[0] |= keyCode10[0];
        HIDKey[7] = keyCode10[1];
    }
    /*
    if(BT_RollerkeyState) {
    HIDKey[0] |= keyCode_Roller[0];
    HIDKey[6] = keyCode_Roller[1];
    }
    */
    Enp1IntIn();

}

bit	data MouseOn = 0;

void MouseClick() {
    HIDMouse[0] = 0x08;

    //HIDMouse[1] = 0x00;
    //HIDMouse[2] = 0x00;
    HIDMouse[3] = 0x00;

    if(BT1keyState) {
        HIDMouse[0] |= keyCode1[2];
        HIDMouse[3] = keyCode1[3];
    }
    if(BT2keyState) {
        HIDMouse[0] |= keyCode2[2];
        HIDMouse[3] = keyCode2[3];
    }
    if(BT3keyState) {
        HIDMouse[0] |= keyCode3[2];
        HIDMouse[3] = keyCode3[3];
    }
    if(BT4keyState) {
        HIDMouse[0] |= keyCode4[2];
        HIDMouse[3] = keyCode4[3];
    }
    if(BT5keyState) {
        HIDMouse[0] |= keyCode5[2];
        HIDMouse[3] = keyCode5[3];
    }
    if(BT6keyState) {
        HIDMouse[0] |= keyCode6[2];
        HIDMouse[3] = keyCode6[3];
    } else if(BT7keyState) {
        HIDMouse[0] |= keyCode7[2];
        HIDMouse[3] = keyCode7[3];
    } else if(BT8keyState) {
        HIDMouse[0] |= keyCode8[2];
        HIDMouse[3] = keyCode8[3];
    } else if(BT9keyState) {
        HIDMouse[0] |= keyCode9[2];
        HIDMouse[3] = keyCode9[3];
    } else if(BT10keyState) {
        HIDMouse[0] |= keyCode10[2];
        HIDMouse[3] = keyCode10[3];
    }
    /* if(BT_RollerkeyState) {
        HIDMouse[0] |= keyCode_Roller[2];
    }*/
    Enp3IntIn();
}

void CheckMouseOn() {
    if(keyCode1[2]||keyCode2[2]||keyCode3[2]||keyCode4[2]||keyCode5[2]||keyCode6[2]||keyCode7[2]||keyCode8[2]||keyCode9[2]||keyCode10[2]
            ||keyCode1[3]||keyCode2[3]||keyCode3[3]||keyCode4[3]||keyCode5[3]||keyCode6[3]||keyCode7[3]||keyCode8[3]||keyCode9[3]||keyCode10[3]
      ) {
        MouseOn = 1;
    }
    else {
        MouseOn = 0;
    }
}

//松开按键
void UpKey() {
    SendKeyPack();
    if(MouseOn) {
        MouseClick();
    }
}


bit isKeyPadShouldBeSendFlag = 0;

//扫描按键
void KeyScan() {
    if(BT1 == 0) {
        if(BT1keyState == 0) {
            BT1keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT1keyState > KEY_DELAY_MIN) {
            // BT1keyState--;
        } else {
            BT1keyState = KEY_DELAY_MIN;
        }
    }
    if(BT2 == 0) {
        if(BT2keyState == 0) {
            BT2keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT2keyState > KEY_DELAY_MIN) {
            // BT2keyState--;
        } else {
            BT2keyState = KEY_DELAY_MIN;
        }
    }
    if(BT3 == 0) {
        if(BT3keyState == 0) {
            BT3keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT3keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT3keyState = KEY_DELAY_MIN;
        }
    }
    if(BT4 == 0) {
        if(BT4keyState == 0) {
            BT4keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT4keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT4keyState = KEY_DELAY_MIN;
        }
    }
    if(BT5 == 0) {
        if(BT5keyState == 0) {
            BT5keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT5keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT5keyState = KEY_DELAY_MIN;
        }
    }
    if(vBT6) {
        if(BT6keyState == 0) {
            BT6keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT6keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT6keyState = KEY_DELAY_MIN;
        }
    } else if(vBT7) {
        if(BT7keyState == 0) {
            BT7keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT7keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT7keyState = KEY_DELAY_MIN;
        }
    } else if(vBT8) {
        if(BT8keyState == 0) {
            BT8keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT8keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT8keyState = KEY_DELAY_MIN;
        }
    } else if(vBT9) {
        if(BT9keyState == 0) {
            BT9keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT9keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT9keyState = KEY_DELAY_MIN;
        }
    } else if(vBT10) {
        if(BT10keyState == 0) {
            BT10keyState = KEY_DELAY;
            isKeyPadShouldBeSendFlag = 1;
        } else if(BT10keyState > KEY_DELAY_MIN) {
            // BT3keyState--;
        } else {
            BT10keyState = KEY_DELAY_MIN;
        }
    }
    if(isKeyPadShouldBeSendFlag) {
        // mTimer2RunCTL(0); // 暂停定时器
        SendKeyPack();
        if(MouseOn) {
            MouseClick();
        }
        isKeyPadShouldBeSendFlag = 0;
        // mTimer2RunCTL(1); // 启用定时器
    }
    return;
}

void keyboardInit() {

    KEY_DELAY = 0x40;
    KEY_DELAY_MIN = 0x10;
    CheckMouseOn();

    // P3 上拉输入模式
    PORT_CFG &= ~bP3_OC;
    P3_DIR = 0x00;
    P3_PU = 0xFF;
    // P3_IE = 0xFF;

    // P4 输入模式
    P4_DIR = 0x00;
    // PORT_CFG &= ~bP4_OC;
    P4_DIR = 0x00;
    P4_PU = 0xFF;
}
