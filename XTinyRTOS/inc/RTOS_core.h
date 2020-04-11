/********************************************************************************
* @File name: RTOS_Core.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS Core ģ��
********************************************************************************/
#ifndef _RTOS_CORE_H__
#define _RTOS_CORE_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************ͷ�ļ�����************************************/
#include "RTOS_type.h"
/*********************************����******************************************/

/*********************************��������************************************/
void 		StartRTOS(void);
uint32 	DisableInterrupt(void);
void 		EnableInterrupt(uint32 temp);
void 		OpenSchedule(void);
/*********************************����******************************************/

#ifdef __cplusplus
}
#endif
#endif
