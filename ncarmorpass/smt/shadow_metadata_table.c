#include <stdio.h>
#include <stdlib.h>
#include "uthash.h"

int getAvailableId(VirtualMemory *vMem) {
	for (int i = 0; i < SM_SIZE; i++) {
		if (vMem->sm.table[i].valid != 1) {
			printf("New virtual ID: %i\n", i);
			return i;
		}
	}
	printf("No virtual ID available.\n");
	return -1;
}

// TODO: change name to create
VirtualPointer VirtualMemory_set(VirtualMemory *vMem, void *p, unsigned char mod,
		unsigned char sec, unsigned short size, unsigned char type) {
	//printf("In vm set\n");
	//printf("%u\n", type);


	// Add key, value to hash table
	//int newId = vMem->count + 1;
	int newId = getAvailableId(vMem);
	if (newId == -1) {
		printf("Maximum memory entries reached.\n");
		exit(0);
	}

	VirtualPointer vp = { .val = newId };

	// Initialize shield object
	// Need to malloc for this object, otherwise you are just refering to the stack ptr.
	//ShieldObject *obj = (ShieldObject *)malloc(sizeof(ShieldObject));
	//obj->vptr = vp;
	//obj->ptr = p;
	// add ShieldObject to hashmap with VirtualPointer as key
	// HASH_ADD(hh, vMem->vMap, vptr, sizeof(VirtualPointer), obj);

	// Add ShieldObject to PointerCast table
	ShieldObject sObj = { .ptr = p, .vptr = vp };
	vMem->pc.table[newId] = sObj;

	// Add pointer info to shadow metadata table
	ShadowMetadata_Entry e = { .valid = 1, .mod = mod, .sec = sec, .size = size, .type = type };
	vMem->sm.table[newId] = e;
	vMem->sm.count++;

	//printf("New count: %i\n", vMem->sm.count);
	//printf("Mod value: %u\n", vMem->sm.table[newId].sec);

	return vp;
}


ShieldObject VirtualMemory_get(VirtualMemory *vMem, VirtualPointer vp) {
	// Get ShieldObject from hash
	//ShieldObject *objPtr;
	//HASH_FIND(hh, vMem->vMap, &vp, sizeof(VirtualPointer), objPtr);

	// temp
	//if (!objPtr) {
	//	printf("Empty pointer!\n");
	//}

	//return objPtr;

	return vMem->pc.table[vp.val];
}

VirtualPointer __shield_malloc__(VirtualMemory *vm, size_t s,
		unsigned char sec, unsigned char type) {
	// TODO: replace malloc with valgrind-style malloc
	void* ptr = malloc(s);
	return VirtualMemory_set(vm, ptr, 0, sec, (unsigned short)s, type);
}

/*
void* __shield_deref__(VirtualMemory *vm, VirtualPointer vptr) {
	return VirtualMemory_get(vm, vptr).ptr;
}


// TODO
int* __shield_memcpy__() {
        return NULL;
}
*/
int main() {

	//VirtualMemory vm = { .count = 0, .vMap = NULL };
	// Cannot initialize like above since value will not persist.
	// Must malloc.
	VirtualMemory *vm = (VirtualMemory *)malloc(sizeof(VirtualMemory));

	printf("===Allocating Virtual Pointer===\n");
	VirtualPointer i = __shield_malloc__(vm, sizeof(int), 1, 1);

	// Print SMT table entry
	printf("===Shadow Metadata Entry===\n");
	printf("Count: %i\n", vm->sm.count);
	printf("Mod: %u\n", vm->sm.table[i.val].mod);
	printf("Sec: %u\n", vm->sm.table[i.val].sec);
	printf("Type: %u\n", vm->sm.table[i.val].type);
	printf("Valid: %u\n", vm->sm.table[i.val].valid);
	printf("Size: %hu\n", vm->sm.table[i.val].size);

	printf("===Assigning Value to Virtual Pointer===\n");
	*((int *)(__shield_deref__(vm, i))) = 10;

	printf("===Dereferencing Virtual Pointer===\n");
	printf("Value: %i\n", *((int *)__shield_deref__(vm, i)));

	/*
	// Insert SM entry
	vm.sm.table[0].mod = 1;
	vm.sm.table[0].sec = 1;
	vm.sm.table[0].size = 11;
	vm.sm.table[0].type = 1;
	vm.sm.count++;

	printf("%u\n", vm.sm.table[0].mod);
	printf("%hu\n", vm.sm.table[0].size);
	printf("Size: %zu\n", sizeof(vm.sm.table[0]));
	printf("SM size: %i\n", vm.sm.count);
	*/

	/*
	SMT[0].mod = 1;
	SMT[0].sec = 1;
	SMT[0].size = 11;
	SMT[0].type = 1;

	printf("%u\n", SMT[0].mod);
	printf("%hu\n", SMT[0].size);
	printf("Size: %zu\n", sizeof(SMT[0]));
	*/

	//printf("What a day.\n");

	return 0;
}
