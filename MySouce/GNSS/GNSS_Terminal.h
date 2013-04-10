#ifndef _GNSS_Terminal_H_BAB
#define _GNSS_Terminal_H_BAB

#include "GNSS.h"

         ///<�ն�ͨ��Ӧ����Ϣ��ṹ��
#define dTerminalGeneralACK_Success       0 ///<�ɹ�
#define dTerminalGeneralACK_Failure       1 ///<ʧ��
#define dTerminalGeneralACK_Error         2 ///<��Ϣ����
#define dTerminalGeneralACK_NotSupported  3 ///<��֧��

typedef struct 
{
	u16 Province;			///<ʡ��
	u16 City;				///<����
	u8 ManufacturerID[5];   ///<������ID
	u8 TerminalModel[8];    ///<�ն��ͺ�
	u8 TerminalID[7];       ///<�ն�ID
	u8 CarBodyColor;        ///<������ɫ
	u8 CarLicenseStr[9];	///<�ܹ�7λ���� �磺³F1G005  ����Ҫ8���ֽ� �ڼ��Ͻ�����\0  ��9���ֽ�
	u8 AuthenticationCodeStr[10];  //��Ȩ�룬ע��ɹ���õ���ע�����ַ���������\0��

}TerminalRegisterStructTypeDef;  ///<�ն�ע����Ϣ�ṹ��



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
