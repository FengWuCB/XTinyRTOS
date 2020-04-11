/********************************************************************************
* @File name: RTOS_timer.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS ��ʱ��ģ��
********************************************************************************/
#ifndef _RTOS_TIMER_H__
#define _RTOS_TIMER_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************ͷ�ļ�����************************************/
#include "RTOS_task.h"
/*********************************����******************************************/


/*********************************���ú�************************************/

/*********************************����******************************************/

/*********************************�Զ�������************************************/
typedef enum{
	TIMER_MODE_ONCE=0,					/** һ��ģʽ */
	TIMER_MODE_INFINITE_LOOP=1	/** ����ѭ��ģʽ */
}TimerRunMode;

typedef struct {
	TASK_TCB 						mTASK_TCB;					/** �̳�������ص�tcb */
	uint32 							backCallTimer;			/** �ص���ʱ�� */
	TaskRunFunction 		funAddr;						/** �����Ļص���ַ */
	uint8 							mode;								/** ��ʱ����ģʽ */
	uint8 							resetFlag;					/** ��λ��� */
}*PTIMER_CB,TIMER_CB;
/*********************************����******************************************/

/*********************************��������************************************/
PTIMER_CB TimerCreate(
	TaskRunFunction taskFun,void *prg,uint8 level,uint32 taskNum,TimerRunMode timerMode,uint32 timerSysCount
);
void 			TimerReset(PTIMER_CB mTIMER);
uint32 		TimerStart(PTIMER_CB mTIMER);
uint32 		TimerStop(PTIMER_CB mTIMER);
uint32 		TimerSetCycleVal(PTIMER_CB mTIMER,uint32 val);
/*********************************����******************************************/
#ifdef __cplusplus
}
#endif
#endif
