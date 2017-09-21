#include "antdefines.h"
#include "antmessage.h"
#include "global.h"
#include "types.h"
#include "ant_dll.h"

#define ANT_CHANNEL ((UCHAR) 7)  // My virtual COM port is 7

static UCHAR GARMIN_KEY[] = "...";

static UCHAR aucTxBuf[17];         ///< Buffer to hold messages that are sent to the ANT device
static UCHAR aucRxBuf[17];         ///< Buffer to hold channel specific RF messages and events received from the ANT device
static UCHAR aucResponseBuf[10];   ///< Buffer to hold the response to configuration commands

// ANT message handlers

UCHAR ProcessChannelEvent(UCHAR ucChannel, UCHAR ucMessageCode)
{
	switch(ucMessageCode)
	{
		case EVENT_RX_ACKNOWLEDGED:
			break;
		case EVENT_RX_BROADCAST:    // A Broadcast message has been received sucessfully
			break;
		case EVENT_TX:
			break;
		case EVENT_TRANSFER_TX_COMPLETED:   // A Acknowledged packet was sucessfully sent
			break;
		case EVENT_RX_FAIL_GO_TO_SEARCH:    // If we ever go back to search we fail the test
			break;
		case EVENT_RX_SEARCH_TIMEOUT:       // If we timeout on the search we faile the test
			break;
		case EVENT_RX_FAIL:                 // We failed to receive a message at the designated message period
			break;
		case EVENT_TRANSFER_TX_FAILED:      // We failed to get a reply for the acknowledged packet we sent
			break;
		case EVENT_CHANNEL_CLOSED:
			break;
	}

	return 1;
}

UCHAR ProcessProtocolEvent(UCHAR ucChannel, UCHAR ucMessageCode)
{
	switch(ucMessageCode)
	{
		case MESG_CAPABILITIES_ID:

			break;
		case MESG_RESPONSE_EVENT_ID:
			{   
				switch (aucResponseBuf[1])
				{
					case MESG_ASSIGN_CHANNEL_ID:
						break;
					case MESG_UNASSIGN_CHANNEL_ID:
						break;
					case MESG_CHANNEL_ID_ID:
						break;
					case MESG_CHANNEL_MESG_PERIOD_ID:
						break;
					case MESG_CHANNEL_RADIO_FREQ_ID:
						break;
					case MESG_OPEN_CHANNEL_ID:
						break;
					case MESG_CLOSE_CHANNEL_ID:
						break;
					case MESG_CHANNEL_SEARCH_TIMEOUT_ID:
					default:
						break;
				}
				break;
			}   
	}

	return 1;
}

BOOL ANT::OnInitDialog()
{
	CDialog::OnInitDialog();

	ANT_Load();
	ANT_ResetSystem();
	Sleep(20);

	ANT_Init(ANT_CHANNEL, 4800);

	ANT_AssignResponseFunction( (RESPONSE_FUNC)ProcessProtocolEvent, aucResponseBuf );
	ANT_AssignChannelEventFunction( ANT_CHANNEL, (CHANNEL_EVENT_FUNC)ProcessChannelEvent, aucRxBuf );

	// Trying to get a message
	ANT_RequestMessage(ANT_CHANNEL, MESG_CHANNEL_ID_ID);

	return TRUE;
}

void ANT::OnBnClickedOpen()
{
	// Rx channel
	ANT_AssignChannel(ANT_CHANNEL, 0x00, 0);   
	Sleep(20);

	// Using the Garmin parameters
	ANT_SetChannelId(ANT_CHANNEL, 0, 0, 0);
	Sleep(20);
	ANT_SetNetworkKey(ANT_CHANNEL, GARMIN_KEY);
	Sleep(20);
	ANT_SetChannelSearchTimeout(ANT_CHANNEL, 255);
	Sleep(20);
	ANT_SetChannelPeriod(ANT_CHANNEL, 0x1f86);
	Sleep(20);
	ANT_SetChannelRFFreq(ANT_CHANNEL, 0x39);
	Sleep(20);

	ANT_OpenChannel(ANT_CHANNEL);

	Sleep(1000);

	// Again trying to get a message
	ANT_RequestMessage(0,MESG_CAPABILITIES_ID);   
}

void ANT::OnClose()
{
	ANT_CloseChannel(ANT_CHANNEL);
	ANT_Close();

	CDialog::OnClose();
}


void ANT::OnCancel()
{
	ANT_CloseChannel(ANT_CHANNEL);
	ANT_Close();

	CDialog::OnCancel();
}


