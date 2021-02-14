/****************************************************************************************
* uCos-II Multitasking Application Code
*****************************************************************************************
*
* Program Name : 
* Description  : This program creates total four uCos-II tasks; out of which
*                main task acts as a parent task and create three child tasks
*
* 
* MicroEmbedded Technologies
* Processor : NXP LPC2148
* Board		: Micro-A748
*
****************************************************************************************/

#include <includes.h>
#include "func.h"
#include "keypad.h"

int count=0;
int current=0;

OS_EVENT *MessageQ;
void *Msg[10];
OS_EVENT *MailBox_1;

/********** Define Task Priorities ***********/
//#define  APP_TASK_START_PRIO                   4
#define  status_check_PRIO                        5
#define  window_1_PRIO                        6
#define  window_2_PRIO                        7
#define  window_3_PRIO                       8
#define  token_display_PRIO                         9


/*--------------- AAPLICATION STACKS ---------*/
//static  OS_STK       AppTaskStartStk[APP_TASK_STK_SIZE];
static  OS_STK       status_checkstk[APP_TASK_STK_SIZE];		/* Create the required number of stacks need for every child task*/
static  OS_STK       window_1stk[APP_TASK_STK_SIZE];
static  OS_STK       window_2stk[APP_TASK_STK_SIZE];
static  OS_STK       window_3stk[APP_TASK_STK_SIZE];
static  OS_STK       token_displaystk[APP_TASK_STK_SIZE];


/*-------------LOCAL FUNCTION PROTOTYPES--------------*/

/*--------------- A PARENT TASK (MAIN TASK) ---------*/
//static  void  AppTaskStart (void        *p_arg); 			 /* Main(Parent) Task Function */
static  void  AppTaskCreate(void);				  			 /* Separate Function To Create Child Task(s) */

/*--------------- CHILDERN TRASKS --------------*/
static  void  status_check   	 (void        *p_arg);			 
static  void  window_1     	 (void        *p_arg);			 
static  void  window_2  	 (void        *p_arg);
static  void  window_3  	 (void        *p_arg);
static  void  token_display  	 (void        *p_arg);


int  main (void)
{
    BSP_IntDisAll();                          /* Disable all interrupts until we are ready to accept them */
    OSInit();                                 /* Initialize "uC/OS-II, The Real-Time Kernel"              */
		
		LEDInit();
		InitLCD();
		keypadInit();
		MessageQ= OSQCreate(&Msg[0], 10);
		MailBox_1=OSMboxCreate((void*)0);
	
    OSTaskCreate(status_check,                               /* Create the starting task i.e. Main Task        */
                    (void *)0,
                    (OS_STK *)&status_checkstk[APP_TASK_STK_SIZE - 1],
                    status_check_PRIO);
		OSTaskCreate(window_1,                               /* Create the starting task i.e. Main Task        */
                    (void *)0,
                    (OS_STK *)&window_1stk[APP_TASK_STK_SIZE - 1],
                    window_1_PRIO);
				OSTaskCreate(window_2,                               /* Create the starting task i.e. Main Task        */
                    (void *)0,
                    (OS_STK *)&window_2stk[APP_TASK_STK_SIZE - 1],
                    window_2_PRIO);
				OSTaskCreate(window_3,                               /* Create the starting task i.e. Main Task        */
                    (void *)0,
                    (OS_STK *)&window_3stk[APP_TASK_STK_SIZE - 1],
                    window_3_PRIO);
		OSTaskCreate(token_display,                               /* Create the starting task i.e. Main Task        */
                    (void *)0,
                    (OS_STK *)&token_displaystk[APP_TASK_STK_SIZE - 1],
                    token_display_PRIO);
		

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}



/*
 *	In this function we will check which key is pressed and accordingly action will be taken.
 *	Key 1: Window_1 is ready to attend the customer. Resume the task window_1 and suspend the rest.
 *	Key 2: Window_2 is ready to attend the customer 
 *	Key 3: Window_3 is ready to attend the customer
 *	Key 4: New customer has arrived.	Increment the count variable(token number) and send it to both mailbox(because we need to display on the screen what is the token number of the newly arrived customer)
					 and message queue(because we need to keep a track of all the token numbers). Then resume the task token_display and suspend the rest.
 */

static  void  status_check (void *p_arg)
{
//   //p_arg = p_arg;									   /*Just to avoid compiler Warning 			*/
			INT8U err;
			int count_temp=0;
			int window_check=0;
			int temp[10]={0},i=2,k=0,l=0;
			static int j=0;
			LEDset(5);
			while(1)
			{     
					LCD_cmd(0X01);
					window_check= keypad();
	 /* User Code Here */
				if(window_check!=0)
			{
				switch(window_check)
				{
					case 1:
						window_check=0;
						OSTaskResume(window_1_PRIO);
						OSTaskSuspend(window_2_PRIO);
						OSTaskSuspend(window_3_PRIO);
						OSTaskSuspend(token_display_PRIO);
						OSTaskSuspend(status_check_PRIO);
						break;
					
					case 2:
						window_check=0;
						OSTaskSuspend(window_1_PRIO);
						OSTaskSuspend(window_3_PRIO);
						OSTaskSuspend(token_display_PRIO);
						OSTaskResume(window_2_PRIO);
						OSTaskSuspend(status_check_PRIO);
						break;
					
					case 3:
						window_check=0;
						OSTaskSuspend(window_2_PRIO);
						OSTaskSuspend(window_1_PRIO);
						OSTaskSuspend(token_display_PRIO);
						OSTaskResume(window_3_PRIO);
						OSTaskSuspend(status_check_PRIO);
						break;
					
					case 4: 
						
						count++;
						count_temp=count;
					  i=0;
						while(i==0) 
						{
 						temp[j]=count_temp;
						i++;
						}

						err=OSQPost(MessageQ,&temp[j]);
						err=OSMboxPost(MailBox_1,&temp[j]);
						j++;
					
					OSTaskSuspend(window_1_PRIO);
					OSTaskSuspend(window_2_PRIO);
					OSTaskSuspend(window_3_PRIO);
					OSTaskResume(token_display_PRIO);
					OSTaskSuspend(status_check_PRIO);
						
				}
		}
			
	}

}


static  void  window_1 (void *p_arg)
{
   //p_arg = p_arg;											 /* Just to avoid compiler Warning 			*
			INT8U err;
			int *msg;
			int i=0,j=0;
			unsigned char message_1[]="Token number ";
			unsigned char message_2[]="Counter 1 ";
			unsigned char token[]="0123456789";
			msg=(int*) OSQPend(MessageQ,0,&err);
   while(1)
   {
		 LCD_display(1,1,message_1);
		 LCD_display_1(1,14,msg++,token);
		 LCD_display(2,1,message_2);
		 delay(500);
		 OSTaskResume(status_check_PRIO);
		 }

}



static  void  window_2 (void *p_arg)
{
	INT8U err;
	int *msg;
	int i=0,j=0;
	unsigned char message_1[]="Token number ";
	unsigned char message_2[]="Counter 2 ";
	unsigned char token[]="0123456789";
	msg=(int*) OSQPend(MessageQ,0,&err);
   while(1)
   {
		 LCD_display(1,1,message_1);
		 LCD_display_1(1,14,msg++,&token);
		 LCD_display(2,1,message_2);
  	 delay(500);
	 	 OSTaskResume(status_check_PRIO);
		 }
}

static  void  window_3 (void *p_arg)
{
	INT8U err;
	int *msg;
	int i=0,j=0;
	unsigned char message_1[]="Token number ";
	unsigned char message_2[]="Counter 3 ";
	unsigned char token[]="0123456789";
	msg=(int*) OSQPend(MessageQ,0,&err);
   while(1)
   {
		 LCD_display(1,1,message_1);
		 LCD_display_1(1,14,msg++,&token);
		 LCD_display(2,1,message_2);
		 delay(500);
		 OSTaskResume(status_check_PRIO);
		}

}

static  void  token_display (void *p_arg)
{
  // p_arg = p_arg;										/* Just to avoid compiler Warning 		*/
	INT8U err;
	int *msg;
	int j=0, k=0;
	unsigned char a[]="Token number ";
	unsigned char token[]="0123456789";
	msg=(int*)OSMboxPend(MailBox_1,0,&err);
   while(1)
    {
			LCD_display(1,1,a);
			LCD_display_1(1,14,msg++,token);
			delay(500);
			OSTaskResume(status_check_PRIO);
		}	  
}



static  void  AppTask5 (void *p_arg)
{
  // p_arg = p_arg;										/* Just to avoid compiler Warning 		*/
 
    while(DEF_TRUE)
    {


   }	  
}

static  void  AppTask6 (void *p_arg)
{
  // p_arg = p_arg;										/* Just to avoid compiler Warning 		*/
 
    while(DEF_TRUE)
    {


   }	  
}







/*
*********************************************************************************************************
*********************************************************************************************************
**                                         uC/OS-II APP HOOKS
**										(PLEASE IGONRE THE CODE BELOW)
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (OS_VIEW_MODULE > 0)
    OSView_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (OS_VIEW_MODULE > 0)
    OSView_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (OS_VIEW_MODULE > 0)
    OSView_TickHook();
#endif
}
#endif
#endif