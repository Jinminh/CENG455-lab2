/*
 * global_variables.h
 *
 *  Created on: Feb 6, 2017
 *      Author: nguyen06
 */
#include <mqx.h>
#include <message.h>
#include <mutex.h>

#ifndef SOURCES_GLOBAL_VARIABLES_H_
#define SOURCES_GLOBAL_VARIABLES_H_

#define NUM_USER_TASKS 4

//Task IDs
#define HANDLER_TASK 5
#define USER_TASK 6

//Queue IDs
#define HANDLER_QUEUE 7
#define OUTPUT_QUEUE 8
#define USER1_QUEUE 9
#define PUTLINE_QUEUE 10
#define USER2_QUEUE 11
#define USER3_QUEUE 12
#define USER4_QUEUE 13

_queue_id handler_qid;
_queue_id putline_qid;

char output_buf[200];
char putline_buf[200];
char getline_buf[200];
int counter;// = 11;
char Cr = 0x0d;
int j;




//message structure
typedef struct{
	MESSAGE_HEADER_STRUCT HEADER;
	unsigned char DATA[200];
}MESSAGE, *MSG_PTR;

//mutex
MUTEX_ATTR_STRUCT mutexattr;
MUTEX_ATTR_STRUCT W_mutexattr;
MUTEX_STRUCT read_mutex;
MUTEX_STRUCT write_mutex;

extern _pool_id msg_pool;

typedef struct{
	_task_id user_id;
	_queue_id user_qid;
}read_privilege, read_privilege;

typedef struct{
	_task_id user_id;
	_queue_id user_qid;
}write_privilege;


MSG_PTR msg_ptr;
MSG_PTR send_msg_ptr;

MSG_PTR putline_msg_ptr;


_mqx_uint mutex_Var;
_mqx_uint w_mutex_Var;
int read_counter = 0;
_task_id open_for_writing = 0;
read_privilege read_privilege_list[10];
write_privilege write_privilege_element;
extern _task_id write_privilege_list[1];


#endif /* SOURCES_GLOBAL_VARIABLES_H_ */
