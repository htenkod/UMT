

#ifndef COMMUNICATION_H
#define COMMUNICATION_H


/* Start of Frame */
#define SOF	0xA0A5


typedef enum {
	HIGH,
	MEDIUM,
	LOW,
}PRIORITY_T;


typedef enum {
	GPIO_SET_DIR,
	GPIO_GET_DIR,
	GPIO_SET_VAL,
	GPIO_GET_VAL,

	UART_SET_CFG,
	UART_GET_CFG,

	UART_WRITE,
	UART_READ,


}COMMANDS_T;


/* Interface Context */
typedef struct {
	COMM_LINK_T *commLink;
	uint32_t 	enableCrc;


}COMM_BUS_CTX;



typedef struct {
	COMM_NODE_T *next;
	uint8_t buffer[];
}COMM_NODE_T;


/* Communication packet link */
typedef struct {
	/* list head for push operation */
	COMM_NODE_T *head;
	/* list tail for pop operation */
	COMM_NODE_T *tail;
	/* numbder of nodes in link*/
	size_t 	totNodes;

	
}COMM_LINK_T;



/* packet header*/
typedef struct
{
	uint16_t sof;	
	uint16_t len;
	COMMANDS_T cmd;	
	PRIORITY_T priority;	
}COMM_HDR_T;


/* communication packet*/
typedef struct
{
	COMM_HDR_T header;
	uint8_t	   buffer[];
}COMM_PKT_T;









#endif /*COMMUNICATION_H*/

