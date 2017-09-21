#ifndef _ANTDEFS_H_
#define _ANTDEFS_H_


//#define DOWORKER
//#define NANTCHANS 8

#define HR_DEV_TYPE	120						// heart rate
#define SC_DEV_TYPE	121						// speed cadence
#define C_DEV_TYPE	122						// cadence

#ifndef WIN32
#define UCHAR unsigned char
#define USHORT unsigned short
#define ULONG unsigned long
#endif


#define MSTONS				(1000000.0)
#define NSTOMS				(1.0/MSTONS)




#define SYNC_BYTE					0xA4
#define MAX_LENGTH				14		// was 9 but the version message is 11 bytes! data message is 14 bytes
#define MAX_MESSAGE_SIZE		32

#define VERSION					0x3E

#define CHANNEL_EVENT			0x40
#define UNASSIGN_CHANNEL		0x41
#define ASSIGN_CHANNEL			0x42
#define CHANNEL_PERIOD			0x43
#define SEARCH_TIMEOUT			0x44
#define CHANNEL_FREQUENCY		0x45
#define SET_NETWORK_KEY			0x46
#define TX_POWER					0x47
#define CW_TEST					0x48
#define SYSTEM_RESET				0x4A
#define OPEN_CHANNEL				0x4B
#define CLOSE_CHANNEL			0x4C
#define REQ_MESSAGE				0x4D
#define BROADCAST_DATA			0x4E
#define ACK_DATA					0x4F

#define BURST_DATA				0x50
#define CHANNEL_ID				0x51
#define CHANNEL_STATUS			0x52
#define CW_INIT					0x53
#define CAPABILITIES				0x54
#define ID_LIST_ADD				0x59
#define ID_LIST_CONFIG			0x5A
#define OPEN_RX_SCAN_CH			0x5B

#define CHANNEL_TX_POWER		0x60
#define SERIAL_NUMBER			0x61
#define LP_SEARCH_TIMEOUT		0x63
#define SET_SERIAL_NUMBER		0x65
#define ENABLE_EXT_MSGS			0x66
#define ENABLE_LED				0x68

#define PROXIMITY_SEARCH		0x71

#define MESG_LIB_CONFIG			0x6e
#define MESG_STARTUP_MESG_ID	0x6F
#define MESG_NETWORK_KEY_ID	0x46
#define MESG_INVALID_ID			0x00


//---------------------
// Channel messages
//---------------------

#define RESPONSE_NO_ERROR						0
#define EVENT_RX_SEARCH_TIMEOUT				1
#define EVENT_RX_FAIL							2
#define EVENT_TX									3
#define EVENT_TRANSFER_RX_FAILED				4
#define EVENT_TRANSFER_TX_COMPLETED			5
#define EVENT_TRANSFER_TX_FAILED				6
#define EVENT_CHANNEL_CLOSED					7
#define EVENT_RX_FAIL_GO_TO_SEARCH			8
#define EVENT_CHANNEL_COLLISION				9
#define EVENT_RX_BROADCAST						10
#define EVENT_RX_ACKNOWLEDGED					11
#define EVENT_RX_BURST_PACKET					12
#define CHANNEL_IN_WRONG_STATE				21
#define CHANNEL_NOT_OPENED						22
#define CHANNEL_ID_NOT_SET						24
#define TRANSFER_IN_PROGRESS					31
#define TRANSFER_SEQUENCE_NUMBER_ERROR		32
#define INVALID_MESSAGE							40
#define INVALID_NETWORK_NUMBER				41
#define EVENT_SERIAL_QUE_OVERFLOW			0x34				// 52
#define EVENT_QUE_OVERFLOW						0x35				// 53


///////////////////////////////////////////////////////
// Reset Mesg Codes
///////////////////////////////////////////////////////

#define RESET_FLAGS_MASK								((UCHAR)0xE0)
#define RESET_SUSPEND									((UCHAR)0x80)					// this must follow bitfield def
#define RESET_SYNC										((UCHAR)0x40)					// this must follow bitfield def
#define RESET_CMD											((UCHAR)0x20)					// this must follow bitfield def
#define RESET_WDT											((UCHAR)0x02)
#define RESET_RST											((UCHAR)0x01)
#define RESET_POR											((UCHAR)0x00)


//////////////////////////////////////////////
// Standard capabilities defines
//////////////////////////////////////////////

#define CAPABILITIES_NO_RX_CHANNELS					((UCHAR)0x01)
#define CAPABILITIES_NO_TX_CHANNELS					((UCHAR)0x02)
#define CAPABILITIES_NO_RX_MESSAGES					((UCHAR)0x04)
#define CAPABILITIES_NO_TX_MESSAGES					((UCHAR)0x08)
#define CAPABILITIES_NO_ACKD_MESSAGES				((UCHAR)0x10)
#define CAPABILITIES_NO_BURST_TRANSFER				((UCHAR)0x20)

//////////////////////////////////////////////
// Advanced capabilities defines
//////////////////////////////////////////////

#define CAPABILITIES_OVERUN_UNDERRUN					((UCHAR)0x01)		// Support for this functionality has been dropped
#define CAPABILITIES_NETWORK_ENABLED					((UCHAR)0x02)
#define CAPABILITIES_AP1_VERSION_2						((UCHAR)0x04)		// This Version of the AP1 does not support transmit and only had a limited release
#define CAPABILITIES_SERIAL_NUMBER_ENABLED			((UCHAR)0x08)
#define CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED	((UCHAR)0x10)
#define CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED	((UCHAR)0x20)
#define CAPABILITIES_SCRIPT_ENABLED						((UCHAR)0x40)
#define CAPABILITIES_SEARCH_LIST_ENABLED				((UCHAR)0x80)

//////////////////////////////////////////////
// Advanced capabilities 2 defines
//////////////////////////////////////////////

#define CAPABILITIES_LED_ENABLED							((UCHAR)0x01)
#define CAPABILITIES_EXT_MESSAGE_ENABLED				((UCHAR)0x02)
#define CAPABILITIES_SCAN_MODE_ENABLED					((UCHAR)0x04)
#define CAPABILITIES_RESERVED								((UCHAR)0x08)
#define CAPABILITIES_PROX_SEARCH_ENABLED				((UCHAR)0x10)
#define CAPABILITIES_EXT_ASSIGN_ENABLED				((UCHAR)0x20)
#define CAPABILITIES_FS_ANTFS_ENABLED					((UCHAR)0x40)
#define CAPABILITIES_FIT1_ENABLED						((UCHAR)0x80)

//////////////////////////////////////////////
// Advanced capabilities 3 defines
//////////////////////////////////////////////

#define CAPABILITIES_SENSRCORE_ENABLED					((UCHAR)0x01)
#define CAPABILITIES_RESERVED_1							((UCHAR)0x02)
#define CAPABILITIES_RESERVED_2							((UCHAR)0x04)
#define CAPABILITIES_RESERVED_3							((UCHAR)0x08)

#define MAXSENSORS 32

typedef struct {
	unsigned short sn = 0xffff;										// copy of the key for easier iteration
	unsigned char assigned_channel = 0xff;
	unsigned char val=0;
	unsigned char type = 0x00;
	unsigned char transtype = 0x00;
} SENSOR;


typedef struct  {										// size = 6*32 + 1 = 194 (rounded up)
	unsigned char n=0;								// number of sensors
	SENSOR sensors[MAXSENSORS];					// up to 32 sensors
} SENSORS;												// see also antdef in ant.h

#endif

