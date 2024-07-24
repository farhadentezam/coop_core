/*
------------------------------------------------------------------------------
~ File   : coop_core.cpp

------------------------------------------------------------------------------
~ Description:

~ Attention  :

~ Changes    :
------------------------------------------------------------------------------
*/

#include "coop_core.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::::::::::: Initialize :::::::::::::::::: */
uint8_t CoOpCore::Task_RegisterNew(cTask_TypeDef *uTask, cTaskFunc_TypeDef uFunc)
{
	
	if (TaskControl.NMBOfRegisteredTasks == _COOP_NMB_OF_TASK)
	{
		return 0;
	}
	else
	{
		/* Set the task parameters */
		TaskList[TaskControl.NMBOfRegisteredTasks] = uTask;
		
		TaskList[TaskControl.NMBOfRegisteredTasks]->Instances = TaskControl.NMBOfRegisteredTasks;
		TaskList[TaskControl.NMBOfRegisteredTasks]->Delay     = 0;
		TaskList[TaskControl.NMBOfRegisteredTasks]->State     = cTaskRunning;
		TaskList[TaskControl.NMBOfRegisteredTasks]->ActivatedStep   = 0;
		TaskList[TaskControl.NMBOfRegisteredTasks]->Function  = uFunc;
		
		/* ....................... */
		TaskControl.NMBOfRegisteredTasks++;
		
		return 1;
	}
	
}

/* :::::::::::::::: Get Parameter ::::::::::::::::: */
uint8_t CoOpCore::Task_GetStepParam(uint8_t *paramIn)
{
	return *paramIn;
}

/* :::::::::::::::: Set Parameter ::::::::::::::::: */
void CoOpCore::Task_SetNextStepParam(uint8_t nexStep)
{
	TaskList[TaskControl.TaskCounter]->ActivatedStep = nexStep;
}

/* :::::::::::::::::: Task Timing ::::::::::::::::: */
void CoOpCore::Task_IncTick(void)
{
	
	for (TaskControl.TickCounter = 0; TaskControl.TickCounter < TaskControl.NMBOfRegisteredTasks; TaskControl.TickCounter++)
	{
		
		if (TaskList[TaskControl.TickCounter]->State == cTaskWait)
		{
			
			if (TaskList[TaskControl.TickCounter]->Delay)
			{
				TaskList[TaskControl.TickCounter]->Delay--;
			}
			
		}
		
	}
	
}

void CoOpCore::Task_Delay(uint32_t tim, uint8_t step)
{
	/* Set task parameters */
	if (TaskList[TaskControl.TaskCounter]->State == cTaskRunning)
	{
		
		TaskList[TaskControl.TaskCounter]->Delay   = tim;
		TaskList[TaskControl.TaskCounter]->State   = cTaskWait;
		TaskList[TaskControl.TaskCounter]->ActivatedStep = step;
		
	}
	
}

/* ::::::::::::::::: Task Control ::::::::::::::::: */
/* .... Task Resume .... */
void CoOpCore::Task_Resume(cTask_TypeDef *uTask)
{
	uTask->State = cTaskRunning;
}
void CoOpCore::Task_ResumeAll(void)
{
	
	uint8_t tCounter = 0;
	
	/* ~~~~~~~~~~ Suspend tasks ~~~~~~~~~~ */
	for (; tCounter < TaskControl.NMBOfRegisteredTasks; tCounter++)
	{
		TaskList[tCounter]->State = cTaskRunning;
	}
	
}

/* .... Task Suspend .... */
void CoOpCore::Task_Suspend(cTask_TypeDef *uTask)
{
	uTask->State = cTaskSuspended;
}
void CoOpCore::Task_SuspendAll(void)
{
	
	uint8_t tCounter = 0;
	
	/* ~~~~~~~~~~ Suspend tasks ~~~~~~~~~~ */
	for (; tCounter < TaskControl.NMBOfRegisteredTasks; tCounter++)
	{
		TaskList[tCounter]->State = cTaskSuspended;
	}
	
}

/* .... Task Delete .... */
void CoOpCore::Task_Delete(cTask_TypeDef *uTask)
{
	
	uint8_t tCounter = uTask->Instances;
	
	/* ~~~~~~~~~~ Check availability ~~~~~~~~~~ */
	if (uTask->State != cTaskDeleted)
	{
		
		/* ~~~~~~~~~~ Delete task ~~~~~~~~~~ */
		for (; tCounter < (TaskControl.NMBOfRegisteredTasks - 1); tCounter++)
		{
			TaskList[tCounter] = TaskList[tCounter + 1];
			
			/* .... Set new instances .... */
			TaskList[tCounter]->Instances = tCounter;
			
		}
		
		TaskControl.NMBOfRegisteredTasks--;
		
		/* Set new state */
		uTask->State = cTaskDeleted;
		
	}
	
}
void CoOpCore::Task_DeleteAll(void)
{
	TaskControl.NMBOfRegisteredTasks = 0;
}

/* .... Task Step .... */
void CoOpCore::Task_SetStep(cTask_TypeDef *uTask, uint8_t step)
{
	uTask->ActivatedStep = step;
}

/* ::::::::::::::::::: Task Run ::::::::::::::::::: */
void CoOpCore::Task_RunCoOpSystem(void)
{
	
	while (1)
	{
		
		for (TaskControl.TaskCounter = 0; TaskControl.TaskCounter < TaskControl.NMBOfRegisteredTasks; TaskControl.TaskCounter++)
		{
			
			switch (TaskList[TaskControl.TaskCounter]->State)
			{
				
				case cTaskRunning:
				{
					TaskList[TaskControl.TaskCounter]->Function(&TaskList[TaskControl.TaskCounter]->ActivatedStep);
				}
				break;
				case cTaskWait:
				{
					
					if (TaskList[TaskControl.TaskCounter]->Delay == 0)
					{
						
						TaskList[TaskControl.TaskCounter]->State = cTaskRunning;
						TaskList[TaskControl.TaskCounter]->Function(&TaskList[TaskControl.TaskCounter]->ActivatedStep);
						
					}
					
				}
				break;
				case cTaskSuspended:
				break;
				case cTaskDeleted:
				break;
				
			}
			
		}
		
	}
	
}
