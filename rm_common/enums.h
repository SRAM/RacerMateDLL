#ifndef _ENUMS_H_
#define _ENUMS_H_

enum TRAINER_COMMUNICATION_TYPE {
	BAD_INPUT_TYPE = -1,
	WIN_RS232,
	UNIX_RS232,
	//TRAINER_IS_SERVER,									// trainer is a server
	TRAINER_IS_CLIENT									// trainer is a client
};

#endif


