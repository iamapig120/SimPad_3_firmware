#ifndef __KEYBOARD_FUNCTIONS__
#define __KEYBOARD_FUNCTIONS__

extern UINT16 data BT1keyState;
extern UINT16 data BT2keyState;

extern UINT16 data BT3keyState;
extern UINT16 data BT4keyState;
extern UINT16 data BT5keyState;

extern UINT16 data BT6keyState;
extern UINT16 data BT7keyState;
extern UINT16 data BT8keyState;
extern UINT16 data BT9keyState;
extern UINT16 data BT10keyState;

void refreshKeyCode();
void CheckCmdCode(UINT8X* buffer);

void KeyScan();
void UpKey();

void keyboardInit();

/*鼠标数据*/
extern UINT8 data HIDMouse[4];// = {0x0,0x0,0x0,0x0};
/*键盘数据*/
extern UINT8 data HIDKey[8];// = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};


#endif