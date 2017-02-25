/* ###################################################################
**     Filename    : Events.c
**     Project     : lab2
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-03, 09:43, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "global_variables.h"
#include "access_functions.h"

#ifdef __cplusplus
extern "C" {
#endif 

_pool_id msg_pool;

/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : myUART_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/
void myUART_RxCallback(uint32_t instance, void * uartState)
{
  /* Write your code here ... */
	/*modify the input for interruption in order to do special characters*/

	//debug array
	char err[20];

	MSG_PTR msg_ptr;
	_queue_id output_qid;
	output_qid = _msgq_open(OUTPUT_QUEUE, 0);

	//allocate a msg
	msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);


	 msg_ptr->HEADER.SOURCE_QID = output_qid;
	 msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, HANDLER_QUEUE);
	 msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) +
			 	 	 	 	strlen((char *)msg_ptr->DATA) + 1;
	 msg_ptr->DATA[0] = myRxBuff[0];

	 _msgq_send(msg_ptr);

	 _msg_free(msg_ptr);


}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
