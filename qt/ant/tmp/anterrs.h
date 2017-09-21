
#ifndef _ANTERRS_H_
#define _ANTERRS_H_

		enum RMANT_ERROR {
			RMANT_SUCCESS = LIBUSB_SUCCESS,											// 0
			RMANT_ERROR_IO = LIBUSB_ERROR_IO,										// -1		Input/output error
			RMANT_ERROR_INVALID_PARAM = LIBUSB_ERROR_INVALID_PARAM,			// -2		Invalid parameter
			RMANT_ERROR_ACCESS = LIBUSB_ERROR_ACCESS,								// -3		Access denied (insufficient permissions)
			RMANT_ERROR_NO_DEVICE = LIBUSB_ERROR_NO_DEVICE,						// -4		No such device (it may have been disconnected)
			RMANT_ERROR_NOT_FOUND = LIBUSB_ERROR_NOT_FOUND,						// -5		Entity not found
			RMANT_ERROR_BUSY = LIBUSB_ERROR_BUSY,									// -6		Resource busy
			RMANT_ERROR_TIMEOUT = LIBUSB_ERROR_TIMEOUT,							// -7		Operation timed out
			RMANT_ERROR_OVERFLOW = LIBUSB_ERROR_OVERFLOW,						// -8		Overflow
			RMANT_ERROR_PIPE = LIBUSB_ERROR_PIPE,									// -9		Pipe error
			RMANT_ERROR_INTERRUPTED = LIBUSB_ERROR_INTERRUPTED,				// -10	System call interrupted (perhaps due to signal) */
			RMANT_ERROR_NO_MEM = LIBUSB_ERROR_NO_MEM,								// -11	Insufficient memory
			RMANT_ERROR_NOT_SUPPORTED = LIBUSB_ERROR_NOT_SUPPORTED,			// -12	Operation not supported or unimplemented on this platform

			RMANT_ERROR_NO_CTX = 1,
			RMANT_ERROR_NO_USB_DEVICES,
			RMANT_ERROR_GET_DEVICE_DESCRIPTOR,
			RMANT_ERROR_DETACHING,
			RMANT_ERROR_NO_DEVICES,

			RMANT_ERROR_OTHER
		};

#endif


