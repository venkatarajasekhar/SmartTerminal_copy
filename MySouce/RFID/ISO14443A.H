#ifndef _ISO14443A_H_BAB
#define _ISO14443A_H_BAB
/////////////////////////////////////////////////////////////////////
//ISO14443A COMMAND
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠
#define PICC_RESET            0xE0               //复位

/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
s8 PcdRequest(u8 req_code,u8 *pTagType);                     
s8 PcdAnticoll(u8 *pSnr);                                   
s8 PcdSelect(u8 *pSnr,u8 *pSize);                        
s8 PcdHalt(void);                                            

s8 ChangeCodeKey(u8 *pUncoded,u8 *pCoded);                
s8 PcdAuthKey(u8 *pCoded);                                  
s8 PcdLoadKeyE2( u8 auth_mode,u8 keynr);
s8 PcdAuthState(u8 auth_mode,u8 block,u8 *pSnr);      
s8 PcdRead(u8 addr,u8 *pReaddata);                       
s8 PcdWrite(u8 addr,u8 *pWritedata);                     
s8 PcdValue(u8 dd_mode,u8 addr,u8 *pValue);           
//s8 PcdRestore(u8 addr);                                    
//s8 PcdTransfer(u8 addr);                                  

//s8 UL_PcdAnticoll(u8 *snr); 
//s8 UL_PcdWrite(u8 addr,u8 *writedata);

//s8 MifareProRst(u8 parameter,u8 *pLen,u8 *pData);
//s8 MifareProCom(u8 CID_NAD,u8 timeout,u8 *pLen,u8 *pCommand);
s8 M500PcdLoadKeyE2(u8 key_type,u8 sector,u8 *uncoded_keys)	   ;
//s8 M500PiccAuthE2( u8 auth_mode,u8 *snr, u8 keynr,u8 block);
#endif
