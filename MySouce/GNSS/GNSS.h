#ifndef _GNSS_H_BAB
#define _GNSS_H_BAB

#include "GNSS_BaseType.h"
#include "GNSS_IO.h"
/**************************************
*** ��ϢID�б�
*** �μ��ն�Э���B.1
***************************************/
typedef enum
{
	eTerminalCommonACK=0x0001,  ///<�ն�ͨ��Ӧ��
	eServerCommonACK  =0x8001,    ///<ƽ̨ͨ��Ӧ��
	eTerminalHeartbeat=0x0002,///<�ն�����
	eTerminalRegister=0x0100,///<�ն�ע��
	eTerminalRegisterACK=0x8100,///<�ն�ע��Ӧ��
	eTerminalUnregister=0x0101,///<�ն�ע��
	eTerminalAuthenticate=0x0102,///<�ն˼�Ȩ
	eSetTerminalParameters=0x8103,///<�����ն˲���
	eGetTerminalParameters=0x8104,///<��ѯ�ն˲���
	eGetTerminalParametersACK=0x0104,///<��ѯ�ն˲���Ӧ��
	eTerminalControl=0x8105,///<�ն˿���
	eLocationInformationReport=0x0200,///<λ����Ϣ�㱨
	eGetLocationInformation=0x8201,///<��ѯλ����Ϣ
	eGetLocationInformationACK=0x0201,///<��ѯλ����ϢӦ��
	eTemporaryPositionTrackingControl=0x8202,///<��ʱλ�ø��ٿ���
	eTextMessage=0x8300,///<�ı���Ϣ�·�
	eEventSet=0x8301,///<�¼�����
	eEventReport=0x0301,///<�¼�����
	eQuestionDownlink=0x8302,///<�����·�
	eQuestionACK=0x0302,///<����Ӧ��
	eSetInformationMenu=0x8303,///<��Ϣ�㲥�˵�����
	eInformationBookOrCancel=0x0303,///<��Ϣ�㲥/ȡ��
	eStorageMultimediaDataSearch=0x8802,///<�洢��ý�����ݼ���
	eStorageMultimediaDataSearchACK=0x0803,///<�洢��ý�����ݼ���Ӧ��
	eStorageUploadMultimediaDataCMD=0x8803,///<�洢��ý�������ϴ�����
	eStartRecording=0x8804,///<¼����ʼ����
	eDownlinkDataWithTransparent=0x8900,///<��������͸��
	eUplinkDataWithTransparent=0x0900,///<��������͸��
	eInformationServices=0x8304,///<��Ϣ����
	eCallBack=0x8400,///<�绰�ز�
	eSetPhoneBook=0x8401,///<���õ绰��
	eCarControl=0x8500,///<��������
	eCarControlACK=0x0500,///<��������Ӧ��
	eSetCircularArea=0x8600,///<����Բ������
	eDeleteCircularArea=0x601,///<ɾ��Բ������
	eSetRectangleArea=0x8602,///<���þ�������
	eDeleteRectangleArea=0x8603,///<ɾ����������
	eSetPolygonArea=0x8604,///<���ö���ξ���
	eDeletePolygonArea=0x8605,///<ɾ������ξ���
	eSetRoute=0x8606,///<����·��
	eDeleteRoute=0x8607,///<ɾ��·��
	eVehicleTravellingDataRecorderCollectCMD=0x8700,///<��ʻ��¼�����ݲɼ�����
	eGetVehicleTravellingDataRecorderDataCMD=0x0700,///<��ʻ��¼�������ϱ�����
	eVehicleTravellingDataRecorderParameters=0x8701,///<��ʻ��¼�ǲ����´�����
	eE_WaybillReported=0x0701,///<�����˵��ϱ�
	eDriverInfo =0x0702,///<��ʻԱ�����Ϣ�ɼ��ϱ�
	eUploadMultimediaEventInformation=0x0800,///<��ý���¼���Ϣ�ϴ�
	eUploadMultimediaData=0x0801,///<��ý�������ϴ�
	eUploadMultimediaDataACK=0x8800,///<��ý�������ϴ�Ӧ��
	eCameraShotImmediately=0x8801,///<����ͷ������������
	eDataCompressionReport=0x0901,///<����ѹ���ϱ�
	eServerPublicRSAKey=0x8A00,///<ƽ̨RSA��Կ
	eTerminalPublicRSAKey=0x0A00,///<�ն�RSA��Կ
	eServerDownlinkMessageReserve=0x8F00,///<������������Ϣ����
	eTerminalUplinkMessageReserve=0x0900,///<�ն�������Ϣ����
	//����Ϊ�Զ���
	eSendVoiceData=0x0AA0,///<��������
	eReceiveVoiceData=0x0AA1,///<��������
	eSendATcommand=0x0AA2,  ///<ATָ������
	eGPSData=0x0AA3  ///<GPS���ݣ�ֻ���ڲ��ԣ���Э����û��

} MessageIDTypeDef;
typedef struct
{
	u16 MessageLen:10;      ///<bit0~9     ��Ϣ�峤��
	u16 EncryptionMode:3;   ///<bit10~12   ���ܷ�ʽ
	u16 MultiPacket:1;      ///<bit 13     �ְ���־
	u16 Reserve:2;          ///<bit 14~15  ����
} MessagePropertyBitTypeDef;
typedef union
{
	MessagePropertyBitTypeDef Bit;
	u16 All;
} MessagePropertyTypeDef;
typedef struct
{
	u16 TotalMessagePackets;  ///<��Ϣ�ܰ���
	u16 PackageNo;            ///<�����
} MessagePacketPropertiesTypeDef;
typedef struct
{
	MessageIDTypeDef MessageID;                              ///<��ϢID
	MessagePropertyTypeDef MessageProperty;                  ///<��Ϣ������
	u8 PhoneNumber[6];                                       ///<�ն��ֻ���
	u16 MessageSerialNumber;                                 ///<��Ϣ��ˮ��
	MessagePacketPropertiesTypeDef MessagePacketProperties;  ///<��Ϣ����װ��
} MessageHeadTypeDef;
typedef union
{
	MessageHeadTypeDef MessageHead;
	u8  MessageHeadBuf[sizeof(MessageHeadTypeDef)];
} MessageHeadUnionTypeDef;
///��Ϣ�ṹ��
typedef struct
{
	u8 StartFlagByte;    ///<��ʶλ
	MessageHeadUnionTypeDef MessageHeadUnion;        ///<��Ϣͷ
	u8 *pMessageBody;                      ///<��Ϣ��
	u8 Data[1024];						   ///<������
	u8 BCC;                                   ///<���У����
	u8 EndFlagByte;                          ///<��ʶλ
	//MessageIDTypeDef MessageID;
} MessageStructTypeDef;

///ͨ��Ӧ��ṹ��
#define dSuccess		0
#define	dFail		1
#define	dError		2
#define	dNotSupport	3
typedef struct
{
	u16 AnswerSerialNumber;///<Ӧ����ˮ��
	u16 AnswerID;          ///<Ӧ��ID
	u8  Result;            ///<���
}GeneralACKStructTypeDef; 
///////////////////////////////////////////////////////////
#define GNSS_FrameBytes_MAX	1024	//Э������ֽ���
#define GNSS_RX_BUF_MAX		5		//����������
/**
  ���ݽڵ�
*/
struct DataNode
{
	u8 buf[GNSS_FrameBytes_MAX];
	u16 len;
	u8 ready;
	struct DataNode *Next;
};

struct ACKNodeTypedef  
{
	u16 SerialNumber;
	u16 ID;
	void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result);
};
//--------------------------------------------------------------------------------------------------------------/

typedef struct 
{
	u32 EmergencyAlarm:1;				///<bit 0 �������������������ش���
	u32 SpeedingAlarm:1;                ///<bit 1 ���ٱ���
	u32 tiredAlarm:1;					///<bit 2 ƣ�ͼ�ʻ����
	u32 Warning:1;						///<bit 3 Ԥ��
	u32 GNSSWarning:1;					///<bit 4 GNSSģ�鷢������
	u32 GNSSAntennaOpenCircuit:1;		///<bit 5 GNSS����δ�ӻ����
	u32 GNSSAntennaShortCircuit:1;		///<bit 6 GNSS����δ�ӻ����
	u32 MainPowerLow:1;					///<bit 7 �ն�����ԴǷѹ
	u32 MainPowerDown:1;				///<bit 8 �ն�����Դ����
	u32 LCDWarning:1;					///<bit 9 �ն�LCD����ʾ������
	u32 TTSWarning:1;					///<bit 10 TTSģ�����
	u32 CameraWarning:1;				///<bit 11 ����ͷ����
	u32 Reserve:6;						///<bit 12~17 ����
	u32 TheDayTotalDrivingTimeWarning:1;///<bit 18 �����ۼƼ�ʻ��ʱ
	u32 ParkingOvertimeWarning:1;		///<bit 19 ��ʱͣ��
	u32 InOutArea:1;					///<bit 20 ��������
	u32 InOutLine:1;					///<bit 21 ������·
	u32 RoadContinueTimeWarning:1;		///<bit 22 ·����ʻʱ�䲻��/����
	u32 RoadDeviateWarning:1;			///<bit 23 ��·ƫ�뱨��
	u32 VSSWarning:1;					///<bit 24 ����VSS����
	u32 OilWarning:1;					///<bit 25���������쳣
	u32 StolenWarning:1;				///<bit 26��������
	u32 IllegalStart:1;					///<bit 27 �����Ƿ����
	u32 IllegalMove:1;					///<bit 28 �����Ƿ��ƶ�
	u32 Reserve1:3;						///<bit 29~31����

}AlarmBit;  ///<������־λ

typedef union
{
	AlarmBit Bit;
	u32 All;
}AlarmFlageTypeDef; ///<������־

typedef struct 
{
	u32 ACCEnable:1;						///<bit 0  ACC״̬ 0����  1����
	u32 LocateOrNot:1;						///<bit 1  0��δ��λ 1;��λ
	u32 Latitude:1;							///<bit 2  0����γ 1����γ
	u32 Longitude:1;						///<bit 3  0������ 1������
	u32 OperatingStatus:1;					///<bit 4  0��Ӫ��״̬ 1��ͣ��״̬
	u32 LatitudeAndLongitudeEncryption:1;	///<bit 5  ��γ�ȼ���  1:����
	u32 Reserve1:4;							///<bit 6~9����
	u32 OilState:1;							///<bit 10  0:��·����  1����·�Ͽ�
	u32 CircuitState:1;						///<bit 11  0��������·����  1��������·�Ͽ�
	u32 DoorState:1;						///<bit 12  0�����Ž��� 1:���ż���
	u32 Reserve2:19;						///<bit 13~31���� 

}StateTypeDef;  ///<״̬λ����

typedef union
{
	StateTypeDef Bit;
	u32 All;
}CarStateTypeDef;
typedef struct 
{
	AlarmFlageTypeDef AlarmFlage;	///<������־
	CarStateTypeDef   CarState;		///<״̬
	u32               Latitude;		//γ��
	u32	              Longitude;	///<����
	u16               Altitude;		///<�߳�
	u16               Speed;		///<�ٶ� 1/10km/H
	u16               Direction;	///<���� 0~359 ,����Ϊ0 ��˳ʱ��
	u8                TimeBCD[6];	///<YY-MM-DD-hh-mm-ss (GMT+8ʱ��)

}LocateBasicInfoTypeDef; ///<λ�û�����Ϣ

typedef enum 
{
	eMileage=0x01,					///<��̶���
	eOil=0x02,						///<����
	eSpeed=0x03,					///<�ٶ�
	eSpeedAlarmInfo=0x11,			///<���ٱ�����Ϣ
	eInOutInfo=0x12,				///<��������/·�߱���������Ϣ
	eRoadContinueTimeWarning=0x13	///<·����ʻʱ�䲻��/��������������Ϣ

}ExtraInfoIDTypeDef; ///<������ϢID

typedef struct 
{
	u8	ExtraInfoID; ///<������ϢID
	u8 	ExtraInfoLen;///<������Ϣ����
	u8  ExtraInfo[7];///<������Ϣ�ĳ������Ϊ7���ֽ�
}ExtraLocateInfoTypeDef;///<λ�ø�����Ϣ

//-----------------------------------------------------------------------------------------------------/
typedef struct 
{
	u8 MultimediaType;
	u8 ChannelID;
	u8 Event;
	u8 BeginTimeBCD[6];	//YY-MM-DD-HH-MM-SS
	u8 EndTimeBCD[6];
	u8 DeletedMarker; 
}MultimediaUploadCMDTypedef;
//-----------------------------------------------------------------------------------------------------/
//-----------------------------------------------------------------------------------------------------/
typedef struct 
{
	u32 MultimediaID;
	u8 type;
	u8 format;
	u8 event;
	u8 channelID;
	u32 TotalPackets;
	u32 PacketID;
	u32 len;	//����
	u8 *pData;
}MultimediaTypedef;
extern u8  PhoneNumber[6];

u32 TimeConvertTimestamp(u8 *pBCDTime,s8 TimeZone);
void GNSS_Init(void);
void GNSS_Close(void);


s16 GNSS_GetDataStream(u8 Data);
s16 GNSS_TryGetMessage(MessageHeadTypeDef *pOutMessageHead,u8 *pBodyBuf);
s16 GNSS_Pack(const MessageHeadTypeDef *pMessageHead,const u8 *pBodyBuf,const u16 Size);

void GNSS_InsertACKNode(u16 SerialNumber,u16 ID,void (*callback)(u16 AnswerSerialNumber,u16 AnswerID,u8 Result));
void GNSS_SearchAndExecute(u16 AnswerSerialNumber,u16 AnswerID,u8 Result);

#endif
