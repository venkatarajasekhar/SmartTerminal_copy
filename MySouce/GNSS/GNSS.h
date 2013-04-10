#ifndef _GNSS_H_BAB
#define _GNSS_H_BAB

#include "GNSS_BaseType.h"
#include "GNSS_IO.h"
/**************************************
*** 消息ID列表
*** 参见终端协议表B.1
***************************************/
typedef enum
{
	eTerminalCommonACK=0x0001,  ///<终端通用应答
	eServerCommonACK  =0x8001,    ///<平台通用应答
	eTerminalHeartbeat=0x0002,///<终端心跳
	eTerminalRegister=0x0100,///<终端注册
	eTerminalRegisterACK=0x8100,///<终端注册应答
	eTerminalUnregister=0x0101,///<终端注销
	eTerminalAuthenticate=0x0102,///<终端鉴权
	eSetTerminalParameters=0x8103,///<设置终端参数
	eGetTerminalParameters=0x8104,///<查询终端参数
	eGetTerminalParametersACK=0x0104,///<查询终端参数应答
	eTerminalControl=0x8105,///<终端控制
	eLocationInformationReport=0x0200,///<位置信息汇报
	eGetLocationInformation=0x8201,///<查询位置信息
	eGetLocationInformationACK=0x0201,///<查询位置信息应答
	eTemporaryPositionTrackingControl=0x8202,///<临时位置跟踪控制
	eTextMessage=0x8300,///<文本信息下发
	eEventSet=0x8301,///<事件设置
	eEventReport=0x0301,///<事件报告
	eQuestionDownlink=0x8302,///<提问下发
	eQuestionACK=0x0302,///<提问应答
	eSetInformationMenu=0x8303,///<信息点播菜单设置
	eInformationBookOrCancel=0x0303,///<信息点播/取消
	eStorageMultimediaDataSearch=0x8802,///<存储多媒体数据检索
	eStorageMultimediaDataSearchACK=0x0803,///<存储多媒体数据检索应答
	eStorageUploadMultimediaDataCMD=0x8803,///<存储多媒体数据上传命令
	eStartRecording=0x8804,///<录音开始命令
	eDownlinkDataWithTransparent=0x8900,///<数据下行透传
	eUplinkDataWithTransparent=0x0900,///<数据上行透传
	eInformationServices=0x8304,///<信息服务
	eCallBack=0x8400,///<电话回拨
	eSetPhoneBook=0x8401,///<设置电话本
	eCarControl=0x8500,///<车辆控制
	eCarControlACK=0x0500,///<车辆控制应答
	eSetCircularArea=0x8600,///<设置圆形区域
	eDeleteCircularArea=0x601,///<删除圆形区域
	eSetRectangleArea=0x8602,///<设置矩形区域
	eDeleteRectangleArea=0x8603,///<删除矩形区域
	eSetPolygonArea=0x8604,///<设置多边形局域
	eDeletePolygonArea=0x8605,///<删除多边形局域
	eSetRoute=0x8606,///<设置路线
	eDeleteRoute=0x8607,///<删除路线
	eVehicleTravellingDataRecorderCollectCMD=0x8700,///<行驶记录仪数据采集命令
	eGetVehicleTravellingDataRecorderDataCMD=0x0700,///<行驶记录仪数据上报命令
	eVehicleTravellingDataRecorderParameters=0x8701,///<行驶记录仪参数下达命令
	eE_WaybillReported=0x0701,///<电子运单上报
	eDriverInfo =0x0702,///<驾驶员身份信息采集上报
	eUploadMultimediaEventInformation=0x0800,///<多媒体事件信息上传
	eUploadMultimediaData=0x0801,///<多媒体数据上传
	eUploadMultimediaDataACK=0x8800,///<多媒体数据上传应答
	eCameraShotImmediately=0x8801,///<摄像头立即拍摄命令
	eDataCompressionReport=0x0901,///<数据压缩上报
	eServerPublicRSAKey=0x8A00,///<平台RSA公钥
	eTerminalPublicRSAKey=0x0A00,///<终端RSA公钥
	eServerDownlinkMessageReserve=0x8F00,///<服务器下行消息保留
	eTerminalUplinkMessageReserve=0x0900,///<终端上行消息保留
	//以下为自定义
	eSendVoiceData=0x0AA0,///<语音数据
	eReceiveVoiceData=0x0AA1,///<语音数据
	eSendATcommand=0x0AA2,  ///<AT指令数据
	eGPSData=0x0AA3  ///<GPS数据，只用于测试，在协议中没有

} MessageIDTypeDef;
typedef struct
{
	u16 MessageLen:10;      ///<bit0~9     消息体长度
	u16 EncryptionMode:3;   ///<bit10~12   加密方式
	u16 MultiPacket:1;      ///<bit 13     分包标志
	u16 Reserve:2;          ///<bit 14~15  保留
} MessagePropertyBitTypeDef;
typedef union
{
	MessagePropertyBitTypeDef Bit;
	u16 All;
} MessagePropertyTypeDef;
typedef struct
{
	u16 TotalMessagePackets;  ///<消息总包数
	u16 PackageNo;            ///<包序号
} MessagePacketPropertiesTypeDef;
typedef struct
{
	MessageIDTypeDef MessageID;                              ///<消息ID
	MessagePropertyTypeDef MessageProperty;                  ///<消息体属性
	u8 PhoneNumber[6];                                       ///<终端手机号
	u16 MessageSerialNumber;                                 ///<消息流水号
	MessagePacketPropertiesTypeDef MessagePacketProperties;  ///<消息包封装项
} MessageHeadTypeDef;
typedef union
{
	MessageHeadTypeDef MessageHead;
	u8  MessageHeadBuf[sizeof(MessageHeadTypeDef)];
} MessageHeadUnionTypeDef;
///消息结构体
typedef struct
{
	u8 StartFlagByte;    ///<标识位
	MessageHeadUnionTypeDef MessageHeadUnion;        ///<消息头
	u8 *pMessageBody;                      ///<消息体
	u8 Data[1024];						   ///<数据区
	u8 BCC;                                   ///<异或校验码
	u8 EndFlagByte;                          ///<标识位
	//MessageIDTypeDef MessageID;
} MessageStructTypeDef;

///通用应答结构体
#define dSuccess		0
#define	dFail		1
#define	dError		2
#define	dNotSupport	3
typedef struct
{
	u16 AnswerSerialNumber;///<应答流水号
	u16 AnswerID;          ///<应答ID
	u8  Result;            ///<结果
}GeneralACKStructTypeDef; 
///////////////////////////////////////////////////////////
#define GNSS_FrameBytes_MAX	1024	//协议最大字节数
#define GNSS_RX_BUF_MAX		5		//缓冲区个数
/**
  数据节点
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
	u32 EmergencyAlarm:1;				///<bit 0 紧急报警触动报警开关触发
	u32 SpeedingAlarm:1;                ///<bit 1 超速报警
	u32 tiredAlarm:1;					///<bit 2 疲劳驾驶报警
	u32 Warning:1;						///<bit 3 预警
	u32 GNSSWarning:1;					///<bit 4 GNSS模块发生故障
	u32 GNSSAntennaOpenCircuit:1;		///<bit 5 GNSS天线未接或剪短
	u32 GNSSAntennaShortCircuit:1;		///<bit 6 GNSS天线未接或剪短
	u32 MainPowerLow:1;					///<bit 7 终端主电源欠压
	u32 MainPowerDown:1;				///<bit 8 终端主电源掉电
	u32 LCDWarning:1;					///<bit 9 终端LCD或显示器故障
	u32 TTSWarning:1;					///<bit 10 TTS模块故障
	u32 CameraWarning:1;				///<bit 11 摄像头故障
	u32 Reserve:6;						///<bit 12~17 保留
	u32 TheDayTotalDrivingTimeWarning:1;///<bit 18 当天累计驾驶超时
	u32 ParkingOvertimeWarning:1;		///<bit 19 超时停车
	u32 InOutArea:1;					///<bit 20 进出区域
	u32 InOutLine:1;					///<bit 21 进出线路
	u32 RoadContinueTimeWarning:1;		///<bit 22 路段行驶时间不足/过长
	u32 RoadDeviateWarning:1;			///<bit 23 线路偏离报警
	u32 VSSWarning:1;					///<bit 24 车辆VSS故障
	u32 OilWarning:1;					///<bit 25车辆油量异常
	u32 StolenWarning:1;				///<bit 26车辆被盗
	u32 IllegalStart:1;					///<bit 27 车辆非法点火
	u32 IllegalMove:1;					///<bit 28 车辆非法移动
	u32 Reserve1:3;						///<bit 29~31保留

}AlarmBit;  ///<报警标志位

typedef union
{
	AlarmBit Bit;
	u32 All;
}AlarmFlageTypeDef; ///<报警标志

typedef struct 
{
	u32 ACCEnable:1;						///<bit 0  ACC状态 0：关  1：开
	u32 LocateOrNot:1;						///<bit 1  0：未定位 1;定位
	u32 Latitude:1;							///<bit 2  0：北纬 1：南纬
	u32 Longitude:1;						///<bit 3  0：东经 1：西经
	u32 OperatingStatus:1;					///<bit 4  0：营运状态 1：停运状态
	u32 LatitudeAndLongitudeEncryption:1;	///<bit 5  经纬度加密  1:加密
	u32 Reserve1:4;							///<bit 6~9保留
	u32 OilState:1;							///<bit 10  0:油路正常  1：油路断开
	u32 CircuitState:1;						///<bit 11  0：车辆电路正常  1：车辆电路断开
	u32 DoorState:1;						///<bit 12  0：车门解锁 1:车门加锁
	u32 Reserve2:19;						///<bit 13~31保留 

}StateTypeDef;  ///<状态位定义

typedef union
{
	StateTypeDef Bit;
	u32 All;
}CarStateTypeDef;
typedef struct 
{
	AlarmFlageTypeDef AlarmFlage;	///<报警标志
	CarStateTypeDef   CarState;		///<状态
	u32               Latitude;		//纬度
	u32	              Longitude;	///<经度
	u16               Altitude;		///<高程
	u16               Speed;		///<速度 1/10km/H
	u16               Direction;	///<方向 0~359 ,正北为0 ，顺时针
	u8                TimeBCD[6];	///<YY-MM-DD-hh-mm-ss (GMT+8时间)

}LocateBasicInfoTypeDef; ///<位置基本信息

typedef enum 
{
	eMileage=0x01,					///<里程读数
	eOil=0x02,						///<油量
	eSpeed=0x03,					///<速度
	eSpeedAlarmInfo=0x11,			///<超速报警信息
	eInOutInfo=0x12,				///<进出区域/路线报警附加信息
	eRoadContinueTimeWarning=0x13	///<路段行驶时间不足/过长报警附加信息

}ExtraInfoIDTypeDef; ///<附加信息ID

typedef struct 
{
	u8	ExtraInfoID; ///<附加信息ID
	u8 	ExtraInfoLen;///<附加信息长度
	u8  ExtraInfo[7];///<附加信息的长度最大为7个字节
}ExtraLocateInfoTypeDef;///<位置附加信息

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
	u32 len;	//附加
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
