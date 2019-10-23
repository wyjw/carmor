/* Carmor Runtime Layer */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "common.h"

#ifndef SDK_BUILD

#include <strings.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>
#include <setjmp.h>
#include <mntent.h>
#include <libgen.h>
#include <getopt.h>
#include <ftw.h>
#include <poll.h>
#include <pthread.h>
#include <libintl.h>
#include <nl_types.h>
#include <iconv.h>
#include <locale.h>
#include <langinfo.h>
#include <monetary.h>
#include <wchar.h>
#include <semaphore.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <grp.h>
#include <pwd.h>
#include <shadow.h>
#include <signal.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <malloc.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif


#define TRUE 1
#define FALSE 0

// TLB Section
#define TLB_SIZE 5

struct s_victim_cache {
	int32_t bs_page_index;
	uint16_t epc_page_index;
	uint8_t is_dirty;
};

// Helper functions typedefs
typedef int (*t_mem_function)(void*, void*, size_t);
typedef uintptr_t (*t_mem_function_single)(void*, int, size_t);

#ifdef JEMALLOC

extern void* je_malloc(size_t size);
extern void* je_calloc(size_t count, size_t size);
extern void* je_realloc(void* ptr, size_t size);
extern size_t je_malloc_usable_size(void* ptr);
extern void  je_free(void* ptr);

#endif

#ifdef TCMALLOC

extern void* tc_malloc(size_t size);
extern void* tc_calloc(size_t count, size_t size);
extern void* tc_realloc(void* ptr, size_t size);
extern void  tc_free(void* ptr);

#endif

__thread struct s_victim_cache gt_TLB[TLB_SIZE];

static __thread void* gt_direct_buffer;

// Optimization to not instrument file I/O libc wrappers if there is no mmap lstore in action for this application
//
static char g_storage_used = FALSE;

#define MAX_ACCESS_SIZE 64
__thread char gt_CrossPageBuffer[MAX_ACCESS_SIZE];
__thread uintptr_t gt_crosspage_access;
__thread int gt_crosspage_access_size;

static void* __carmor_get_lstore_direct_buffer(size_t mpage_size)
{
	if (!gt_direct_buffer)
	{
		gt_direct_buffer = memalign(mpage_size, mpage_size);
		ASSERT(gt_direct_buffer);
	}

	return gt_direct_buffer;
}

/* Helper debugging functions */

#ifdef SDK_BUILD
extern "C" void ocall_debug(const char* str);
extern "C" void ocall_untrusted_alloc(void** ptr, size_t size);
#endif

/* Forward declarations */
void* __carmor_memset(void *dest, int c, size_t n);
int __carmor_memcmp(const void *vl, const void *vr, size_t n);
void* __carmor_memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n);
void* __carmor_memmove(void *dest, const void *src, size_t n);

/* Init & Cleanup wrappers */

void* __carmor_allocate_buffer(size_t size)
{
	// Note: we always allocate with extra HW PAGE to later align to hardware pages for better performance.
	//
	size_t alloc_size = size + 0x1000;
	void* bs_ptr = NULL;

	ocall_untrusted_alloc(&bs_ptr, alloc_size);
	// Note: workaround for SCONE. They don't have page cache,
	// so allocating annonymous memory backed by "a file" will actually be untrusted memory
	//
	int fd = open("/dev/zero", O_RDWR);
	bs_ptr = mmap(0, alloc_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	close(fd);

	ASSERT (bs_ptr != MAP_FAILED);
	ASSERT (bs_ptr);
	return bs_ptr;
}

extern struct s_victim_cache* lstore_get_tlb();

struct s_victim_cache* lstore_get_tlb_template()
{
	return gt_TLB;
}

//lstores are the local memory (local store)
// Forward declarations
int lstore_init(void* priv_data);
int lstore_cleanup();
uintptr_t lstore_get_mstorage_page(uintptr_t ptr);
uintptr_t lstore_get_mpage_cache_base_ptr();
void* lstore_alloc(size_t size, void* priv_data);
size_t lstore_get_mpage_size();
int lstore_get_mpage_bits();
void notify_tlb_cached(void* ptr);
void notify_tlb_dropped(void* ptr, bool dirty);
void* mpf_handler_c(void* ptr);
void mpf_handler_d(void* ptr, void* dst, size_t size);
void lstore_write_back(void* ptr, void* src, size_t size);

void lstore_open(char* filename, int fd, char always_register, char* registered);
void lstore_close(int fd);

ssize_t lstore_write(int fd, const void* buf, size_t count);
ssize_t lstore_read(int fd, void* buf, size_t count);

ssize_t lstore_write(int fd, const void* buf, size_t count)
{
	return write(fd,buf,count);
}
ssize_t lstore_read(int fd, void* buf, size_t count)
{
	return read(fd,buf,count);
}

uintptr_t lstore_get_mpage_cache_base_ptr()
{
	return 0;
}

uintptr_t lstore_get_mstorage_page(uintptr_t ptr)
{
	g_debug("direct_lstore_get_mstorage_page not supported\n");
	abort();
	return 0;
}

void lstore_open(char* filename, int fd, char always_register, char* registered)
{
}

void lstore_close(int fd)
{
}

void* lstore_alloc(size_t size, void* priv_data)
{
	return nullptr;
}

// Template method for initializing lstores. Priv_data will be sent by the compiler based on the parsed config file when needed
//
int __carmor_initialize_template(void* priv_data)
{
	// Note: the compiler will replace this called function with the correct lstore_init callback

	int res = lstore_init(priv_data);

	if (res)
	{
		g_debug("Failed initializing lstore, returned error is %d\n", res);
		abort();
	}

	if (!lstore_is_direct())
	{
		memset(gt_TLB, 0xFF, sizeof(struct s_victim_cache) * TLB_SIZE);
		gt_crosspage_access = 0;
		gt_crosspage_access_size = 0;
	}

	return res;
}

int __carmor_cleanup_template()
{
	int res = lstore_cleanup();

	if (res)
	{
		g_debug("Failed releasing lstore resources, returned error is %d\n", res);
		abort();
	}

	return res;
}

size_t lstore_get_min_size();
size_t lstore_get_max_size();
uintptr_t lstore_tag(void* ptr);

size_t lstore_get_min_size()
{
	return 0;
}

size_t lstore_get_max_size()
{
	return 1000000000000;
}

void* __carmor_malloc_template(size_t size)
{
	if ((size < lstore_get_min_size() || size >= lstore_get_max_size()))
	{
		return malloc(size);
	}

	void* bs_ptr = lstore_alloc(size, nullptr);
	void* masked_bs_ptr = (void*) lstore_tag(bs_ptr);

	return masked_bs_ptr;
}

void* __carmor_calloc_template(size_t num_of_elements, size_t elements_size) {
	void* ptr = __carmor_malloc_template(num_of_elements * elements_size);
	__carmor_memset(ptr, 0, num_of_elements * elements_size);
	return ptr;
}

void* __attribute__((noinline)) __carmor_lstore_annotation(void* ptr)
{
	return ptr;
}

void* __carmor_memalign_template(size_t alignment, size_t n) {
	// Alignment is inherited in lstore page cache
	//
	void* bs_mem = lstore_alloc(n + (alignment-1) + sizeof(void*), nullptr);

    if(!bs_mem) {
    	return NULL;
    }

    char *bs_aligned_mem = ((char*)bs_mem) + sizeof(void*);
    bs_aligned_mem += (alignment - ((uintptr_t)bs_aligned_mem & (alignment - 1)) & (alignment-1));

    ((void**)bs_aligned_mem)[-1] = bs_mem;

    return (void*)lstore_tag(bs_aligned_mem);
}

/* Allocation & Deallocation Methods */
void __carmor_free(void* p) {
	if (!IS_CARMOR_PTR(p))
	{
		free(p);
		return;
	}

	void* unmasked_bs_ptr = (void*) (uintptr_t)MAKE_ORIGINAL_POINTER(p);

	int topByte = (uintptr_t)p >> 60;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			_0_lstore_free(unmasked_bs_ptr);
			break;
		case lstore1_SHIFT60_VAL:
			_1_lstore_free(unmasked_bs_ptr);
			break;
		case lstore2_SHIFT60_VAL:
			_2_lstore_free(unmasked_bs_ptr);
			break;
		case lstore3_SHIFT60_VAL:
			_3_lstore_free(unmasked_bs_ptr);
			break;
		default:
			abort();
	}
}

void* __carmor_realloc(void* ptr, size_t size) {
	if (!IS_CARMOR_PTR(ptr)) {
		void* res = realloc(ptr,size);
		return res;
	}

	int orig_ptr_size = -1;
	void* res = nullptr;
	int topByte = (uintptr_t)ptr >> 60;
	void* unamsked_ptr = (void*)MAKE_ORIGINAL_POINTER(ptr);

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			orig_ptr_size = _0_lstore_alloc_size(unamsked_ptr);
			break;
		case lstore1_SHIFT60_VAL:
			orig_ptr_size = _1_lstore_alloc_size(unamsked_ptr);
			break;
		case lstore2_SHIFT60_VAL:
			orig_ptr_size = _2_lstore_alloc_size(unamsked_ptr);
			break;
		case lstore3_SHIFT60_VAL:
			orig_ptr_size = _3_lstore_alloc_size(unamsked_ptr);
			break;
		default:
			abort();
	}

	if (orig_ptr_size >= size) {
		return ptr; // just reuse the same memory
	}

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			res = (void*)MAKE_lstore0_PTR(_0_lstore_alloc(size, nullptr));
			break;
		case lstore1_SHIFT60_VAL:
			res = (void*)MAKE_lstore1_PTR(_1_lstore_alloc(size, nullptr));
			break;
		case lstore2_SHIFT60_VAL:
			res = (void*)MAKE_lstore2_PTR(_2_lstore_alloc(size, nullptr));
			break;
		case lstore3_SHIFT60_VAL:
			res = (void*)MAKE_lstore3_PTR(_3_lstore_alloc(size, nullptr));
			break;
		default:
			abort();
	}

	__carmor_memcpy(res, ptr, orig_ptr_size);
	__carmor_free(ptr);

	return res;
}

int32_t __carmor_get_valid_iterations_template(const void* unlinked_ptr, const void* ptr, int64_t stepVal, int32_t ptrSize)
{
	if (likely(!IS_CARMOR_PTR(unlinked_ptr)))
	{
		// Return max val so native pointers will enjoy interuptless loops
		//
		return (1<<30);
	}

	if (ptr == gt_CrossPageBuffer)
	{
		return 1;
	}

	int page_offset = (uintptr_t) ((uintptr_t)ptr - lstore_get_mpage_cache_base_ptr()) & (lstore_get_mpage_size() - 1);
	int realStep = stepVal;
	int left_bytes = (realStep > 0) ? (lstore_get_mpage_size() - page_offset) : -(page_offset+1);
	int32_t res = left_bytes / realStep;

	if (page_offset + ((res-1) * realStep) + ptrSize > lstore_get_mpage_size())
	{
		res--;
	}

	if (res <= 0)
	{
		return 1;
	}

	// TODO: theoretically we don't need MAKE_ORIGINAL_POINTER(ptr) but just ptr since its linked, but need to test this before removing
	// ASSERT((((uintptr_t)(MAKE_ORIGINAL_POINTER(ptr) - lstore_get_mpage_cache_base_ptr()) & (lstore_get_mpage_size()-1)) + realStep * (res-1) + ptrSize) <= lstore_get_mpage_size());

	return res;
}

INLINEATTR void* __carmor_link_cached_template(const void* ptr, int ptr_size, char is_vector_type, char dirty)
{
	if (likely(!IS_CARMOR_PTR(ptr)))
	{
		return (void*)ptr;
	}

	ASSERT(!is_vector_type);

	uintptr_t unmasked_bs_ptr = MAKE_ORIGINAL_POINTER(ptr);

	uintptr_t mstorage_page = lstore_get_mstorage_page(unmasked_bs_ptr);
	unsigned bs_page_index =  (mstorage_page >> lstore_get_mpage_bits());
	unsigned page_offset= (mstorage_page & (lstore_get_mpage_size()-1));

#ifndef DISABLE_TLB
	struct s_victim_cache* tlb = lstore_get_tlb();

	int tlb_index=0;

	do {
		if (tlb[tlb_index].bs_page_index == bs_page_index) {
			tlb[tlb_index].is_dirty |= dirty;
			uintptr_t res = lstore_get_mpage_cache_base_ptr() + tlb[tlb_index].epc_page_index * lstore_get_mpage_size() + page_offset;

			ASSERT((page_offset + ptr_size) <= lstore_get_mpage_size());

			return (void*)res;
		}
		tlb_index++;
	} while (tlb_index < TLB_SIZE);
#endif

	// TODO: res should be actually for EPC page but not with offset
	void* res = mpf_handler_c((void*)unmasked_bs_ptr);

#ifndef DISABLE_TLB
	// Decrease ref count since no one else in this thread is currently using this value
	// No need for lock because its per thread
	//
	int removed_page_index = tlb[TLB_SIZE-1].bs_page_index;
	if (removed_page_index >= 0)
	{
		void* removed_bs_page = (void*)(unmasked_bs_ptr - ((bs_page_index - removed_page_index) * lstore_get_mpage_size()));
		notify_tlb_dropped(removed_bs_page, tlb[TLB_SIZE-1].is_dirty);
	}

	// Now, add the new page to the TLB, in FIFO way
	memmove(&tlb[1], &tlb[0], sizeof(struct s_victim_cache) * (TLB_SIZE-1));

	notify_tlb_cached((void*)unmasked_bs_ptr);

	tlb[0].bs_page_index = bs_page_index;// (unmasked_bs_ptr - g_mstorage_base_ptr) >> COSMIX_PAGE_BITS;
	tlb[0].epc_page_index = ((uintptr_t)res - lstore_get_mpage_cache_base_ptr()) >> lstore_get_mpage_bits();
	tlb[0].is_dirty = dirty;

#endif

	ASSERT((((uintptr_t)((uintptr_t)res - lstore_get_mpage_cache_base_ptr()) & (lstore_get_mpage_size()-1)) + ptr_size) <= lstore_get_mpage_size());

	return res;
}

void* __carmor_page_fault(const void* bs_ptr, char dirty) {
	int topByte = (uintptr_t)bs_ptr >> 60;

	void* res = NULL;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
				res = _0__carmor_link(bs_ptr, 0, 0, dirty);
				break;
		case lstore1_SHIFT60_VAL:
				res = _1__carmor_link(bs_ptr, 0, 0, dirty);
				break;
		case lstore2_SHIFT60_VAL:
				res = _2__carmor_link(bs_ptr, 0, 0, dirty);
				break;
		case lstore3_SHIFT60_VAL:
				res = _3__carmor_link(bs_ptr, 0, 0, dirty);
				break;
		default:
				abort();
	}

	return res;
}

INLINEATTR void __carmor_writeback_cached_cross_page_template()
{
	if (gt_crosspage_access != 0)
	{
		ASSERT(gt_crosspage_access_size != 0);

		uintptr_t bs_ptr = gt_crosspage_access;
		int size = gt_crosspage_access_size;

		// Reset for next cross page access
		//
		gt_crosspage_access = 0;
		gt_crosspage_access_size = 0;

		// writeback for old value
		//
		uintptr_t mstorage_page1 = lstore_get_mstorage_page(bs_ptr);
		unsigned page_offset1= (mstorage_page1 & (lstore_get_mpage_size()-1));
		unsigned bytes_to_copy_from_first_page = lstore_get_mpage_size() - page_offset1;

		void* first_page_ptr = __carmor_page_fault((void*)bs_ptr, TRUE);
		void* second_page_ptr = __carmor_page_fault((char*)bs_ptr + bytes_to_copy_from_first_page, TRUE);

		memcpy(first_page_ptr, gt_CrossPageBuffer, bytes_to_copy_from_first_page);
		memcpy(second_page_ptr, gt_CrossPageBuffer + bytes_to_copy_from_first_page, size - bytes_to_copy_from_first_page);
	}
}

void __carmor_init_global_template(void* newGlobal, void* global, int size)
{
	// alloc
	//
	void* new_global_memory = (void*)lstore_tag(lstore_alloc(size, nullptr));

	// copy global data into lstore memory
	//
	__carmor_memcpy(new_global_memory, global, size);

	// set newGlobal to point to the new global memory
	//
	*(void**)newGlobal = new_global_memory;
}

INLINEATTR void* __carmor_link_cached_cross_page_template(const void* ptr, int ptr_size, char is_vector_type, char dirty)
{
	if (likely(!IS_CARMOR_PTR(ptr)))
	{
		return (void*)ptr;
	}

	ASSERT(!is_vector_type);

	uintptr_t unmasked_bs_ptr = MAKE_ORIGINAL_POINTER(ptr);
	uintptr_t mstorage_page = lstore_get_mstorage_page(unmasked_bs_ptr);
	unsigned page_offset= (mstorage_page & (lstore_get_mpage_size()-1));

	if (page_offset + ptr_size > lstore_get_mpage_size())
	{
		// load from both pages into gt_CrossPageBuffer temp buffer
		//
		unsigned bytes_to_copy_from_first_page = lstore_get_mpage_size() - page_offset;
		void* first_page_ptr = __carmor_page_fault(ptr, dirty);
		void* second_page_ptr = __carmor_page_fault((char*)ptr + bytes_to_copy_from_first_page, dirty);

		memcpy(gt_CrossPageBuffer, first_page_ptr, bytes_to_copy_from_first_page);
		memcpy(gt_CrossPageBuffer + bytes_to_copy_from_first_page, second_page_ptr, ptr_size - bytes_to_copy_from_first_page);

		gt_crosspage_access = (uintptr_t)ptr;
		gt_crosspage_access_size = ptr_size;

		return gt_CrossPageBuffer;
	}

	unsigned bs_page_index =  (mstorage_page >> lstore_get_mpage_bits());

#ifndef DISABLE_TLB
	struct s_victim_cache* tlb = lstore_get_tlb();

	int tlb_index=0;

	do {
		if (tlb[tlb_index].bs_page_index == bs_page_index) {
			tlb[tlb_index].is_dirty |= dirty;
			uintptr_t res = lstore_get_mpage_cache_base_ptr() + tlb[tlb_index].epc_page_index * lstore_get_mpage_size() + page_offset;

			ASSERT((page_offset + ptr_size) <= lstore_get_mpage_size());

			return (void*)res;
		}
		tlb_index++;
	} while (tlb_index < TLB_SIZE);
#endif

	// TODO: res should be actually for EPC page but not with offset
	void* res = mpf_handler_c((void*)unmasked_bs_ptr);

#ifndef DISABLE_TLB
	// Decrease ref count since no one else in this thread is currently using this value
	// No need for lock because its per thread
	//
	int removed_page_index = tlb[TLB_SIZE-1].bs_page_index;
	if (removed_page_index >= 0)
	{
		void* removed_bs_page = (void*)(unmasked_bs_ptr - ((bs_page_index - removed_page_index) * lstore_get_mpage_size()));
		notify_tlb_dropped(removed_bs_page, tlb[TLB_SIZE-1].is_dirty);
	}

	// Now, add the new page to the TLB, in FIFO way
	memmove(&tlb[1], &tlb[0], sizeof(struct s_victim_cache) * (TLB_SIZE-1));

	notify_tlb_cached((void*)unmasked_bs_ptr);

	tlb[0].bs_page_index = bs_page_index;// (unmasked_bs_ptr - g_mstorage_base_ptr) >> COSMIX_PAGE_BITS;
	tlb[0].epc_page_index = ((uintptr_t)res - lstore_get_mpage_cache_base_ptr()) >> lstore_get_mpage_bits();
	tlb[0].is_dirty = dirty;

#endif

	ASSERT((((uintptr_t)((uintptr_t)res - lstore_get_mpage_cache_base_ptr()) & (lstore_get_mpage_size()-1)) + ptr_size) <= lstore_get_mpage_size());

	return res;
}

int32_t __carmor_get_valid_iterations_generic(const void* unlinked_ptr, const void* ptr, int64_t stepVal, int32_t ptrSize) {
	if (likely(!IS_CARMOR_PTR(unlinked_ptr))) {
		// Return max val so native pointers will enjoy interuptless loops
		//
		return (1<<30);
	}

	int topByte = (uintptr_t)unlinked_ptr >> 60;

	int32_t res;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
				res = _0__carmor_get_valid_iterations(unlinked_ptr, ptr, stepVal, ptrSize);
				break;
		case lstore1_SHIFT60_VAL:
				res = _1__carmor_get_valid_iterations(unlinked_ptr, ptr, stepVal, ptrSize);
				break;
		case lstore2_SHIFT60_VAL:
				res = _2__carmor_get_valid_iterations(unlinked_ptr, ptr, stepVal, ptrSize);
				break;
		case lstore3_SHIFT60_VAL:
				res = _3__carmor_get_valid_iterations(unlinked_ptr, ptr, stepVal, ptrSize);
				break;
		default:
				abort();
	}

	return res;
}

// Invokes lstore_write_back
//
INLINEATTR void __carmor_writeback_direct_template(const void* ptr, int ptr_size)
{
	if (likely(!IS_CARMOR_PTR(ptr)))
	{
		return;
	}

	void* unmasked_bs_ptr = (void*)MAKE_ORIGINAL_POINTER(ptr);

	int topByte = (uintptr_t)ptr >> 60;
	size_t mpage_size;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			mpage_size = _0_lstore_get_mpage_size();
			break;
		case lstore1_SHIFT60_VAL:
			mpage_size = _1_lstore_get_mpage_size();
			break;
		case lstore2_SHIFT60_VAL:
			mpage_size = _2_lstore_get_mpage_size();
			break;
		case lstore3_SHIFT60_VAL:
			mpage_size = _3_lstore_get_mpage_size();
		default:
			abort();
	}

	void* dest = __carmor_get_lstore_direct_buffer(mpage_size);

	// Note: internally, there might be multiple fetches from mstorage if the offset is between mpages
	//
	lstore_write_back(unmasked_bs_ptr, dest, ptr_size);
}

void __carmor_writeback_generic(const void* ptr, int ptr_size)
{
	if (likely(!IS_CARMOR_PTR(ptr)))
	{
		return;
	}

	int topByte = (uintptr_t)ptr >> 60;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			_0__carmor_writeback_direct(ptr, ptr_size);
			break;
		case lstore1_SHIFT60_VAL:
			_1__carmor_writeback_direct(ptr, ptr_size);
			break;
		case lstore2_SHIFT60_VAL:
			_2__carmor_writeback_direct(ptr, ptr_size);
			break;
		case lstore3_SHIFT60_VAL:
			_3__carmor_writeback_direct(ptr, ptr_size);
			break;
		default:
			abort();
	}
}

INLINEATTR void* __carmor_link_direct_template(const void* ptr, int ptr_size, char is_vector_type, char dirty)
{
	if (likely(!IS_CARMOR_PTR(ptr)))
	{
		return (void*)ptr;
	}

	void* unmasked_bs_ptr = (void*)MAKE_ORIGINAL_POINTER(ptr);

	int topByte = (uintptr_t)ptr >> 60;
	size_t mpage_size;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			mpage_size = _0_lstore_get_mpage_size();
			break;
		case lstore1_SHIFT60_VAL:
			mpage_size = _1_lstore_get_mpage_size();
			break;
		case lstore2_SHIFT60_VAL:
			mpage_size = _2_lstore_get_mpage_size();
			break;
		case lstore3_SHIFT60_VAL:
			mpage_size = _3_lstore_get_mpage_size();
		default:
			abort();
	}

	// Note: for now, just assert it, otherwise should divide to two separate requests
	//
	ASSERT(mpage_size >= ptr_size);

	void* dest = __carmor_get_lstore_direct_buffer(mpage_size);

	// Note: internally, there might be multiple fetches from mstorage if the offset is between mpages
	//
	mpf_handler_d(unmasked_bs_ptr, dest, ptr_size);

	return dest;
}

void* __carmor_link_generic(const void* ptr, int ptr_size, char is_vector_type, char dirty)
{
	if (likely(!IS_CARMOR_PTR(ptr))) {
		return (void*)ptr;
	}

	int topByte = (uintptr_t)ptr >> 60;

	void* res = NULL;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			res = _0__carmor_link(ptr, ptr_size, is_vector_type, dirty);
			break;
		case lstore1_SHIFT60_VAL:
			res = _1__carmor_link(ptr, ptr_size, is_vector_type, dirty);
			break;
		case lstore2_SHIFT60_VAL:
			res = _2__carmor_link(ptr, ptr_size, is_vector_type, dirty);
			break;
		case lstore3_SHIFT60_VAL:
			res = _3__carmor_link(ptr, ptr_size, is_vector_type, dirty);
			break;
		default:
			abort();
	}

	return res;
}

/* MemIntrinsic functions */

// Mem intrinsics managements wrapper functions

char lstore_type_is_direct(void* ptr)
{
	int topByte = (uintptr_t)ptr >> 60;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			return _0_is_direct();
		case lstore1_SHIFT60_VAL:
			return _1_is_direct();
		case lstore2_SHIFT60_VAL:
			return _2_is_direct();
		case lstore3_SHIFT60_VAL:
			return _3_is_direct();
		default:
			abort();
	}

	return FALSE;
}

void get_lstore_parameters(void* ptr, uintptr_t* base_addr, size_t* block_size)
{
	int topByte = (uintptr_t)ptr >> 60;

	switch (topByte)
	{
		case lstore0_SHIFT60_VAL:
			// Note: we replace these calls with the correct lstore value. The compiler will take all these and call correct lstore callback
			*base_addr = _0_lstore_get_mpage_cache_base_ptr();
			*block_size = _0_lstore_get_mpage_size();
			break;
		case lstore1_SHIFT60_VAL:
			*base_addr = _1_lstore_get_mpage_cache_base_ptr();
			*block_size = _1_lstore_get_mpage_size();
			break;
		case lstore2_SHIFT60_VAL:
			*base_addr = _2_lstore_get_mpage_cache_base_ptr();
			*block_size = _2_lstore_get_mpage_size();
			break;
		case lstore3_SHIFT60_VAL:
			*base_addr = _3_lstore_get_mpage_cache_base_ptr();
			*block_size = _3_lstore_get_mpage_size();
		default:
			abort();
	}
}

int __carmor_mem_wrapper(void *vl, char vl_dirty, void *vr, char vr_dirty, size_t n, t_mem_function func, char partial_return)
{
	int vl_linked = IS_CARMOR_PTR(vl);
	int vr_linked = IS_CARMOR_PTR(vr);

	if (likely(!vl_linked && !vr_linked))
	{
		return (*func)(vl,vr,n);
	}

	// TODO: if direct, then should only use writeback if vl dirty, and page_fault if vr dirty
	//
	char is_lstore_direct = vl_linked ? lstore_type_is_direct(vl) : lstore_type_is_direct(vr);
	if (is_lstore_direct)
	{
		// TODO: add support to multiple variable direct lstores
		//
		ASSERT(!(vl_linked && vr_linked));
	}

	uintptr_t vl_base_addr;
	uintptr_t vr_base_addr;
	size_t vl_block_size;
	size_t vr_block_size;

	if (vl_linked)
	{
		get_lstore_parameters(vl, &vl_base_addr, &vl_block_size);
	}

	if (vr_linked)
	{
		get_lstore_parameters(vr, &vr_base_addr, &vr_block_size);
	}

	int ctr = n;
	unsigned char* vl0 = vl_linked ? (unsigned char*)__carmor_page_fault(vl, vl_dirty) : (unsigned char*)vl;
	unsigned char* vr0 = vr_linked ? (unsigned char*)__carmor_page_fault(vr, vr_dirty) : (unsigned char*)vr;
	unsigned char* vl1 = (unsigned char*)vl;
	unsigned char* vr1 = (unsigned char*)vr;

	unsigned vl_block_offset_mask = (vl_block_size -1);
	unsigned vr_block_offset_mask = (vr_block_size -1);

	while (ctr > 0) {
		int vr_page_offset = (uintptr_t)(vr0 - vr_base_addr) & vr_block_offset_mask;
		int vl_page_offset = (uintptr_t)(vl0 - vl_base_addr) & vl_block_offset_mask;
		int left_in_vr = vr_linked ? (vr_block_size - vr_page_offset) : (1<< 30);
		int left_in_vl = vl_linked ? (vl_block_size - vl_page_offset) : (1<< 30);
		int min_left = left_in_vr < left_in_vl ? left_in_vr : left_in_vl;

		if (ctr > min_left) {
			int res = (*func)((void*)vl0, (void*)vr0, min_left);

			if (is_lstore_direct)
			{
				if (vl_linked && vl_dirty)
				{
					__carmor_writeback_generic(vl1, vl_block_size);
				}

				if (vr_linked && vr_dirty)
				{
					__carmor_writeback_generic(vr1, vr_block_size);
					//__carmor_write_page(vr1);
				}
			}

			if (partial_return && res != 0)
			{
				return res;
			}

			vl1 += min_left;
			vr1 += min_left;

			if (min_left == left_in_vl && vl_linked)
			{
				vl0 = (unsigned char*)__carmor_page_fault(vl1, vl_dirty);
			}
			else
			{
				vl0 += min_left;
			}

			if (min_left == left_in_vr && vr_linked)
			{
				vr0 = (unsigned char*)__carmor_page_fault(vr1, vr_dirty);
			}
			else
			{
				vr0 += min_left;
			}

			ctr -= min_left;
		} else {
			int res = (*func)((void*)vl0, (void*)vr0, ctr);

			if (is_lstore_direct)
			{
				if (vl_linked && vl_dirty)
				{
					__carmor_writeback_generic(vl1, vl_block_size);
				}

				if (vr_linked && vr_dirty)
				{
					__carmor_writeback_generic(vr1, vr_block_size);
					//__carmor_write_page(vr1);
				}
			}

			if (partial_return && res != 0)
			{
				return res;
			}

			ctr -= ctr;
		}
	}

	return 0;
}

uintptr_t __carmor_mem_wrapper_single(void* dest, int c, size_t n, t_mem_function_single func, char dirty, char partial_return, char aggregate_res) {
	if (likely(!IS_CARMOR_PTR(dest)))
	{
		return (*func)(dest, c, n);
	}

	int topByte = (uintptr_t)dest >> 60;
	size_t acc = 0;

	uintptr_t base_addr;
	size_t block_size;
	char is_lstore_direct = lstore_type_is_direct(dest);

	get_lstore_parameters(dest, &base_addr, &block_size);

	unsigned block_offset_mask = (block_size -1);

	int ctr = n;
	unsigned char* dst_orig = (unsigned char*)dest;
	unsigned char* dst0 = (unsigned char*)__carmor_page_fault(dest, dirty);

	while (ctr > 0) {
		int dest_offset = (uintptr_t)(dst0 - base_addr) & block_offset_mask;
		int left_in_dest = block_size - dest_offset;

		if (ctr > left_in_dest) {
			uintptr_t res = (*func)((void*)dst0, c, left_in_dest);

			if (is_lstore_direct)
			{
				// __carmor_write_page(dst_orig);
				__carmor_writeback_generic(dst_orig, block_size);
			}

			if (partial_return && res != 0)
			{
				return res;
			}

			if (aggregate_res)
			{
				acc += (size_t)res;
			}

			dst_orig += left_in_dest;
			dst0 = (unsigned char*)__carmor_page_fault(dst_orig, dirty);

			ctr -= left_in_dest;
		} else {
			uintptr_t res = (*func)((void*)dst0, c, ctr);

			if (is_lstore_direct)
			{
				// __carmor_write_page(dst_orig);
				__carmor_writeback_generic(dst_orig, block_size);
			}

			if (partial_return && res != 0)
			{
				return res;
			}

			if (aggregate_res)
			{
				acc += (size_t)res;
			}

			ctr -= ctr;
		}
	}

	if (aggregate_res)
	{
		return (uintptr_t)acc;
	}

	return 0;
}

int __carmor_memmove_wrapper(void* dst, void* src, size_t n)
{
	memmove(dst, (const void*)src, n);
	return 0;
}

int __carmor_memcpy_wrapper(void* dst, void* src, size_t n)
{
	memcpy(dst, (const void*)src, n);
	return 0;
}

int __carmor_memcmp_wrapper(void* dst, void* src, size_t n)
{
	return memcmp((const void*)dst, (const void*)src, n);
}

uintptr_t __carmor_memchr_wrapper(void* dst, int c, size_t n)
{
	return (uintptr_t)memchr((const void*)dst, c, n);
}

uintptr_t __carmor_memset_wrapper(void* dst, int c, size_t n)
{
	return (uintptr_t)memset(dst, c, n);
}

void* __carmor_memmove(void *dest, const void *src, size_t n)
{
	__carmor_mem_wrapper((void*)dest, TRUE, (void*)src, FALSE, n, &__carmor_memmove_wrapper, FALSE);

	return dest;
}

void* __carmor_memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n)
{
	__carmor_mem_wrapper((void*)dest, TRUE, (void*)src, FALSE, n, &__carmor_memcpy_wrapper, FALSE);

	return dest;
}

int __carmor_memcmp(const void *vl, const void *vr, size_t n)
{
	int res = __carmor_mem_wrapper((void*)vl, FALSE, (void*)vr, FALSE, n, &__carmor_memcmp_wrapper, TRUE);

	return res;
}

void* __carmor_memchr(const void *dest, int c, size_t n)
{
	uintptr_t res = __carmor_mem_wrapper_single((void*)dest, c, n, &__carmor_memchr_wrapper, FALSE, TRUE, FALSE);

	return (void*)res;
}

void* __carmor_memset(void *dest, int c, size_t n)
{
	__carmor_mem_wrapper_single(dest, c, n, &__carmor_memset_wrapper, TRUE, FALSE, FALSE);

	return dest;
}

#endif

#endif

#ifdef __cplusplus
}
#endif
