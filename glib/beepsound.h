
#ifndef _BEEPSOUND_H_
#define _BEEPSOUND_H_

#if 0
static unsigned char beepsound[] = {
		0x52, 0x49, 0x46, 0x46, 0xa2, 0x03, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
		0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x11, 0x2b, 0x00, 0x00,
		0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x3a, 0x03, 0x00, 0x00, 0x80, 0xab, 0xc9, 0xcf,
		0xbd, 0x97, 0x6a, 0x44, 0x31, 0x37, 0x54, 0x7f, 0xaa, 0xc8, 0xcf, 0xbd, 0x98, 0x6b, 0x45, 0x31,
		0x36, 0x53, 0x7e, 0xa9, 0xc8, 0xd0, 0xbe, 0x99, 0x6c, 0x46, 0x31, 0x36, 0x52, 0x7d, 0xa8, 0xc7,
		0xd0, 0xbf, 0x9a, 0x6d, 0x46, 0x32, 0x36, 0x51, 0x7b, 0xa7, 0xc7, 0xd0, 0xc0, 0x9b, 0x6e, 0x47,
		0x32, 0x35, 0x50, 0x7a, 0xa6, 0xc6, 0xd0, 0xc0, 0x9c, 0x70, 0x48, 0x32, 0x35, 0x4f, 0x79, 0xa5,
		0xc6, 0xd0, 0xc1, 0x9d, 0x71, 0x49, 0x32, 0x34, 0x4e, 0x78, 0xa4, 0xc5, 0xd0, 0xc2, 0x9f, 0x72,
		0x4a, 0x33, 0x34, 0x4d, 0x77, 0xa3, 0xc4, 0xd0, 0xc2, 0xa0, 0x73, 0x4a, 0x33, 0x34, 0x4d, 0x76,
		0xa2, 0xc4, 0xd0, 0xc3, 0xa1, 0x74, 0x4b, 0x33, 0x33, 0x4c, 0x75, 0xa1, 0xc3, 0xd0, 0xc4, 0xa2,
		0x75, 0x4c, 0x33, 0x33, 0x4b, 0x74, 0xa0, 0xc3, 0xd0, 0xc4, 0xa3, 0x76, 0x4d, 0x34, 0x33, 0x4a,
		0x72, 0x9f, 0xc2, 0xd0, 0xc5, 0xa4, 0x77, 0x4e, 0x34, 0x32, 0x49, 0x71, 0x9e, 0xc1, 0xd0, 0xc5,
		0xa5, 0x79, 0x4f, 0x35, 0x32, 0x48, 0x70, 0x9d, 0xc1, 0xd0, 0xc6, 0xa6, 0x7a, 0x50, 0x35, 0x32,
		0x48, 0x6f, 0x9c, 0xc0, 0xd0, 0xc6, 0xa7, 0x7b, 0x51, 0x35, 0x32, 0x47, 0x6e, 0x9b, 0xbf, 0xd0,
		0xc7, 0xa8, 0x7c, 0x52, 0x36, 0x31, 0x46, 0x6d, 0x9a, 0xbf, 0xd0, 0xc7, 0xa9, 0x7d, 0x52, 0x36,
		0x31, 0x45, 0x6c, 0x99, 0xbe, 0xcf, 0xc8, 0xaa, 0x7e, 0x53, 0x37, 0x31, 0x44, 0x6b, 0x98, 0xbd,
		0xcf, 0xc8, 0xab, 0x7f, 0x54, 0x37, 0x31, 0x44, 0x6a, 0x96, 0xbc, 0xcf, 0xc9, 0xac, 0x81, 0x55,
		0x38, 0x31, 0x43, 0x68, 0x95, 0xbc, 0xcf, 0xc9, 0xad, 0x82, 0x56, 0x38, 0x31, 0x42, 0x67, 0x94,
		0xbb, 0xcf, 0xca, 0xae, 0x83, 0x57, 0x39, 0x30, 0x41, 0x66, 0x93, 0xba, 0xcf, 0xca, 0xae, 0x84,
		0x58, 0x39, 0x30, 0x41, 0x65, 0x92, 0xb9, 0xce, 0xcb, 0xaf, 0x85, 0x59, 0x3a, 0x30, 0x40, 0x64,
		0x91, 0xb8, 0xce, 0xcb, 0xb0, 0x86, 0x5a, 0x3a, 0x30, 0x3f, 0x63, 0x90, 0xb8, 0xce, 0xcb, 0xb1,
		0x87, 0x5b, 0x3b, 0x30, 0x3f, 0x62, 0x8f, 0xb7, 0xce, 0xcc, 0xb2, 0x89, 0x5c, 0x3b, 0x30, 0x3e,
		0x61, 0x8e, 0xb6, 0xcd, 0xcc, 0xb3, 0x8a, 0x5d, 0x3c, 0x30, 0x3d, 0x60, 0x8c, 0xb5, 0xcd, 0xcd,
		0xb4, 0x8b, 0x5e, 0x3c, 0x30, 0x3d, 0x5f, 0x8b, 0xb4, 0xcd, 0xcd, 0xb5, 0x8c, 0x5f, 0x3d, 0x30,
		0x3c, 0x5e, 0x8a, 0xb3, 0xcc, 0xcd, 0xb6, 0x8d, 0x60, 0x3e, 0x30, 0x3c, 0x5d, 0x89, 0xb3, 0xcc,
		0xcd, 0xb6, 0x8e, 0x61, 0x3e, 0x30, 0x3b, 0x5c, 0x88, 0xb2, 0xcc, 0xce, 0xb7, 0x8f, 0x63, 0x3f,
		0x30, 0x3a, 0x5b, 0x87, 0xb1, 0xcb, 0xce, 0xb8, 0x90, 0x64, 0x40, 0x30, 0x3a, 0x5a, 0x86, 0xb0,
		0xcb, 0xce, 0xb9, 0x92, 0x65, 0x40, 0x30, 0x39, 0x59, 0x85, 0xaf, 0xca, 0xce, 0xba, 0x93, 0x66,
		0x41, 0x30, 0x39, 0x58, 0x83, 0xae, 0xca, 0xcf, 0xba, 0x94, 0x67, 0x42, 0x30, 0x38, 0x57, 0x82,
		0xad, 0xca, 0xcf, 0xbb, 0x95, 0x68, 0x43, 0x31, 0x38, 0x56, 0x81, 0xac, 0xc9, 0xcf, 0xbc, 0x96,
		0x69, 0x43, 0x31, 0x37, 0x55, 0x80, 0xab, 0xc9, 0xcf, 0xbd, 0x97, 0x6a, 0x44, 0x31, 0x37, 0x54,
		0x7f, 0xaa, 0xc8, 0xcf, 0xbd, 0x98, 0x6b, 0x45, 0x31, 0x36, 0x53, 0x7e, 0xa9, 0xc8, 0xd0, 0xbe,
		0x99, 0x6c, 0x46, 0x31, 0x36, 0x52, 0x7d, 0xa8, 0xc7, 0xd0, 0xbf, 0x9a, 0x6d, 0x46, 0x32, 0x36,
		0x51, 0x7b, 0xa7, 0xc7, 0xd0, 0xc0, 0x9b, 0x6e, 0x47, 0x32, 0x35, 0x50, 0x7a, 0xa6, 0xc6, 0xd0,
		0xc0, 0x9c, 0x70, 0x48, 0x32, 0x35, 0x4f, 0x79, 0xa5, 0xc6, 0xd0, 0xc1, 0x9d, 0x71, 0x49, 0x32,
		0x34, 0x4e, 0x78, 0xa4, 0xc5, 0xd0, 0xc2, 0x9f, 0x72, 0x4a, 0x33, 0x34, 0x4d, 0x77, 0xa3, 0xc4,
		0xd0, 0xc2, 0xa0, 0x73, 0x4a, 0x33, 0x34, 0x4d, 0x76, 0xa2, 0xc4, 0xd0, 0xc3, 0xa1, 0x74, 0x4b,
		0x33, 0x33, 0x4c, 0x75, 0xa1, 0xc3, 0xd0, 0xc4, 0xa2, 0x75, 0x4c, 0x33, 0x33, 0x4b, 0x74, 0xa0,
		0xc3, 0xd0, 0xc4, 0xa3, 0x76, 0x4d, 0x34, 0x33, 0x4a, 0x72, 0x9f, 0xc2, 0xd0, 0xc5, 0xa4, 0x77,
		0x4e, 0x34, 0x32, 0x49, 0x71, 0x9e, 0xc1, 0xd0, 0xc5, 0xa5, 0x79, 0x4f, 0x35, 0x32, 0x48, 0x70,
		0x9d, 0xc1, 0xd0, 0xc6, 0xa6, 0x7a, 0x50, 0x35, 0x32, 0x48, 0x6f, 0x9c, 0xc0, 0xd0, 0xc6, 0xa7,
		0x7b, 0x51, 0x35, 0x32, 0x47, 0x6e, 0x9b, 0xbf, 0xd0, 0xc7, 0xa8, 0x7c, 0x52, 0x36, 0x31, 0x46,
		0x6d, 0x9a, 0xbf, 0xd0, 0xc7, 0xa9, 0x7d, 0x52, 0x36, 0x31, 0x45, 0x6c, 0x99, 0xbe, 0xcf, 0xc8,
		0xaa, 0x7e, 0x53, 0x37, 0x31, 0x44, 0x6b, 0x98, 0xbd, 0xcf, 0xc8, 0xab, 0x7f, 0x54, 0x37, 0x31,
		0x44, 0x6a, 0x96, 0xbc, 0xcf, 0xc9, 0xac, 0x81, 0x55, 0x38, 0x31, 0x43, 0x68, 0x95, 0xbc, 0xcf,
		0xc9, 0xad, 0x82, 0x56, 0x38, 0x31, 0x42, 0x67, 0x94, 0xbb, 0xcf, 0xca, 0xae, 0x83, 0x57, 0x39,
		0x30, 0x41, 0x66, 0x93, 0xba, 0xcf, 0xca, 0xae, 0x84, 0x58, 0x39, 0x30, 0x41, 0x65, 0x92, 0xb9,
		0xce, 0xcb, 0xaf, 0x85, 0x59, 0x3a, 0x30, 0x40, 0x64, 0x91, 0xb8, 0xce, 0xcb, 0xb0, 0x86, 0x5a,
		0x3a, 0x30, 0x3f, 0x63, 0x90, 0xb8, 0xce, 0xcb, 0xb1, 0x87, 0x5b, 0x3b, 0x30, 0x3f, 0x62, 0x8f,
		0xb7, 0xce, 0xcc, 0xb2, 0x89, 0x5c, 0x3b, 0x30, 0x3e, 0x61, 0x8e, 0xb6, 0xcd, 0xcc, 0xb3, 0x8a,
		0x5d, 0x3c, 0x30, 0x3d, 0x60, 0x8c, 0xb5, 0xcd, 0xcd, 0xb4, 0x8b, 0x5e, 0x3c, 0x30, 0x3d, 0x5f,
		0x8b, 0xb4, 0xcd, 0xcd, 0xb5, 0x8c, 0x5f, 0x3d, 0x30, 0x3c, 0x5e, 0x8a, 0xb3, 0xcc, 0xcd, 0xb6,
		0x8d, 0x60, 0x3e, 0x30, 0x3c, 0x5d, 0x89, 0xb3, 0xcc, 0xcd, 0xb6, 0x8e, 0x61, 0x3e, 0x30, 0x3b,
		0x5c, 0x88, 0xb2, 0xcc, 0xce, 0xb7, 0x8f, 0x63, 0x3f, 0x30, 0x3a, 0x5b, 0x87, 0xb1, 0xcb, 0xce,
		0xb8, 0x90, 0x64, 0x40, 0x30, 0x3a, 0x73, 0x6d, 0x70, 0x6c, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x62, 0x01, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xff, 0x07, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
#endif
#endif

