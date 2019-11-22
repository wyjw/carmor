#include <stdlib.h>

#include "uthash.h"
#include "shadow_metadata_table.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include <linux/userfaultfd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>

typedef unsigned long uptr;

#define PAGE_SIZE 4096
#define u64 unsigned int
#define u32 unsigned int

// global interface symbol
uptr __carmor_shadow_memory_dynamic_address;

//the total amount to be mapped
static uptr g_total_mmaped;

// Derived heavily from ASAN runtime
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//


// Helper function that tells the function what number to round up to.
static size_t RoundUpTo(size_t size, size_t boundary)
{
  return (size + boundary - 1) & ~(boundary - 1);
}

static size_t RoundDownTo(size_t size, size_t boundary)
{
  return size & ~(boundary - 1);
}

void IncreaseTotalMmap(uptr size) {
  g_total_mmaped += size;
}

void DecreaseTotalMmap(uptr size) {
  g_total_mmaped -= size;
}

bool internal_iserror(uptr retval, int *rverrno) {
  if (retval == (uptr)-1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uptr MmapNamed(void *addr, uptr length, int prot, int flags, const char *name) {
	return (uptr)mmap(addr, length, prot, flags, flags, 0);
}

static uptr MmapFixed(uptr fixed_addr, uptr size, int additional_flags, const char * name){
  size = RoundUpTo(size, PAGE_SIZE);
  fixed_addr = RoundDownTo(fixed_addr, PAGE_SIZE);
  uptr p = MmapNamed((void *) fixed_addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED | additional_flags | MAP_ANON, name);

  int reserrno;
  if (internal_iserror(p, &reserrno))
  {
    printf("ERROR: failed to allocate (%zd bytes)", size);
  }
  IncreaseTotalMmap(size);
  return p;
}

void ReserveShadowMemoryRange(uptr beg, uptr end, const char *name) {
  uptr size = end - beg + 1;
  DecreaseTotalMmap(size);  // Don't count the shadow against mmap_limit_mb.
  if (!MmapFixed(beg, size, MAP_NORESERVE, name)) {
    printf("ReserveShadowMemoryRange failed while trying to map 0x%zx bytes. "
        "Perhaps you're using ulimit -v\n",
        size);
    exit(-1);
  }
}

#define VMA_BITS 52
#define HIGH_BITS (64 - VMA_BITS)
#define MEM_TO_SHADOW(mem) \
  ((((mem) << HIGH_BITS) >> (HIGH_BITS + (SHADOW_SCALE))) + (SHADOW_OFFSET))
#define SHADOW_SCALE 5
#define SHADOW_OFFSET 29

#define defaultShadowOffset (1ULL << SHADOW_OFFSET)

#define kLowMemBeg 0
#define kLowMemEnd (defaultShadowOffset - 1)
#define kLowShadowBeg defaultShadowOffset
#define kLowShadowEnd MEM_TO_SHADOW(kLowMemEnd) + 1

#define kMidMemBeg 0x3000000000ULL
#define kMidMemEnd 0x4fffffffffULL

static uptr kHighMemEnd = 0x7fffffffffffULL;
#define kHighMemBeg (MEM_TO_SHADOW(kHighMemEnd) + 1)
#define kHighShadowBeg (MEM_TO_SHADOW(kHighMemBeg))
#define kHighShadowEnd (MEM_TO_SHADOW(kHighMemEnd))

#define kMidShadowBeg MEM_TO_SHADOW(kMidMemBeg)
#define kMidShadowEnd MEM_TO_SHADOW(kMidMemEnd)

#define kShadowGapBeg kLowShadowEnd
#define kShadowGapEnd (kMidShadowBeg - 1)

static inline bool AddrIsInLowMem(uptr a) {
  return a <= kLowMemEnd;
}

static inline bool AddrIsInLowShadow(uptr a) {
  return a >= kLowShadowBeg && a <= kLowShadowEnd;
}

static inline bool AddrIsInMidMem(uptr a) {
  return kMidMemBeg && a >= kMidMemBeg && a <= kMidMemEnd;
}

static inline bool AddrIsInMidShadow(uptr a) {
  return kMidMemBeg && a >= kMidShadowBeg && a <= kMidShadowEnd;
}

static inline bool AddrIsInHighMem(uptr a) {
  return kHighMemBeg && a >= kHighMemBeg && a <= kHighMemEnd;
}

static inline bool AddrIsInHighShadow(uptr a) {
  return kHighMemBeg && a >= kHighShadowBeg && a <= kHighShadowEnd;
}

static inline bool AddrIsInShadowGap(uptr a) {
  return a >= kShadowGapBeg && a <= kShadowGapEnd;
}

static void ProtectGap(uptr addr, uptr size)
{
  uptr res = MmapFixed(addr, size, MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE | MAP_ANON, "shadow gap");
}

uptr GetMaxVirtualAddresses()
{
  return (1ULL << 47) - 1;
}

static void InitializeHighMemEnd()
{
  kHighMemEnd = GetMaxVirtualAddresses();
}

void InitializeShadowMemory() {
  __carmor_shadow_memory_dynamic_address = ~(uptr) 0;

  uptr shadow_start = defaultShadowOffset;
  ReserveShadowMemoryRange(shadow_start, kLowShadowEnd, "low shadow");
  ReserveShadowMemoryRange(kHighShadowBeg, kHighShadowEnd, "high shadow");
  ProtectGap(kShadowGapBeg, kShadowGapEnd - kShadowGapBeg + 1);
}

// ALLOCATE STRUCT
// L L L L L L L H H U U U U U U R R
// Chunk header has info
typedef struct
{
  u32 chunk_state : 8;
  u32 alloc_tid : 24;
  u32 free_tid : 24;
  u32 from_memalign : 1;
  u32 alloc_type : 2;
  u32 rz_log : 3;
  u32 lsan_tag : 2;
} ChunkHeader;

typedef struct {
  ChunkHeader header;
  u32 free_context_id;
} ChunkBase;

typedef struct
{
  ChunkBase base;
  //uptr Beg() { return reinterpret_cast<uptr>(this) + kChunkHeaderSize; }
} CarmorChunk;

int _BitScanReverse(int x)
{
  return 63 - x;
}

uptr MostSignificantSetBitIndex(uptr x) {
  return _BitScanReverse(x);
}

#define kSpaceBeg 0x600000000000ULL
#define kSpaceSize 0x40000000000ULL
#define kMetadataSize 0
#define kNumberClassesRounded 5

typedef struct {
  uptr kMinSize;
  uptr kMidSize;
  uptr kMidClass;
  uptr kMidSizeLog;
  uptr kMaxBytesCachedLog;
  uptr S;
  uptr M;
  uptr kRegionSize;
} SizeClassMap;

static void init_SizeClassMap(uptr kNumBits, uptr kMinSizeLog, uptr kMidSizeLog,
  uptr kMaxSizeLog, uptr kMaxNumCachedHintT, uptr kMaxBytesCachedLog, SizeClassMap * scm)
{
  scm->kMidSizeLog = kMidSizeLog;
  scm->kMaxBytesCachedLog = kMaxBytesCachedLog;
  scm->kMinSize = 1 << kMinSizeLog;
  scm->kMidSize = 1 << kMidSizeLog; //probably
  scm->kMidClass = kMidSizeLog / kMinSizeLog;
  scm->S = kNumBits - 1;
  scm->M = (1 << scm->S) - 1;
  scm->kRegionSize = kMinSizeLog / kNumBits;
}

static uptr Size(SizeClassMap * scm, uptr class_id) {
  class_id -= scm->kMidClass;
  uptr t = scm->kMidSize << (class_id >> scm->S);
  return t + (t >> scm->S) * (class_id & scm->M);
}

static uptr ClassId(SizeClassMap * scm, uptr size) {
  const uptr l = MostSignificantSetBitIndex(size);
  const uptr hbits = (size >> (l - scm->S)) & scm->M;
  const uptr lbits = size & ((1U << (1 - scm->S)) - 1);
  const uptr l1 = 1 - scm->kMidSizeLog;
  return scm->kMidClass + (l1 << scm->S) + hbits + (lbits > 0);
}

static uptr MaxCachedHint(SizeClassMap * scm, uptr size){
  return ((1UL << scm->kMaxBytesCachedLog) / size);
}

//typedef SizeClassMap<3, 4, 8, 17, 128, 16> DefaultSizeClassMap;

void errExit(char* err){
  printf("%s", err);
}

uptr getSizeClass(SizeClassMap * scm, const void *p)
{
  return (((uptr)(p) - kSpaceBeg) / scm->kRegionSize) % kNumberClassesRounded;
}

static void *
fault_handler_thread(void *arg)
{
    int page_size = sysconf(_SC_PAGE_SIZE);
    static struct uffd_msg msg;   /* Data read from userfaultfd */
    static int fault_cnt = 0;     /* Number of faults so far handled */
    long uffd;                    /* userfaultfd file descriptor */
    static char *page = NULL;
    struct uffdio_copy uffdio_copy;
    ssize_t nread;
    uffd = (long) arg;
    /* Create a page that will be copied into the faulting region */
    if (page == NULL) {
        page = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (page == MAP_FAILED)
            errExit("mmap");
    }
    /* Loop, handling incoming events on the userfaultfd
       file descriptor */
    for (;;) {
        /* See what poll() tells us about the userfaultfd */
        struct pollfd pollfd;
        int nready;
        pollfd.fd = uffd;
        pollfd.events = POLLIN;
        nready = poll(&pollfd, 1, -1);
        if (nready == -1)
            errExit("poll");
        printf("\nfault_handler_thread():\n");
        printf("    poll() returns: nready = %d; "
                "POLLIN = %d; POLLERR = %d\n", nready,
                (pollfd.revents & POLLIN) != 0,
                (pollfd.revents & POLLERR) != 0);
        /* Read an event from the userfaultfd */
        nread = read(uffd, &msg, sizeof(msg));
        if (nread == 0) {
            printf("EOF on userfaultfd!\n");
            exit(EXIT_FAILURE);
        }
        if (nread == -1)
            errExit("read");
        /* We expect only one kind of event; verify that assumption */
        if (msg.event != UFFD_EVENT_PAGEFAULT) {
            fprintf(stderr, "Unexpected event on userfaultfd\n");
            exit(EXIT_FAILURE);
        }
        /* Display info about the page-fault event */
        printf("    UFFD_EVENT_PAGEFAULT event: ");
        printf("flags = %llx; ", msg.arg.pagefault.flags);
        printf("address = %llx\n", msg.arg.pagefault.address);
        /* Copy the page pointed to by 'page' into the faulting
           region. Vary the contents that are copied in, so that it
           is more obvious that each fault is handled separately. */
        memset(page, 'A' + fault_cnt % 20, page_size);
        fault_cnt++;
        uffdio_copy.src = (unsigned long) page;
        /* We need to handle page faults in units of pages(!).
           So, round faulting address down to page boundary */
        uffdio_copy.dst = (unsigned long) msg.arg.pagefault.address &
                                           ~(page_size - 1);
        uffdio_copy.len = page_size;
        uffdio_copy.mode = 0;
        uffdio_copy.copy = 0;
        if (ioctl(uffd, UFFDIO_COPY, &uffdio_copy) == -1)
            errExit("ioctl-UFFDIO_COPY");
        printf("        (uffdio_copy.copy returned %lld)\n",
                uffdio_copy.copy);
    }
}

void __init_userfaultfd(unsigned long int numPages)
{
  long uffd;          /* userfaultfd file descriptor */
  char *addr;         /* Start of region handled by userfaultfd */
  unsigned long len;  /* Length of region handled by userfaultfd */
  pthread_t thr;      /* ID of thread that handles page faults */
  struct uffdio_api uffdio_api;
  struct uffdio_register uffdio_register;
  int s;
  int page_size = sysconf(_SC_PAGE_SIZE);
  len = numPages * page_size;
  /* Create and enable userfaultfd object */
  uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);

  if (uffd == -1)
      errExit("userfaultfd");
  uffdio_api.api = UFFD_API;
  uffdio_api.features = 0;
  if (ioctl(uffd, UFFDIO_API, &uffdio_api) == -1)
      errExit("ioctl-UFFDIO_API");

  s = pthread_create(&thr, NULL, fault_handler_thread, (void *) uffd);
  if (s != 0)
  {
    errno = s;
    errExit("pthread_create");
  }
}

void *__carmor_init_(unsigned long int numPages)
{
  __init_userfaultfd(numPages);
	VirtualMemory *vm = (VirtualMemory *)malloc(sizeof(VirtualMemory));
  SizeClassMap * scm = malloc(sizeof(SizeClassMap));
  init_SizeClassMap(3, 4, 8, 17, 128, 16, scm);
	return vm;
}

/*
void *__carmor_malloc_(unsigned long int size)
{
	int addr = 0;
	void* ptr = malloc(size);
	VirtualMemory_set(vm, ptr, 0, sizeof(int), 1, 1);
	//addr = mmap();
	printf("This is the changed malloc at address %d of size %d\n", addr, size);

	registerInTable();

	//shadow_addr = mmap()
	printf("Shadow mapped address is mapped at address %d of size %d\n", addr, size);
	return malloc(size);
}
*/

void *__carmor_malloc_(unsigned long int size, VirtualMemory *vm)
{
	int addr = 0;
	void* ptr = malloc(size);
	VirtualMemory_set(vm, ptr, 0, sizeof(int), 1, 1);
	//addr = mmap();
	printf("This is the changed malloc at address %d of size %lu\n", addr, size);

	//registerInTable();

	//shadow_addr = mmap()
	printf("Shadow mapped address is mapped at address %d of size %lu\n", addr, size);
	return malloc(size);
}

void *__carmor_realloc_(void *ptr, size_t size)
{
  int addr = 0;
	printf("This is the changed realloc at address %d of size %lu\n", addr, size);
	return realloc(ptr, size);
}
