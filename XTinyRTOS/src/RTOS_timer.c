/********************************************************************************
* @File name: RTOS_timer.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS ��ʱ��ģ��
********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "RTOS_timer.h"
#include "RTOS_mem.h"
#include "RTOS_task.h"
/*********************************����******************************************/

/*******************************************************
*
* Function name :TimerCreate
* Description        :����һ����ʱ�������̿�ʼ����
* Parameter         :
*        @taskFun       ��ʱ������
*        @prg    			���ݵĲ���
*        @taskNum    ջ��С
*        @timerMode    ����ģʽ
*        @timerSysCount    ��ʱ��ʱ��
* Return          : ���󷵻�NULL
**********************************************************/
PTIMER_CB TimerCreate(
	TaskRunFunction taskFun,void *prg,uint8 level,uint32 taskNum,TimerRunMode timerMode,uint32 timerSysCount
){
	PTIMER_CB pTimerCB;
	uint32 *mem;
	/*������ƿ���ڴ�*/
	pTimerCB=(PTIMER_CB)OSMalloc(sizeof(TIMER_CB));
	if(pTimerCB==NULL){
		return NULL;
	}
	/*�����ջ���ڴ�*/
	mem=(uint32 *)OSMalloc(sizeof(uint32)*taskNum);
	if(mem == NULL){
		OSFree(pTimerCB);
		return NULL;
	}
	/*��������*/
	if(
		OSCreateBaseManual((TASK_TCB *)pTimerCB,taskFun,prg,level,&mem[taskNum-1],taskNum,TASK_MODE_TIMER)==FALSE
	){
		/*��������ˣ�����ʧ����*/
		OSFree(mem);
		OSFree(pTimerCB);
		return NULL;
	}
	OSEnterExclusiveMode();
	pTimerCB->backCallTimer=timerSysCount;		/*����ʱ��ص�һ��,һ����������ʱ*/
	pTimerCB->mode=timerMode;									/*��ʱ��ģʽ*/
	pTimerCB->resetFlag=0;										/*����δ��λģʽ*/
	pTimerCB->mTASK_TCB.status=FALSE;					/*��ʱ����ʼ����ֹͣ״̬*/
	pTimerCB->funAddr=taskFun;								/*�����ĵ�ַ*/
	OSExitExclusiveMode();
	return pTimerCB;
}

/*******************************************************
*
* Function name :TimerSetCycleVal
* Description        :���ö�ʱ���Ķ�ʱֵ
* Parameter         :
*        @pTIMER       ��ʱ������ָ��
*        @val    			ѭ��ֵ
* Return          : TRUE or FALSE
**********************************************************/
uint32 TimerSetCycleVal(PTIMER_CB pTIMER,uint32 val){
	if(pTIMER==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pTIMER->backCallTimer=val;
	OSExitExclusiveMode();
	return TRUE;		
}
/*******************************************************
*
* Function name :TimerStop
* Description        :ֹͣ��ʱ��
* Parameter         :
*        @pTIMER       ��ʱ������ָ��
* Return          : TRUE or FALSE
**********************************************************/
uint32 TimerStop(PTIMER_CB pTIMER){
	if(pTIMER==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pTIMER->mTASK_TCB.status=FALSE;
	OSExitExclusiveMode();
	return TRUE;	
}

/*******************************************************
*
* Function name :TimerStart
* Description        :�����ⶨʱ��
* Parameter         :
*        @pTIMER       ��ʱ������ָ��
* Return          : TRUE or FALSE
**********************************************************/
uint32 TimerStart(PTIMER_CB pTIMER){
	if(pTIMER==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pTIMER->mTASK_TCB.status=TRUE;
	OSExitExclusiveMode();
	return TRUE;
}
/*******************************************************
*
* Function name :TimerDel
* Description        :ɾ����ʱ��
* Parameter         :��
* Return          : ��
**********************************************************/
void TimerDel(void){
	/* ����ֱ�ӵ���task��ɾ����������Ϊ�̳���task */
	OSTaskDel();
}
/** 
 * ��λ��ʱ�����ʼ��״̬
 * @param[in] mTIMER ��ʱ����ָ��
 * @retval  ��
 */
/*******************************************************
*
* Function name :TimerReset
* Description        :��λ��ʱ�����ʼ��״̬
* Parameter         :
*        @pTIMER       ��ʱ������ָ��
* Return          : ��
**********************************************************/
void TimerReset(PTIMER_CB pTIMER){
	uint32 *mem;
	OSEnterExclusiveMode();
	pTIMER->mTASK_TCB.memTask=pTIMER->mTASK_TCB.memBottomTask;	/*��λջ����ָ��*/
	pTIMER->mTASK_TCB.delayCount=pTIMER->backCallTimer;	/*��λ��ʱֵ*/
	pTIMER->mTASK_TCB.runCount=0;	/*��λ���е�ʱ��*/
	pTIMER->resetFlag=1;		/*����Ϊ��λģʽ*/
	mem=pTIMER->mTASK_TCB.memTask;
	/* Registers stacked as if auto-saved on exception */
	*(mem) = (uint32)0x01000000L; /* xPSR */
	*(--mem) = (uint32)(pTIMER->funAddr); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem) = (uint32)OSTaskEnd;
	*(--mem) = (uint32)0x12121212L; /* R12*/
	*(--mem) = (uint32)0x03030303L; /* R3 */
	*(--mem) = (uint32)0x02020202L; /* R2 */
	*(--mem) = (uint32)0x01010101L; /* R1 */
	*(--mem) = (uint32)0x00000000L; /* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem) = (uint32)0x11111111L; /* R11 */
	*(--mem) = (uint32)0x10101010L; /* R10 */
	*(--mem) = (uint32)0x09090909L; /* R9 */
	*(--mem) = (uint32)0x08080808L; /* R8 */
	*(--mem) = (uint32)0x07070707L; /* R7 */
	*(--mem) = (uint32)0x06060606L; /* R6 */
	*(--mem) = (uint32)0x05050505L; /* R5 */
	*(--mem) = (uint32)0x04040404L; /* R4 */
	pTIMER->mTASK_TCB.memTask=mem;
	OSExitExclusiveMode();
	OpenSchedule();
}
