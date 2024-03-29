#include "uthash.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct VirtualPointer {
	int val;
} VirtualPointer;

/*
 * ShieldObejct is a struct that holds a virtual pointer (vptr) and the real 
 * machine-specific pointer (ptr). The corrent way to interface with this 
 * object is by querying the VirtualMemory class with vptr. The _shield_malloc 
 * function will return a vptr as a VirtualPointer.
 */
typedef struct ShieldObject {
	VirtualPointer vptr;        // key 
	int* ptr;		    // machine-specific pointer
	UT_hash_handle hh;
} ShieldObject;


typedef struct VirtualMemory {
	int count; 
	ShieldObject* vMap;  // VirtualPointer -> ShieldObject
} VirtualMemory;


VirtualPointer VirtualMemory_set(VirtualMemory* vMem, int* p) {
	// Add key, value to hash table
	int newId = vMem->count + 1;
	VirtualPointer vp = { .val = newId };
	
	// Initialize shield object
	// Need to malloc for this object, otherwise you are just refering to the stack ptr.
	// ShieldObject obj = { .vptr = vp, .ptr = p };
	ShieldObject *obj = (ShieldObject *)malloc(sizeof(ShieldObject));
	obj->vptr = vp;
	obj->ptr = p;

	HASH_ADD(hh, vMem->vMap, vptr, sizeof(VirtualPointer), obj);
	return vp;
}


ShieldObject* VirtualMemory_get(VirtualMemory* vMem, VirtualPointer vp) {
	// Get ShieldObject from hash
	//ShieldObject* objPtr = (ShieldObject*) malloc(sizeof(ShieldObject)); 
	ShieldObject* objPtr;
	HASH_FIND(hh, vMem->vMap, &vp, sizeof(VirtualPointer), objPtr);

	return objPtr;
}

int VirtualMemory_del(VirtualMemory* vMem, ShieldObject* vptr) {
	return 0;
}



/*
 * TODO: determine the vptr return type.
 */
int* __shield_malloc(size_t s) {
	int* ptr;
		
	return ptr;	
}

int* __shield_memcpy() {
       return NULL;
}       

/*
 * Declare virtual memory, set to NULL as required by uthash.
 */
VirtualMemory vMem = { .count = 0, .vMap = NULL };

int main() {
	
	printf("This is the test for _carmor_malloc.\n");

	// create int and store in vMap
	int* i = (int*)malloc(sizeof(int));
	*i = 10;
	printf("%i\n", *i);

	printf("Setting entry in hash table...\n");
	VirtualPointer vptr = VirtualMemory_set(&vMem, i);

	printf("vptr: %i\n", vptr.val);

	printf("Getting entry...\n");
	ShieldObject *sObj = VirtualMemory_get(&vMem, vptr);
	
	printf("Value returned: %i\n", *(sObj->ptr));

	printf("Modifying object...\n");
	*(sObj->ptr) = 250;

	printf("Output: %i\n", *(VirtualMemory_get(&vMem, vptr)->ptr));


	return 0;
}
