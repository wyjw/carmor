#include <stdlib.h>
#include <iostream>
#include <unordered_map>
// using namespace std;
class Pointer {
	public:
		/*
		 * TODO: define arbitrary pointer
		 */			
};

class VirtualMemory {
	public:
		int* index = 0; 
		std::unordered_map<int*, int*> vMap;

		int* set(int* ptr) {
			vMap[++index] = ptr;
			return index;
		}

		int* get(int* vptr) {
			return vMap[vptr];
		}
};

/*
 * TODO: determine the vptr return type.
 */
int* _carmor_malloc(size_t s) {
	int* ptr;
		
	return ptr;	
}

/*
 * Declare global variables
 */
VirtualMemory vMem;

int main() {
	std::cout << "This is the test for _carmor_malloc." << std::endl;
	
	// create int and store in vMap
	int* i = (int*) malloc(sizeof(int));
	*i = 10;
	printf("%i\n", *i);
	
	// this is the virtual pointer
	int* obj_ptr = vMem.set(i);

	// use vptr to modify target value
	*(vMem.get(obj_ptr)) = 20;
	printf("%i\n", *(vMem.get(obj_ptr)));

	
	return 0;
}
