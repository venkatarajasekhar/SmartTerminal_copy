#ifndef _GNSS_Terminal_H_BAB
#define _GNSS_Terminal_H_BAB

#include "GNSS.h"

         ///<终端通用应答消息体结构体
#define dTerminalGeneralACK_Success       0 ///<成功
#define dTerminalGeneralACK_Failure       1 ///<失败
#define dTerminalGeneralACK_Error         2 ///<消息有误
#define dTerminalGeneralACK_NotSupported  3 ///<不支持

typedef struct 
{
	u16 Province;			///<省域
	u16 City;				///<城市
	u8 ManufacturerID[5];   ///<制造商ID
	u8 TerminalModel[8];    ///<终端型号
	u8 TerminalID[7];       ///<终端ID
	u8 CarBodyColor;        ///<车身颜色
	u8 CarLicenseStr[9];	///<总共7位号码 如：鲁F1G005  则需要8个字节 在加上结束符\0  共9个字节
	u8 AuthenticationCodeStr[10];  //鉴权码，注册成功后得到的注册码字符串（包括\0）

}TerminalRegisterStructTypeDef;  ///<终端注册信息结构体



void GNNS_T_GeneralACK(GeneralACKStructTypeDef *pTerminalGeneralACKStruct);
void GNSS_T_Ask_Heartbeat(void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result));
void GNSS_T_ask_Register(TerminalRegisterStructTypeDef *pTerminalRegisterStruct);
s16 GNSS_T_GetACK_Register(const u8 *pBodyBuf);
void GNSS_T_Ask_Unregister(void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result));
void GNSS_T_Ask_Authenticate(u8 *pStr,u8 len,void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result));
void GNSS_T_Get_SetParam(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Get_QueryParam(u16 AnswerSerialNumber);
void GNSS_T_Get_Control(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Ask_LocateInfo(LocateBasicInfoTypeDef *pBasic,ExtraLocateInfoTypeDef *pExtraLocateInfoList,u8 ListNum,void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result));
void GNSS_T_Get_QueryLocateInfo(LocateBasicInfoTypeDef *pBasic,	ExtraLocateInfoTypeDef *pExtraLocateInfoList,u8 ListNum);
void GNNS_T_Get_TempLocateTrack(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Get_TextInfo(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Get_Question(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Ask_AnswerQuestion(u16 AnswerSerialNumber,u8 AnswerID);
void GNSS_T_Get_SetMenu(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Ask_BookInfo(u8 type,u8 state,void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result));
void GNSS_T_Get_InfoService(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Get_PhoneCallBack(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
void GNSS_T_Ask_MultimediaUpload(MultimediaTypedef *pMultimedia);
void GNSS_T_GetAck_MultimediaUpload(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);
u8 GNSS_T_Get_MultimediaUploadCMD(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf,u32 *pOutMultimediaID);

void GNSS_T_Voice(const u8 *pBuf,u16 len);

#endif
