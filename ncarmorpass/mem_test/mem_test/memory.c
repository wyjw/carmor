#include <stdio.h>
#include <stdlib.h>
#include "uthash.h"

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
	void *ptr;		    // machine-specific pointer
	UT_hash_handle hh;
} ShieldObject;


typedef struct VirtualMemory {
	int count; 
	ShieldObject *vMap;  // VirtualPointer -> ShieldObject
} VirtualMemory;

/*
 * Declare virtual memory, set to NULL as required by uthash.
 */
VirtualMemory vMem = { .count = 0, .vMap = NULL };


VirtualPointer VirtualMemory_set(VirtualMemory *vMem, void *p) {
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


ShieldObject* VirtualMemory_get(VirtualMemory *vMem, VirtualPointer vp) {
	// Get ShieldObject from hash
	//ShieldObject* objPtr = (ShieldObject*) malloc(sizeof(ShieldObject)); 
	ShieldObject *objPtr;
	HASH_FIND(hh, vMem->vMap, &vp, sizeof(VirtualPointer), objPtr);

	return objPtr;
}

int VirtualMemory_del(VirtualMemory *vMem, ShieldObject *vptr) {
	return 0;
}



/*
 * TODO: determine the vptr return type.
 */
VirtualPointer __shield_malloc(size_t s) {
	void* ptr = malloc(s);
	return VirtualMemory_set(&vMem, ptr);
}

void* __shield_deref(VirtualPointer vptr) {
	return VirtualMemory_get(&vMem, vptr)->ptr;
}

int* __shield_memcpy() {
        return NULL;
}       

int main() {
	
	printf("This is the test for a naive implementation of the Virtual Coordination Layer.\n");
	
	/*
	// create int and store in vMap
	int *i = (int *)malloc(sizeof(int));
	*i = 10;
	printf("%i\n", *i);

	printf("Setting entry in hash table...\n");
	VirtualPointer vptr = VirtualMemory_set(&vMem, (void *)i);

	printf("vptr: %i\n", vptr.val);

	printf("Getting entry...\n");
	ShieldObject *sObj = VirtualMemory_get(&vMem, vptr);
	
	printf("Value returned: %i\n", *(int *)(sObj->ptr));

	printf("Modifying object...\n");
	*(int *)(sObj->ptr) = 250;

	printf("Output: %i\n", *(int *)(VirtualMemory_get(&vMem, vptr)->ptr));
	*/
	
	VirtualPointer i = __shield_malloc(sizeof(int));
	*((int *)(__shield_deref(i))) = 10;

	printf("Initial value: %i\n", *((int *)__shield_deref(i)));



	return 0;
}
