/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : lab2
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-03, 09:56, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_task - void serial_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "global_variables.h"
#include "access_functions.h"
#include <inttypes.h>
#include <stdbool.h>


int end_for =0;

#ifdef __cplusplus
extern "C" {
#endif 




/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/

void serial_task(os_task_param_t task_init_data)
{

	printf("serialTask Created!\n\r");
	char serial_buf[200];

	memset(serial_buf,0, 200);
	sprintf(serial_buf, "Type here:\n\r");
	UART_DRV_SendDataBlocking(myUART_IDX, serial_buf, sizeof(serial_buf), 1000);

#ifdef PEX_USE_RTOS
  while (1) {
#endif




	OSA_TimeDelay(10);                 /* Example code (for task release)*/

    

    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data)
{

	handlerInit();

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */

	  /* Get input from keyboard */
	  msg_ptr = _msgq_receive(0, 0);

	  memset(putline_buf,0, 200);

	  /*check how many users are reading*/
	  int check = 0;

	  if(msg_ptr == NULL){
		  printf("\nmessage not received\n");
		  _mqx_exit(0);
	  }

	  /* Get message from user task who is with writing privilege and store the string in putline queue */

	  if(msg_ptr->HEADER.TARGET_QID == putline_qid){

		  for(int i=0; (int)msg_ptr->DATA[i] != 10; i++){

			  putline_buf[i] = msg_ptr->DATA[i];
		  }
		  strcat(putline_buf, "\n");

		UART_DRV_SendDataBlocking(myUART_IDX, putline_buf, sizeof(char)*strlen(putline_buf),1000);
		UART_DRV_SendDataBlocking(myUART_IDX, &Cr, sizeof(char),500);
		memset(putline_buf,0, sizeof(char) * strlen(putline_buf));

//		  _msg_free(putline_msg_ptr);
		_msg_free(msg_ptr);
	  }

	 if(msg_ptr->HEADER.TARGET_QID == handler_qid){

	  // only print to screen if task wants to read
	  if(read_counter > 0){

		  //if printable
		  if((int)msg_ptr->DATA[0] > 31 && (int)msg_ptr->DATA[0] < 127){

			  // stops overwriting new line
			  if(output_buf[counter] == 0xa)
					counter++;

			  // revieced from isr input queue
			  output_buf[counter] = msg_ptr->DATA[0];

			  //				  printf("%c\n", output_buf[counter]);

			  UART_DRV_SendDataBlocking(myUART_IDX, &output_buf[counter], sizeof(char),1000);
			  //puts("for debug");
			  counter++;


		  }	else{
			  switch((int)msg_ptr->DATA[0]){

			  // enter key is pressed
			  case 13:
				  enterKeyPressed();
				  break;

			  // delete a character
			  case 8:
				  DeleteChar();
				  break;

			  // erase a word -> ctrl + w
			  case 23:
				  eraseWord();
				  break;

			  // erase a line -> ctrl + u
			  case 21:
				  eraseLine();
				  break;
			  default:
				  break;
			  }
		  }

		  /*for all reading users, send data to _getline.
					Data are get from keyboard */
		  delay(500);


		  // if enter key is pressed
		  if(output_buf[counter-1] == 0xa) {
			  delay(1000);
			  send_msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);

			  memset(send_msg_ptr->DATA,0, sizeof(char) * 200);

			  for(int i=0; i<strlen(output_buf); i++){
				  send_msg_ptr->DATA[i] = output_buf[i];
			  }


			  delay(1000);
			  printf("BROADCAST MESSAGE FROM HANDLER IS -->>   %s\n",send_msg_ptr->DATA);

			  // broadcast to all reading tasks
			  for(int check=0; check<read_counter; check++) {
				  delay(500);
				 send_msg_ptr->HEADER.SOURCE_QID = handler_qid;
				 send_msg_ptr->HEADER.TARGET_QID = read_privilege_list[check].user_qid;
				 send_msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) +
										strlen((char *)send_msg_ptr->DATA) + 1;
				 _msgq_send(send_msg_ptr);


			  }
			  memset(output_buf,0, sizeof(char) * strlen(output_buf));
			  counter = 0;



		  }

  }else{

	  if((int)msg_ptr->DATA[0] > 31 && (int)msg_ptr->DATA[0] < 127){

		  // stops overwriting new line
		  if(output_buf[counter] == 0xa)
				counter++;

		  // revieced from isr input queue
		  output_buf[counter] = msg_ptr->DATA[0];

		  //				  printf("%c\n", output_buf[counter]);

		  UART_DRV_SendDataBlocking(myUART_IDX, &output_buf[counter], sizeof(char),1000);
		  //puts("for debug");
		  counter++;

		  }	else{
				  switch((int)msg_ptr->DATA[0]){

				  // enter key is pressed
				  case 13:
					  enterKeyPressed();
					  break;

				  // delete a character
				  case 8:
					  DeleteChar();
					  break;

				  // erase a word -> ctrl + w
				  case 23:
					  eraseWord();
					  break;

				  // erase a line -> ctrl + u
				  case 21:
					  eraseLine();
					  break;
				  default:
					  break;
				  }
			  }
  }
	  _msg_free(msg_ptr);
  }

    //OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : user3_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user3_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */

		MSG_PTR msg_ptr;

		/*_getline queue*/
		_queue_id task_qid;
		_task_id task_send_id = 0;
		bool mygetline;
		bool get_read_privilege;
		int loop = 0;

		/*string for store data received in _getline*/
		char string[200];
		memset(string, 0, 200);

		// putline string
		char third_string[] = "this is a string coming from a task3\n ";

		/*open the _getline queue*/
		task_qid = _msgq_open(USER3_QUEUE,0);
		delay(500);
		printf("\nuser3_id-----------------------------%d\n", _task_get_id);
		delay(500);
		printf("TASK3\n");

		if (task_qid == 0) {
			printf("\nCould not open the user3 queue\n");
			_task_block();
		}





#ifdef PEX_USE_RTOS
  while (loop<3) {
#endif
    /* Write your code here ... */
	delay(500);
	printf("     TASK 3 LOOP ITERIATION === %d\n", loop+1);
	loop++;

	delay(1000);
	printf("TASK3 -> entering openW\n");
	task_send_id = OpenW();


	delay(1000);
	printf("TASK3 -> entering putline\n");
	/*call _putline right after openW*/

	_putline(task_send_id, third_string);
	/*try to get read privilege*/


	delay(500);
	printf("\nTASK 3 ENTERING openR\n");
	get_read_privilege = openR(task_qid);
	if(!get_read_privilege){
		printf("\nCould not get read privilege\n");
	}else{
//			printf("\nTaskID %i has read privilege\n", _task_get_id());
		puts("\n TASK 3 JUST GOT READ ACCESS\n");
	}


	delay(500);
	printf("TASK3 -> entering getline\n");
	/*request getline*/
	mygetline = _getline(string);
	if(!mygetline){
		puts("Cannot get line!");
	}

    OSA_TimeDelay(10);                 /* Example code (for task release) */

    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif
//		printf("\nOUTSIDE PUTLINE\n");

		/*Close itself after finishing writing line*/
//		delay(500);
//		puts("im task 3 closinggggggggggggg\n");
		Close();
		delay(500);
		printf("\n        task 3 closed\n");
		return 0;
}

/*
** ===================================================================
**     Callback    : user_task2
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task2(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
/* Write your local variable definition here */
	MSG_PTR msg_ptr;

	/*_getline queue*/
	_queue_id task_qid;
	_task_id send_qid = 0;
	bool mygetline;
	bool get_read_privilege;
	int loop = 0;

	char string[200];
	memset(string, 0, 200);
	char second_string[] = "this is a string coming from a task2\n ";

	/*open the _getline queue*/
	task_qid = _msgq_open(USER2_QUEUE,0);
	delay(500);
	printf("\nuser2_id-----------------------------%d\n", _task_get_id());
	delay(500);
	printf("TASK2\n");

	if (task_qid == 0) {
		printf("\nCould not open the user2 queue\n");
		_task_block();
	}



#ifdef PEX_USE_RTOS
  while (loop <5) {
#endif
    /* Write your code here ... */
	delay(500);
	printf("     TASK 2 LOOP ITERIATION === %d\n", loop+1);
	loop++;

	delay(1000);
	printf("TASK2 -> entering openW\n");
	send_qid = OpenW();

	// print message
	_putline(send_qid, second_string);


	/*try to get read privilege*/
	delay(500);
	printf("\nTASK 2 ENTERING openR\n");
	get_read_privilege = openR(task_qid);
	if(!get_read_privilege){
		printf("\nTASK 2 Could not get read privilege\n");
	}else{
		delay(500);
//		printf("\nTaskID %i has read privilege\n", _task_get_id());
		puts("\n TASK 2 JUST GOT READ ACCESS\n");
	}


	delay(500);
	printf("TASK2 -> entering getline\n");
	/*request getline*/
	mygetline = _getline(&string);
	if(!mygetline){
		puts("Cannot get line!");
	}



    OSA_TimeDelay(10);                 /* Example code (for task release) */




#ifdef PEX_USE_RTOS
  }
#endif

	Close();
	delay(500);
	printf("\n     task 2 closed\n");
	return 0;
}

/*
** ===================================================================
**     Callback    : user_task1
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task1(os_task_param_t task_init_data)
{
	/* Write your local variable definition here */
		MSG_PTR msg_ptr;

		/*_getline queue*/
		_queue_id task_qid;
		_queue_id send_qid;
		bool get_read_privilege;
		bool mygetline;
		int loop = 0;

		char first_string[] = "this is a string coming from a task1\n ";
		/*string for store data received in _getline*/
		char string[200];
		memset(string, 0, 200);



		/*open the _getline queue*/
		task_qid = _msgq_open(USER1_QUEUE,0);
		delay(500);
		printf("\nuser1_id-----------------------------%d\n", _task_get_id());
		delay(500);
		printf("TASK1\n");

		if (task_qid == 0) {
			printf("\nCould not open the user1 queue\n");
			_task_block();
		}



#ifdef PEX_USE_RTOS
  while (loop<3) {
#endif
    /* Write your code here ... */
	  delay(500);
	  printf("     TASK 1 LOOP ITERIATION === %d\n", loop+1);
	  loop++;


	delay(500);
	printf("TASK1 -> entering openW\n");
	/*keep requesting writing privilege until get it*/
	send_qid = OpenW();

	delay(500);
	printf("TASK1 -> entering putline\n");
	/*call _putline right after openW*/
	_putline(send_qid, first_string);

  // read the line
	delay(500);
	printf("TASK1 -> entering getline\n");
	/*request getline*/
	mygetline = _getline(&string);
	if(!mygetline){
		puts("Cannot get line!");
	}



    OSA_TimeDelay(10);                 /* Example code (for task release) */




#ifdef PEX_USE_RTOS
  }
#endif
		/*Close itself after finishing writing line*/
		delay(500);
		printf("\n    task 1 closed\n");
		Close();
}

/*
** ===================================================================
**     Callback    : user4_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user4_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	MSG_PTR msg_ptr;

	/*_getline queue*/
	_queue_id task_qid;
	_queue_id send_qid;

	char fourth_string[] = "this is a string coming from a task4\n ";
	/*string for store data received in _getline*/
	char string[200];
	memset(string, 0, 200);

	int loop = 0;
	bool mygetline;

	/*open the _getline queue*/
	task_qid = _msgq_open(USER4_QUEUE,0);
	delay(500);
	printf("\nuser4_id-----------------------------%d\n", _task_get_id());
	delay(500);
	printf("TASK4\n");

	if (task_qid == 0) {
		printf("\nCould not open the user4 queue\n");
		_task_block();
	}


  
#ifdef PEX_USE_RTOS
  while (loop < 5) {
#endif
    /* Write your code here ... */
	delay(500);
	printf("     TASK 4 LOOP ITERIATION === %d\n", loop+1);
    loop++;

	// get write privilege
	delay(500);
	printf("TASK4 -> entering openW\n");
	/*keep requesting writing privilege until get it*/
	send_qid = OpenW();

	delay(500);
	printf("TASK4 -> entering putline\n");

	_putline(send_qid, fourth_string);

//     read from handler
	delay(500);
    printf("TASK4 -> entering getline\n");
	/*request getline*/
	mygetline = _getline(&string);
	if(!mygetline){
		puts("Cannot get line!");
	}

	delay(500);
	printf("TASK4 -> entering openR\n");
    /*try to get read privilege*/
	bool get_read_privilege = openR(task_qid);
	if(!get_read_privilege){
		printf("\nCould not get read privilege\n");
	}else{
		delay(500);
		puts("\n TASK 4 JUST GOT READ ACCESS\n");
	}


    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif
	delay(500);
	printf("\n   task 4 closed\n");
	Close();
}


