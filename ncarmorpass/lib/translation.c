typedef struct {
	int 			val;	// virtual pointer ID
} VirtualPointer;

typedef struct {
	int 			addr; //physical pointer ID
} PhysicalPointer

#define MACHINE_ONE_PTR 400000000
#define MACHINE_TWO_PTR 400000000

PhysicalPointer virtToPhysical(VirtualPointer vptr)
{
	PhysicalPointer y;
	PhysicalPointer * y = vprt->val % MACHINE_TWO_PTR ;
	return y;
}
