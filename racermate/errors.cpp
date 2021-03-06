
#include <errors.h>

const char *err_strs[NERRORS] = {
	"OK",
	"DEVICE NOT RUNNING",
	"WRONG DEVICE",
	"DIRECTORY DOES NOT EXIST",
	"DEVICE ALREADY RUNNING",
	"BAD FIRMWARE VERSION",
	"VELOTRON PARAMETERS NOT SET",
	"BAD GEAR COUNT",
	"BAD TEETH COUNT",
	"PORT DOES NOT EXIST",
	"PORT OPEN ERROR",
	"PORT EXISTS BUT IS NOT A TRAINER",
	"DEVICE RUNNING",
	"BELOW UPPER SPEED",
	"ABORTED",
	"TIMEOUT",
	"BAD RIDER INDEX",
	"DEVICE_NOT_INITIALIZED",
	"CAN_NOT_OPEN_FILE",
	"CAN_NOT_CREATE_SERVER",
	"GENERIC ERROR"
};

