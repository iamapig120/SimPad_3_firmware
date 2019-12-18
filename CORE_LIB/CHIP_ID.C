
#define NO_XSFR_DEFINE 1

#include "CORE_LIB\CH559.H"
#include "CORE_LIB\CHIP_ID.H"

/*******************************************************************************
* Function Name  : GetChipID(void)
* Description    : 获取ID号和ID号和校验
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT32 GetChipID( void )
{
    UINT8	d0, d1;
    UINT16	xl, xh;
    E_DIS = 1;                                                                  //避免进入中断
    d0 = *(PUINT8C)( CHIP_ID + 0 );
    d1 = *(PUINT8C)( CHIP_ID + 1 );                                    //ID号低字
    xl = ( d1 << 8 ) | d0;
    d0 = *(PUINT8C)( CHIP_ID + 2 );
    d1 = *(PUINT8C)( CHIP_ID + 3 );                                    //ID号高字
    xh = ( d1 << 8 ) | d0;
    E_DIS = 0;
    return( ( (UINT32)xh << 16 ) | xl );
}

UINT32 xdata chipId;