#ifndef _GLIB_TYPES_H_
#define _GLIB_TYPES_H_

#include <glib_defines.h>
//--------------------------------------------
// you need this struct for the getrgb16 proc
//--------------------------------------------

/*
windows only:
typedef struct  {
    RGBQUAD depth;
    RGBQUAD Amount;
    RGBQUAD Position;
} RGB16;
*/


typedef struct  {
	float top;
	float bottom;
	float left;
	float right;
} FRECT;

typedef struct  {
	float x;
	float y;
} FPOINT;


/*
in types.h:
typedef struct tagCPUINFO  {
	DWORD CpuID;
	char ProcType, Family, Model, Stepping, MMX;
	char CpuName[13];
} CPUINFO, *LPCPUINFO;
*/



//----------------------------------
// machine information structures:
//----------------------------------

typedef enum {
	MCP_UNKNOWN = 0,
	MCP_386 = 1,
	MCP_486 = 2,
	MCP_PENTIUM = 3
} MC_PROCESSOR;

typedef enum {
	MCB_UNKNOWN = 0,
	MCB_ISA = 1,
	MCB_EISA = 2,
	MCB_PCI = 3
} MC_BUSTYPE;

typedef enum {
	MCV_UNKNOWN = 0,
	MCV_DDRAW = 1
} MC_VIDSYS;

typedef struct _MACHINE_CAPS {
	MC_PROCESSOR processor;
	MC_BUSTYPE bus;
	DWORD sysMemory;
	DWORD vidMemory;
	MC_VIDSYS vidSystem;
} MACHINE_CAPS;

/*
windows only:
typedef struct  {
	int Count;
	HWND hWindow;
} ENUMSTRUCT;
*/


typedef struct {							// unencrypted header
	unsigned char c[23];
} HEADER;

#endif									// #ifndef _GENTYPES_H_


