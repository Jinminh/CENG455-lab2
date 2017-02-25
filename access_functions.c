/*
 * access_functions.c
 *
 *  Created on: Feb 7, 2017
 *      Author: nguyen06
 */
#include "access_functions.h"

bool openR(uint16_t stream_no){	// stream_no is the queue id for the task to recieve data

	//get task id and set it to user_id
	_task_id user_id = _task_get_id();

	bool has_read_privilege = false;

	// checks if task already has read privilege
    for (int i=0; i < read_counter; i++) {
        if (read_privilege_list[i].user_id == user_id){
        	has_read_privilege = true;
        	break;
        }
    }

	// return true only if task was assigned read privilege
    if(has_read_privilege)
    	return false;

	//mutex lock here to prevent multiple access of the read privilege struct

	// locks mutex
	mutex_Var = _mutex_lock(&read_mutex);
    if(mutex_Var != MQX_EOK){
    	printf("Mutex lock failed.\n");
    	_mqx_exit(0);
    }else{
    	delay(500);
		printf("Mutex lock succeeded.\n");
	}

	// assigning read privilage
	read_privilege_list[read_counter].user_id = user_id;
	read_privilege_list[read_counter].user_qid = stream_no;
	read_counter++;
	delay(500);
	printf("read counter is now: %d.\n", read_counter);


	//mutex unlock
	mutex_Var = _mutex_unlock(&read_mutex);
	if(mutex_Var == MQX_OK)
		printf("Mutex unlock succeeded.\n");

	return true;
}


bool _getline(char* string){

	_task_id user_id = _task_get_id();
	_queue_id user_qid;
	MSG_PTR msg_ptr = NULL;

	bool has_read_privilege = false;

	/*find if the task has read privilege and get the qid*/
	// check read privilege of task
    for (int i=0; i < read_counter; i++) {
        if (read_privilege_list[i].user_id == user_id){
        	has_read_privilege = true;
        	user_qid = read_privilege_list[i].user_qid;
        	break;
        }
    }

	// return true only if task was assigned read privilege
    if(!has_read_privilege)
    	return false;

    int str_counter = 0;

    /*receive message to the user queue until getting a "\n"*/
    while(1){
    	msg_ptr = _msgq_receive(user_qid, 0);

    	if(msg_ptr != NULL){

    		// store received message to task array
    		for(int aa=0;(int)msg_ptr->DATA[aa] != 10; aa++ ){
				*string = msg_ptr->DATA[aa];
				string++;
				str_counter++;
    		}

    		// prints the recieved message on debug screen
    		string = string - str_counter;
			delay(500);
			printf("TASK %d RECEIVED BROADCAST MESSAGE:  ", _task_get_id());

			for(;  *string != 0; string++){
				printf("%c",*string);
			}
			printf("\n");
			// clear pointer
			string = string - str_counter;
			memset(string,0, sizeof(char) * strlen(string));
			_msg_free(msg_ptr);
			return true;
		}
	}
}


_queue_id OpenW(){

	// check if can get write privilege
	_task_id taskID =_task_get_id();


	if(open_for_writing != 0){
		delay(500);
		puts("\nSOMEONE ALREADY HAS WRITE PRIVILEGE!!!\n");
		return 0;
	}


	// assigning write privilage
	write_privilege_element.user_id = taskID;
	write_privilege_element.user_qid = PUTLINE_QUEUE;
	delay(500);
	printf("ASSIGNING WRITE PRIVILEGE FOR TASK %d\n", write_privilege_element.user_id);

	open_for_writing = taskID;


	return write_privilege_element.user_qid;

}



/*check whether the user is write_privilege_list, send data from user queue to handler queue*/
bool _putline(_queue_id qid, char* string){

	if( write_privilege_element.user_id != _task_get_id()){
		return false;
	}

	strcat(string, "\n");
	int str_length = strlen(string);

	MSG_PTR msg_ptr;

	//allocate a msg from pool
	msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);

	//Send the data to handler queue
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char);
	msg_ptr->HEADER.TARGET_QID = putline_qid;

	// clean data before send
	memset(msg_ptr->DATA,0, sizeof(char) * 200);

	// send the data to the handler via message passing
	int i = 0;
	for(; i<str_length; i++){
		msg_ptr->DATA[i] = *string;
		string++;
	}


	_msgq_send(msg_ptr);

	return true;

}

/*revoke both read and write privilege;*/
bool Close(void){
	/*get user task id*/
	_task_id user_id = _task_get_id();
	int i = 0;
	int c = 0;
	bool has_read_privilege = false;

	/*find if the task has read privilege and get the qid*/
	// check read privilege of task
    for (; i < read_counter; i++) {
        if (read_privilege_list[i].user_id == user_id){
        	has_read_privilege = true;
        	user_id = read_privilege_list[i].user_id;
        	break;
        }
    }

	// return true only if task was assigned read privilege
    if(has_read_privilege){
		/*delete the element in read_privilege_list*/
		for(c = i; c < read_counter; c++){
			read_privilege_list[c] = read_privilege_list[c+1];
		}
		//read_privilege_list[c] = NULL;
		read_counter--;
    }

	if(user_id == write_privilege_element.user_id ){
		delay(500);
		printf("REVOKING WRITE PRIVILEGE FOR TASK %d\n",_task_get_id());
		write_privilege_element.user_id = NULL;
		write_privilege_element.user_qid = NULL;
		open_for_writing = 0;
	}


	delay(500);
	printf("TASK %d IS CLOSED\n",_task_get_id());
	delay(500);
	printf("read counter is now ------> %d\n", read_counter);
	return 0;
}



//**************************************************************************************************************
//**************************************************************************************************************
void counterReset(){
	if (counter < 0)
		counter = 0;
}

void enterKeyPressed(){

	  output_buf[counter] = 0xa; // new line char
	  UART_DRV_SendDataBlocking(myUART_IDX, &"\n", sizeof(char),500);
	  UART_DRV_SendDataBlocking(myUART_IDX, &Cr, sizeof(char),500);

	  counter++;



}

void DeleteChar(){
	counterReset();


		if( counter >= 0){
		counter--; // point to previous char
		memset(&output_buf[counter],0, sizeof(char));

		UART_DRV_SendDataBlocking(myUART_IDX, "\b \b", sizeof("\b \b"), 1000);
	}


}

void eraseWord(){

	  counterReset();
		  while(output_buf[counter-1] == 0x20){ // space
			  DeleteChar();
		  }
		  // if printable except for spaces
		  while(counter >=0 && output_buf[counter-1] >33 && output_buf[counter-1] < 127){
			  counter--;
//			  printf("points to %c about to delete\n", output_buf[counter]);
			  memset(&output_buf[counter],0, sizeof(char));
			  UART_DRV_SendDataBlocking(myUART_IDX, "\b \b", sizeof("\b \b"), 1000);

		  }




}

void eraseLine(){


	while(output_buf[counter-1] != 0){
	// if printable with space
	if(counter >=0 && output_buf[counter-1] >31 && output_buf[counter-1] < 127){
	  UART_DRV_SendDataBlocking(myUART_IDX, "\b \b", sizeof("\b \b"), 1000);
	  counter--;
//	  printf("line to %c about to delete\n", output_buf[counter]);
	  memset(&output_buf[counter],0, sizeof(char));

	}
	else {
		counterReset();
		break;
	}

	}
}


void delay(int val){
	for ( int c = 1 ; c <= val ; c++ )
	       for (int d = 1 ; d <= val ; d++ )
	       {}
}


void handlerInit(void){

	//create handler queue for isr
	handler_qid = _msgq_open(HANDLER_QUEUE,0);

	// mem
	  memset(putline_buf,0, 200);

	// check if message queue was opened
	if (handler_qid == 0) {
			printf("\nCould not open the handler isr queue\n");
			_task_block();
		}

	//create putline queue
	putline_qid = _msgq_open(PUTLINE_QUEUE,0);

	if (putline_qid == 0) {
		printf("\nCould not open the putline queue\n");
		_task_block();
	}

	// create message pool
	msg_pool = _msgpool_create(sizeof(MESSAGE),NUM_USER_TASKS, 0, 0);
	 if (msg_pool == MSGPOOL_NULL_POOL_ID) {
		 printf("\nCount not create a message pool\n");
		 _task_block();
	  }

	/* Initialize mutex attributes*/
	 if (_mutatr_init(&mutexattr) != MQX_OK) {
		printf("Initializing mutex attributes failed.\n");
		_mqx_exit(0);
	 }

//	 if (_mutatr_init(&W_mutexattr) != MQX_OK) {
//		printf("Initializing mutex attributes failed.\n");
//		_mqx_exit(0);
//	 }

		 mutex_Var = _mutex_init(&read_mutex, &mutexattr);
		 if(mutex_Var != MQX_EOK){
			_mqx_exit(0);
		}else
			printf("Mutex init succeeded.\n");

//		 w_mutex_Var = _mutex_init(&write_mutex, &W_mutexattr);
//		 if(w_mutex_Var != MQX_EOK){
//			_mqx_exit(0);
//		}else
//			printf("Mutex init succeeded.\n");
}


