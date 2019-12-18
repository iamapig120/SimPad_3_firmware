#include "DATAFLASH\DATAFLASH.H"

/*******************************************************************************
* Function Name  : ProgWord( UINT16 Addr, UINT16 Data )
* Description    : 写EEPROM，双字节写
* Input          : UNIT16 Addr,写地址
                   UINT16 Data,数据
* Output         : None
* Return         : SUCESS 
*******************************************************************************/
UINT8	ProgWord( UINT16 Addr, UINT16 Data )
{
	ROM_ADDR = Addr;
	ROM_DATA = Data;
	if ( ROM_STATUS & bROM_ADDR_OK ) {                                           // 操作地址有效
		ROM_CTRL = ROM_CMD_PROG;
		return( ( ROM_STATUS ^ bROM_ADDR_OK ) & 0x7F );                            // 返回状态,0x00=success, 0x01=time out(bROM_CMD_TOUT), 0x02=unknown command(bROM_CMD_ERR)
	}
	else return( 0x40 );
}

/*******************************************************************************
* Function Name  : WriteDataFlash(UINT16 Addr,PUINT8 buf,UINT16 len)
* Description    : DataFlash写
* Input          : UINT16 Addr，PUINT16 buf,UINT16 len
* Output         : None
* Return         : 
*******************************************************************************/
void WriteDataFlash(UINT16 Addr,PUINT8 buf,UINT16 len)
{
    UINT16 j,tmp;                                           

	  Addr += DATA_FLASH_ADDR;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;                                                           //进入安全模式
    GLOBAL_CFG |= bDATA_WE;                                                    //使能DataFlash写
    SAFE_MOD = 0;
    for(j=0;j<len;j=j+2)
    {
        tmp = buf[j+1];
        tmp <<= 8;
        tmp += buf[j];			
        ProgWord(Addr,tmp);
        Addr = Addr + 2;
    }
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;                                                           //进入安全模式
    GLOBAL_CFG &= ~bDATA_WE;                                                   //开启DataFlash写保护
    SAFE_MOD = 0;
}


/*******************************************************************************
* Function Name  : ReadDataFlash(UINT16 Addr,PUINT8 buf,UINT16 len)
* Description    : DataFlash读
* Input          : UINT16 Addr，PUINT16 buf,UINT16 len
* Output         : None
* Return         : 
*******************************************************************************/
void ReadDataFlash(UINT16 Addr,PUINT8 buf,UINT16 len)
{
    UINT16 j;                                           
    Addr += DATA_FLASH_ADDR;

    for(j=0;j<len;j=j+2)
    {
        *(buf+j) = (UINT16)*(PUINT8C)(Addr);
    }
}

UINT8X DataToWrite[8]; //用于复写内存储，大小为 8 字节
UINT8	ProgWord( UINT16, UINT16);