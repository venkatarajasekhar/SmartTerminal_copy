#ifndef _VocoderCommon_H_BAB
#define _VocoderCommon_H_BAB

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <speex/speex.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TIM2ARRValue            9000//4500 /* TIM2 @16KHz */
#define TIM3ARRValue            9000 /* sampling rate = 8KHz => TIM3 period =72MHz/8KHz = 9000 */
#define TIM_INT_Update          ((uint16_t)(~TIM_IT_Update))
#define CR1_CEN_Set             ((uint16_t)0x0001)
/*60seconds contain 3000 frames of 20ms, and every 20ms will be encoded into 20bytes so 1min=60000bytes */
#define ENCODED_FRAME_SIZE      20
#define FRAME_SIZE              160


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



extern SpeexBits bits;/* Holds bits so they can be read and written by the Speex routines */

#endif

