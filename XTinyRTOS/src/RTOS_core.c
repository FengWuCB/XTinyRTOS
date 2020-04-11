/********************************************************************************
 * @File name: RTOS_core.c
 * @Author: zspace
 * @Version: 1.0
 * @Date: 2020-4-10
 * @Description: RTOS Core ģ��
 ********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "RTOS_core.h"
/*********************************����******************************************/

/*********************************��������************************************/
void OSScheduleProcess(void);
/*********************************����******************************************/

/*******************************************************
*
* Function name :DisableInterrupt
* Description        :�ر�оƬ���ж���Ӧ
* Parameter         :��
* Return          : PRIMASK��ֵ
**********************************************************/
__asm uint32 DisableInterrupt(void){
		REQUIRE8    					; ������������αָ���ֹ����������
		PRESERVE8   					; 8 �ֶ���	
    MRS     R0, PRIMASK  	; ��ȡPRIMASK��R0,R0Ϊ����ֵ 
    CPSID   I							; PRIMASK=1,���ж�(NMI��Ӳ��FAULT������Ӧ)
    BX      LR			    	; ����
}
/*******************************************************
*
* Function name :EnableInterrupt
* Description        :����оƬ���ж���Ӧ
* Parameter         :
*        @temp        ֮ǰ������PRIMASKֵ
* Return          : ��
**********************************************************/
__asm void EnableInterrupt(uint32 temp){
    MSR     PRIMASK, R0	  ;��ȡR0��PRIMASK��,R0Ϊ����
    BX      LR						;����
}
			
/*******************************************************
*
* Function name :StartRTOS
* Description        :��������ϵͳ
* Parameter         :��
* Return          : ��
**********************************************************/
__asm void StartRTOS(void){
			CPSID   I 
			;��������pendsvΪ������ȼ�
			;����pendsv���ж����ȼ�
			ldr r0,=0xE000ED22
			;������ȼ�
			ldr r1,=0xff
			;����
			strb r1,[r0]
			;����pspΪ0,�����ж��Ƿ��һ���������
			MOVS R0, #0 ;R0 = 0
			MSR PSP, R0 ;PSP = R0
			;����pendsv�ж�
			LDR R0, =0xE000ED04 ;R0 = 0xE000ED04
			LDR R1, =0x10000000 ;R1 = 0x10000000
			;���ô���
			STR.w R1, [R0] ;*(uint32_t *)NVIC_INT_CTRL = NVIC_PENDSVSET
			;���ж�
			CPSIE I ;
			;��ѭ��
os_loop 
			B os_loop
}

/*******************************************************
*
* Function name :StartRTOS
* Description        :����pendsv�жϣ��Ա���в���ϵͳ����
* Parameter         :��
* Return          : ��
**********************************************************/
__asm void OpenSchedule(void){
				;push {r0-r1,lr}
				LDR R0, =0xE000ED04
				LDR R1, =0x10000000 
				;����pendsv�ж�
				STR R1, [R0]
				;pop	  {r0-r1,pc}
				bx lr
}
/*******************************************************
*
* Function name :PendSV_Handler
* Description        :pendsv�жϺ������ú����ֽ��������л�
* Parameter         :��
* Return          : ��
**********************************************************/
__asm void PendSV_Handler(void){
		
				IMPORT OSTaskSW
				IMPORT OSLastTaskMem
	
        CPSID   I
				;���spָ���ֵ
				MRS R0, PSP ;R0 = PSP
					
				;�����һ��ִ��,��ִ��һ���жϵ���
				CBZ R0, thread_change 
				;���ǵ�һ���򱣻�r4-r11
				SUBS R0, R0, #0x20 ;R0 -= 0x20
				STM R0, {R4-R11} ;		
				
				;���汾�ε�ջ����;
				;�޸��ϴ������ջ��ֵ
				;Ϊ�˳��ֱ�֤�´��л������ִ���
				ldr r1,=OSLastTaskMem
				ldr r1,[r1]
				str R0,[r1]					
thread_change	;�������
				push {lr}
				ldr.w r0,=OSTaskSW
				blx r0
				pop  {lr}

				LDM R0, {R4-R11} ;�ָ��µ�r4-r11��ֵ
				ADDS R0, R0, #0x20 ;R0 += 0x20
				MSR PSP, R0
				;�л����û��߳�ģʽ
				;lr �ĵ�2λΪ1ʱ�Զ��л�
				ORR LR, LR, #0x04  
				;���ж�
				cpsie I
				BX 	LR	
}
/*******************************************************
*
* Function name :SysTick_Handler
* Description        :systick���жϺ���
* Parameter         :��
* Return          :��
**********************************************************/
void SysTick_Handler(void){
	OSScheduleProcess();
}

