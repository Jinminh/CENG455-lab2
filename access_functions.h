#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "global_variables.h"

bool openR(uint16_t stream_no);
bool _getline(char* string);
_queue_id OpenW(void);
bool _putline(_queue_id qid, char* string);
bool Close(void);


