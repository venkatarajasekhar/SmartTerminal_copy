#ifndef _GNSS_Platform_H_BAB
#define _GNSS_Platform_H_BAB

#include "GNSS.h"

void GNNS_P_GeneralACK(GeneralACKStructTypeDef *pTerminalGeneralACKStruct);
void GNSS_S_ACK_Heartbeat(u16 AnswerSerialNumber,u16 AnswerID);
void GNSS_S_ACK_Register(MessageHeadTypeDef *pMessageHead,u8 *pBodyBuf);



#endif
