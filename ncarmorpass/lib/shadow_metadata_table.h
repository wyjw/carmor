#include <stdio.h>
#include <stdlib.h>


#define SM_SIZE			100	// maximum size of the shadow metadata table
#define SM_CHUNKS		5 	// each shadow memory entry is 5 bytes

/*
 * Universal identifier for shared objects across boundaries
 * Also used as index for shadow metadata table
 * Currently also the key for hash table-based ShieldObject storage
 */
typedef struct {
	int 			val;	// virtual pointer ID
} VirtualPointer;


typedef struct {
	VirtualPointer		vptr;	// key
	void			*ptr;	// machine-specific pointer
	// UT_hash_handle		hh;
} ShieldObject;


typedef struct {
	unsigned char		valid;	// validity of the entry, if deleted, set to 0
	unsigned char		mod;	// dirty bit
	unsigned char		sec;	// security level, used to infer location
	unsigned short		size;	// memory bound
	unsigned char		type;	// class type
} ShadowMetadata_Entry;


typedef struct {
	unsigned char		count;
	ShadowMetadata_Entry	table[SM_SIZE];
} ShadowMetadata;


// TODO: use pointerCast table to handle virtual -> real pointer conversion
typedef struct {
	unsigned char		count;
	ShieldObject		table[SM_SIZE];
} PointerCast;


typedef struct {
	int			count;
	ShadowMetadata		sm;
	//ShieldObject		*vMap;  // VirtualPointer -> ShieldObject
	PointerCast		pc;
} VirtualMemory;

VirtualPointer VirtualMemory_set(VirtualMemory *vMem, void *p, unsigned char mod,
		unsigned char sec, unsigned short size, unsigned char type);
ShieldObject VirtualMemory_get(VirtualMemory *vMem, VirtualPointer vp);
