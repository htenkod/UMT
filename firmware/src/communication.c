#include "communication.h"



void *COMM_PKT_Alloc(size_t len)
{
	return OSAL_Malloc(sizeof(COMM_HDR_T)+len);

}

void COMM_PKT_Free(void *pkt)
{
	OSAL_Free(pkt);	

}


void COMM_ADD_Packet(COMM_PKT_T *pkt)
{


}

COMM_PKT_T COMM_REM_Packet()
{


}

void COMM_PKT_Process(void)
{

}


void COMM_Task(void)
{

	COMM_PKT_Process();

}




