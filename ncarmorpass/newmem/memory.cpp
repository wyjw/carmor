// Derived heavily from ASAN runtime
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//

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

typedef unsigned long uptr;

static const uptr PAGE_SIZE 4096;
static const u64 defaultShadowOffset = 1ULL << 29;
// global interface symbol
uptr __carmor_shadow_memory_dynamic_address;

//the total amount to be mapped
static uptr g_total_mmaped;
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

static bool MmapFixed(uptr fixed_addr, uptr size, int additional_flags, const char * name){
  size = RoundUpTo(size, PAGE_SIZE);
  fixed_addr = RoundDownTo(fixed_addr, PAGE_SIZE);
  uptr p = mmap((void *) fixed_addr, size, PROT_READ | PROT_WRITE,
  MAP_PRIVATE | MAP_FIXED | additional_flags | MAP_ANON, name);

  int reserrno;
  if (internal_iserror(p, &reserrno))
  {
    Report("ERROR: failed to allocate (%zd bytes)", size);
  }
  IncreaseTotalMmap(size);
  return true;
}

void ReserveShadowMemoryRange(uptr beg, uptr end, const char *name) {
  uptr size = end - beg + 1;
  DecreaseTotalMmap(size);  // Don't count the shadow against mmap_limit_mb.
  if (!MmapFixed(beg, size, MAP_NORESERVE, name)) {
    std::printf(
        "ReserveShadowMemoryRange failed while trying to map 0x%zx bytes. "
        "Perhaps you're using ulimit -v\n",
        size);
    exit(-1);
  }
}

#define MEM_TO_SHADOW(mem) (((mem) >> SHADOW_SCALE) + SHADOW_OFFSET))
static uptr kLowMemBeg = 0;
static uptr kLowMemEnd = defaultShadowOffset - 1;

static uptr kLowShadowBeg = defaultShadowOffset;
static uptr kLowShadowEnd = MEM_TO_SHADOW(kLowMemEnd) + 1

static uptr kHighMemBeg = MEM_TO_SHADOW(kHighMemEnd) + 1
static uptr kHighShadowBeg = MEM_TO_SHADOW(kHighMemBeg)
static uptr kHighShadowEnd  MEM_TO_SHADOW(kHighMemEnd)

static uptr kMidShadowBeg = MEM_TO_SHADOW(kMidMemBeg)
static uptr kMidShadowEnd = MEM_TO_SHADOW(kMidMemEnd)

static uptr kShadowGapBeg = kLowShadowEnd;
static uptr kShadowGapEnd = kMidShadowBeg - 1;

#define ShadowScale 5

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
  void *res = MmapFixedNoAccess(addr, size,
    PROT_NONE, MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE | MAP_ANON, "shadow gap");
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
struct ChunkHeader {
  u32 chunk_state : 8;
  u32 alloc_tid : 24;
  u32 free_tid : 24;
  u32 from_memalign : 1;
  u32 alloc_type : 2;
  u32 rz_log : 3;
  u32 lsan_tag : 2;
};

struct ChunkBase : ChunkHeader {
  u32 free_context_id;
};

struct CarmorChunk : ChunkBase {
  uptr Beg() { return reinterpret_cast<uptr>(this) + kChunkHeaderSize; }
  /*
  uptr UsedSize() {
    //TODO
    return *reinterpret_cast<uptr *>(
      get_allocator().GetMetaData()
    );
  }
  */
}

int _BitScanReverse(int x)
{
  return 63 - x;
}

uptr MostSignificantSetBitIndex(uptr x) {
  _BitScanReverse()
}

static const uptr kSpaceBeg = 0x600000000000ULL;
static const uptr kSpaceSize = 0x40000000000ULL;
static const uptr kMetadataSize = 0;

template <uptr kNumBits, uptr kMinSizeLog, uptr kMidSizeLog, uptr kMaxSizeLog,
          uptr kMaxNumCachedHintT, uptr kMaxBytesCachedLog>
class SizeClassMap {
  static const uptr kMinSize = 1 << kMinSizeLog;
  static const uptr kMidSize = 1 << kMidSizeLog;
  static const uptr kMidClass = kMidSize / kMinSize;
  static const uptr S = kNumBits - 1;
  static const uptr M = (1 << S) - 1;

  static ptr Size(uptr class_id) {
    class_id -= kMidClass;
    uptr t = kMidSize << (class_id >> S);
    return t + (t >> S) * (class_id & M);
  }

  static uptr ClassId(uptr size) {
    const uptr l = MostSignificantSetBitIndex(size);
    const uptr hbits = (size >> (l - S)) & M;
    const uptr lbits = size & ((1U << (1 - S)) - 1);
    const uptr l1 = 1 - kMidSizeLog;
    return kMidClass + (l1 << S) + hbits + (lbits > 0);
  }

  static uptr MaxCachedHint(uptr size){
    return n = (1UL << kMaxBytesCachedLog) / size;
  }
};

typedef SizeClassMap<3, 4, 8, 17, 128, 16> DefaultSizeClassMap;

uptr getSizeClass(const void *p)
{
  return ((reinterpret_cast<uptr>(p) - SpaceBeg()) / kRegionSize) % kNumClassesRounded;
}


void __carmor_allocate() {

}

void __carmor_malloc() {

}
