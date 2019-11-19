; ModuleID = 'llvm-link'
source_filename = "llvm-link"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.SizeClassMap = type { i64, i64, i64, i64, i64, i64, i64, i64 }
%struct.VirtualMemory = type { i32, %struct.ShadowMetadata, %struct.PointerCast }
%struct.ShadowMetadata = type { i8, [100 x %struct.ShadowMetadata_Entry] }
%struct.ShadowMetadata_Entry = type { i8, i8, i8, i16, i8 }
%struct.PointerCast = type { i8, [100 x %struct.ShieldObject] }
%struct.ShieldObject = type { %struct.VirtualPointer, i8* }
%struct.VirtualPointer = type { i32 }

@llvm.global.annotations = appending global [2 x { i8*, i8*, i8*, i32 }] [{ i8*, i8*, i8*, i32 } { i8* bitcast (i32 (i32)* @sub_one to i8*), i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.1, i32 0, i32 0), i32 1 }, { i8*, i8*, i8*, i32 } { i8* bitcast (i32 (i32)* @add_one to i8*), i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.1, i32 0, i32 0), i32 5 }], section "llvm.metadata"
@.str = private unnamed_addr constant [14 x i8] c"carmorsplit_1\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [37 x i8] c"../../outoftree/commenttool/split2.c\00", section "llvm.metadata"
@.str.2 = private unnamed_addr constant [14 x i8] c"carmorsplit_0\00", section "llvm.metadata"
@__carmor_shadow_memory_dynamic_address = common dso_local global i64 0, align 8
@g_total_mmaped = internal global i64 0, align 8
@.str.3 = private unnamed_addr constant [97 x i8] c"ReserveShadowMemoryRange failed while trying to map 0x%zx bytes. Perhaps you're using ulimit -v\0A\00", align 1
@.str.6 = private unnamed_addr constant [38 x i8] c"ERROR: failed to allocate (%zd bytes)\00", align 1
@.str.1.4 = private unnamed_addr constant [11 x i8] c"low shadow\00", align 1
@kHighMemEnd = internal global i64 140737488355327, align 8
@.str.2.5 = private unnamed_addr constant [12 x i8] c"high shadow\00", align 1
@.str.7 = private unnamed_addr constant [11 x i8] c"shadow gap\00", align 1
@.str.3.6 = private unnamed_addr constant [54 x i8] c"This is the changed malloc at address %d of size %lu\0A\00", align 1
@.str.4 = private unnamed_addr constant [59 x i8] c"Shadow mapped address is mapped at address %d of size %lu\0A\00", align 1
@.str.5 = private unnamed_addr constant [55 x i8] c"This is the changed realloc at address %d of size %lu\0A\00", align 1
@.str.8 = private unnamed_addr constant [20 x i8] c"New virtual ID: %i\0A\00", align 1
@.str.1.9 = private unnamed_addr constant [26 x i8] c"No virtual ID available.\0A\00", align 1
@.str.2.12 = private unnamed_addr constant [33 x i8] c"Maximum memory entries reached.\0A\00", align 1
@.str.3.13 = private unnamed_addr constant [34 x i8] c"===Allocating Virtual Pointer===\0A\00", align 1
@.str.4.14 = private unnamed_addr constant [29 x i8] c"===Shadow Metadata Entry===\0A\00", align 1
@.str.5.15 = private unnamed_addr constant [11 x i8] c"Count: %i\0A\00", align 1
@.str.6.16 = private unnamed_addr constant [9 x i8] c"Mod: %u\0A\00", align 1
@.str.7.17 = private unnamed_addr constant [9 x i8] c"Sec: %u\0A\00", align 1
@.str.8.18 = private unnamed_addr constant [10 x i8] c"Type: %u\0A\00", align 1
@.str.9 = private unnamed_addr constant [11 x i8] c"Valid: %u\0A\00", align 1
@.str.10 = private unnamed_addr constant [11 x i8] c"Size: %hu\0A\00", align 1
@.str.11 = private unnamed_addr constant [42 x i8] c"===Assigning Value to Virtual Pointer===\0A\00", align 1
@.str.12 = private unnamed_addr constant [37 x i8] c"===Dereferencing Virtual Pointer===\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @sub_one(i32) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = sub nsw i32 %3, 1
  ret i32 %4
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add_one(i32) #1 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = add nsw i32 %3, 1
  ret i32 %4
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #2 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 4, i32* %1, align 4
  store i32 5, i32* %2, align 4
  %5 = load i32, i32* %1, align 4
  %6 = call i32 @add_one(i32 %5)
  store i32 %6, i32* %3, align 4
  %7 = load i32, i32* %2, align 4
  %8 = call i32 @sub_one(i32 %7)
  store i32 %8, i32* %4, align 4
  ret i32 0
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @IncreaseTotalMmap(i64) #2 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = load i64, i64* @g_total_mmaped, align 8
  %5 = add i64 %4, %3
  store i64 %5, i64* @g_total_mmaped, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @DecreaseTotalMmap(i64) #2 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = load i64, i64* @g_total_mmaped, align 8
  %5 = sub i64 %4, %3
  store i64 %5, i64* @g_total_mmaped, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local zeroext i1 @internal_iserror(i64, i32*) #2 {
  %3 = alloca i1, align 1
  %4 = alloca i64, align 8
  %5 = alloca i32*, align 8
  store i64 %0, i64* %4, align 8
  store i32* %1, i32** %5, align 8
  %6 = load i64, i64* %4, align 8
  %7 = icmp eq i64 %6, -1
  br i1 %7, label %8, label %9

8:                                                ; preds = %2
  store i1 true, i1* %3, align 1
  br label %10

9:                                                ; preds = %2
  store i1 false, i1* %3, align 1
  br label %10

10:                                               ; preds = %9, %8
  %11 = load i1, i1* %3, align 1
  ret i1 %11
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @MmapNamed(i8*, i64, i32, i32, i8*) #2 {
  %6 = alloca i8*, align 8
  %7 = alloca i64, align 8
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i8*, align 8
  store i8* %0, i8** %6, align 8
  store i64 %1, i64* %7, align 8
  store i32 %2, i32* %8, align 4
  store i32 %3, i32* %9, align 4
  store i8* %4, i8** %10, align 8
  %11 = load i8*, i8** %6, align 8
  %12 = load i64, i64* %7, align 8
  %13 = load i32, i32* %8, align 4
  %14 = load i32, i32* %9, align 4
  %15 = load i32, i32* %9, align 4
  %16 = call i8* @mmap(i8* %11, i64 %12, i32 %13, i32 %14, i32 %15, i64 0) #7
  %17 = ptrtoint i8* %16 to i64
  ret i64 %17
}

; Function Attrs: nounwind
declare dso_local i8* @mmap(i8*, i64, i32, i32, i32, i64) #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @ReserveShadowMemoryRange(i64, i64, i8*) #2 {
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i8*, align 8
  %7 = alloca i64, align 8
  store i64 %0, i64* %4, align 8
  store i64 %1, i64* %5, align 8
  store i8* %2, i8** %6, align 8
  %8 = load i64, i64* %5, align 8
  %9 = load i64, i64* %4, align 8
  %10 = sub i64 %8, %9
  %11 = add i64 %10, 1
  store i64 %11, i64* %7, align 8
  %12 = load i64, i64* %7, align 8
  call void @DecreaseTotalMmap(i64 %12)
  %13 = load i64, i64* %4, align 8
  %14 = load i64, i64* %7, align 8
  %15 = load i8*, i8** %6, align 8
  %16 = call i64 @MmapFixed(i64 %13, i64 %14, i32 16384, i8* %15)
  %17 = icmp ne i64 %16, 0
  br i1 %17, label %21, label %18

18:                                               ; preds = %3
  %19 = load i64, i64* %7, align 8
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([97 x i8], [97 x i8]* @.str.3, i64 0, i64 0), i64 %19)
  call void @exit(i32 -1) #8
  unreachable

21:                                               ; preds = %3
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define internal i64 @MmapFixed(i64, i64, i32, i8*) #2 {
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i32, align 4
  %8 = alloca i8*, align 8
  %9 = alloca i64, align 8
  %10 = alloca i32, align 4
  store i64 %0, i64* %5, align 8
  store i64 %1, i64* %6, align 8
  store i32 %2, i32* %7, align 4
  store i8* %3, i8** %8, align 8
  %11 = load i64, i64* %6, align 8
  %12 = call i64 @RoundUpTo(i64 %11, i64 4096)
  store i64 %12, i64* %6, align 8
  %13 = load i64, i64* %5, align 8
  %14 = call i64 @RoundDownTo(i64 %13, i64 4096)
  store i64 %14, i64* %5, align 8
  %15 = load i64, i64* %5, align 8
  %16 = inttoptr i64 %15 to i8*
  %17 = load i64, i64* %6, align 8
  %18 = load i32, i32* %7, align 4
  %19 = or i32 18, %18
  %20 = or i32 %19, 32
  %21 = load i8*, i8** %8, align 8
  %22 = call i64 @MmapNamed(i8* %16, i64 %17, i32 3, i32 %20, i8* %21)
  store i64 %22, i64* %9, align 8
  %23 = load i64, i64* %9, align 8
  %24 = call zeroext i1 @internal_iserror(i64 %23, i32* %10)
  br i1 %24, label %25, label %28

25:                                               ; preds = %4
  %26 = load i64, i64* %6, align 8
  %27 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.6, i64 0, i64 0), i64 %26)
  br label %28

28:                                               ; preds = %25, %4
  %29 = load i64, i64* %6, align 8
  call void @IncreaseTotalMmap(i64 %29)
  %30 = load i64, i64* %9, align 8
  ret i64 %30
}

declare dso_local i32 @printf(i8*, ...) #4

; Function Attrs: noreturn nounwind
declare dso_local void @exit(i32) #5

; Function Attrs: noinline nounwind optnone uwtable
define internal i64 @RoundUpTo(i64, i64) #2 {
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  store i64 %0, i64* %3, align 8
  store i64 %1, i64* %4, align 8
  %5 = load i64, i64* %3, align 8
  %6 = load i64, i64* %4, align 8
  %7 = add i64 %5, %6
  %8 = sub i64 %7, 1
  %9 = load i64, i64* %4, align 8
  %10 = sub i64 %9, 1
  %11 = xor i64 %10, -1
  %12 = and i64 %8, %11
  ret i64 %12
}

; Function Attrs: noinline nounwind optnone uwtable
define internal i64 @RoundDownTo(i64, i64) #2 {
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  store i64 %0, i64* %3, align 8
  store i64 %1, i64* %4, align 8
  %5 = load i64, i64* %3, align 8
  %6 = load i64, i64* %4, align 8
  %7 = sub i64 %6, 1
  %8 = xor i64 %7, -1
  %9 = and i64 %5, %8
  ret i64 %9
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @GetMaxVirtualAddresses() #2 {
  ret i64 140737488355327
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @InitializeShadowMemory() #2 {
  %1 = alloca i64, align 8
  store i64 -1, i64* @__carmor_shadow_memory_dynamic_address, align 8
  store i64 536870912, i64* %1, align 8
  %2 = load i64, i64* %1, align 8
  call void @ReserveShadowMemoryRange(i64 %2, i64 16777245, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.1.4, i64 0, i64 0))
  %3 = load i64, i64* @kHighMemEnd, align 8
  %4 = shl i64 %3, 12
  %5 = lshr i64 %4, 17
  %6 = add i64 %5, 29
  %7 = add i64 %6, 1
  %8 = shl i64 %7, 12
  %9 = lshr i64 %8, 17
  %10 = add i64 %9, 29
  %11 = load i64, i64* @kHighMemEnd, align 8
  %12 = shl i64 %11, 12
  %13 = lshr i64 %12, 17
  %14 = add i64 %13, 29
  call void @ReserveShadowMemoryRange(i64 %10, i64 %14, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.2.5, i64 0, i64 0))
  call void @ProtectGap(i64 16777245, i64 6425673730)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define internal void @ProtectGap(i64, i64) #2 {
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  store i64 %0, i64* %3, align 8
  store i64 %1, i64* %4, align 8
  %6 = load i64, i64* %3, align 8
  %7 = load i64, i64* %4, align 8
  %8 = call i64 @MmapFixed(i64 %6, i64 %7, i32 16434, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.7, i64 0, i64 0))
  store i64 %8, i64* %5, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @_BitScanReverse(i32) #2 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = sub nsw i32 63, %3
  ret i32 %4
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @MostSignificantSetBitIndex(i64) #2 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = trunc i64 %3 to i32
  %5 = call i32 @_BitScanReverse(i32 %4)
  %6 = sext i32 %5 to i64
  ret i64 %6
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @getSizeClass(%struct.SizeClassMap*, i8*) #2 {
  %3 = alloca %struct.SizeClassMap*, align 8
  %4 = alloca i8*, align 8
  store %struct.SizeClassMap* %0, %struct.SizeClassMap** %3, align 8
  store i8* %1, i8** %4, align 8
  %5 = load i8*, i8** %4, align 8
  %6 = ptrtoint i8* %5 to i64
  %7 = sub i64 %6, 105553116266496
  %8 = load %struct.SizeClassMap*, %struct.SizeClassMap** %3, align 8
  %9 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %8, i32 0, i32 7
  %10 = load i64, i64* %9, align 8
  %11 = udiv i64 %7, %10
  %12 = urem i64 %11, 5
  ret i64 %12
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @__carmor_init_(i64) #2 {
  %2 = alloca i64, align 8
  %3 = alloca %struct.VirtualMemory*, align 8
  %4 = alloca %struct.SizeClassMap*, align 8
  store i64 %0, i64* %2, align 8
  %5 = call noalias i8* @malloc(i64 2416) #7
  %6 = bitcast i8* %5 to %struct.VirtualMemory*
  store %struct.VirtualMemory* %6, %struct.VirtualMemory** %3, align 8
  %7 = call noalias i8* @malloc(i64 64) #7
  %8 = bitcast i8* %7 to %struct.SizeClassMap*
  store %struct.SizeClassMap* %8, %struct.SizeClassMap** %4, align 8
  %9 = load %struct.SizeClassMap*, %struct.SizeClassMap** %4, align 8
  call void @init_SizeClassMap(i64 3, i64 4, i64 8, i64 17, i64 128, i64 16, %struct.SizeClassMap* %9)
  %10 = load %struct.VirtualMemory*, %struct.VirtualMemory** %3, align 8
  %11 = bitcast %struct.VirtualMemory* %10 to i8*
  ret i8* %11
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #3

; Function Attrs: noinline nounwind optnone uwtable
define internal void @init_SizeClassMap(i64, i64, i64, i64, i64, i64, %struct.SizeClassMap*) #2 {
  %8 = alloca i64, align 8
  %9 = alloca i64, align 8
  %10 = alloca i64, align 8
  %11 = alloca i64, align 8
  %12 = alloca i64, align 8
  %13 = alloca i64, align 8
  %14 = alloca %struct.SizeClassMap*, align 8
  store i64 %0, i64* %8, align 8
  store i64 %1, i64* %9, align 8
  store i64 %2, i64* %10, align 8
  store i64 %3, i64* %11, align 8
  store i64 %4, i64* %12, align 8
  store i64 %5, i64* %13, align 8
  store %struct.SizeClassMap* %6, %struct.SizeClassMap** %14, align 8
  %15 = load i64, i64* %10, align 8
  %16 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %17 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %16, i32 0, i32 3
  store i64 %15, i64* %17, align 8
  %18 = load i64, i64* %13, align 8
  %19 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %20 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %19, i32 0, i32 4
  store i64 %18, i64* %20, align 8
  %21 = load i64, i64* %9, align 8
  %22 = trunc i64 %21 to i32
  %23 = shl i32 1, %22
  %24 = sext i32 %23 to i64
  %25 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %26 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %25, i32 0, i32 0
  store i64 %24, i64* %26, align 8
  %27 = load i64, i64* %10, align 8
  %28 = trunc i64 %27 to i32
  %29 = shl i32 1, %28
  %30 = sext i32 %29 to i64
  %31 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %32 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %31, i32 0, i32 1
  store i64 %30, i64* %32, align 8
  %33 = load i64, i64* %10, align 8
  %34 = load i64, i64* %9, align 8
  %35 = udiv i64 %33, %34
  %36 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %37 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %36, i32 0, i32 2
  store i64 %35, i64* %37, align 8
  %38 = load i64, i64* %8, align 8
  %39 = sub i64 %38, 1
  %40 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %41 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %40, i32 0, i32 5
  store i64 %39, i64* %41, align 8
  %42 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %43 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %42, i32 0, i32 5
  %44 = load i64, i64* %43, align 8
  %45 = trunc i64 %44 to i32
  %46 = shl i32 1, %45
  %47 = sub nsw i32 %46, 1
  %48 = sext i32 %47 to i64
  %49 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %50 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %49, i32 0, i32 6
  store i64 %48, i64* %50, align 8
  %51 = load i64, i64* %9, align 8
  %52 = load i64, i64* %8, align 8
  %53 = udiv i64 %51, %52
  %54 = load %struct.SizeClassMap*, %struct.SizeClassMap** %14, align 8
  %55 = getelementptr inbounds %struct.SizeClassMap, %struct.SizeClassMap* %54, i32 0, i32 7
  store i64 %53, i64* %55, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @__carmor_malloc_(i64, %struct.VirtualMemory*) #2 {
  %3 = alloca i64, align 8
  %4 = alloca %struct.VirtualMemory*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i8*, align 8
  %7 = alloca %struct.VirtualPointer, align 4
  store i64 %0, i64* %3, align 8
  store %struct.VirtualMemory* %1, %struct.VirtualMemory** %4, align 8
  store i32 0, i32* %5, align 4
  %8 = load i64, i64* %3, align 8
  %9 = call noalias i8* @malloc(i64 %8) #7
  store i8* %9, i8** %6, align 8
  %10 = load %struct.VirtualMemory*, %struct.VirtualMemory** %4, align 8
  %11 = load i8*, i8** %6, align 8
  %12 = call i32 @VirtualMemory_set(%struct.VirtualMemory* %10, i8* %11, i8 zeroext 0, i8 zeroext 4, i16 zeroext 1, i8 zeroext 1)
  %13 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %7, i32 0, i32 0
  store i32 %12, i32* %13, align 4
  %14 = load i32, i32* %5, align 4
  %15 = load i64, i64* %3, align 8
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([54 x i8], [54 x i8]* @.str.3.6, i64 0, i64 0), i32 %14, i64 %15)
  %17 = load i32, i32* %5, align 4
  %18 = load i64, i64* %3, align 8
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([59 x i8], [59 x i8]* @.str.4, i64 0, i64 0), i32 %17, i64 %18)
  %20 = load i64, i64* %3, align 8
  %21 = call noalias i8* @malloc(i64 %20) #7
  ret i8* %21
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @__carmor_realloc_(i8*, i64) #2 {
  %3 = alloca i8*, align 8
  %4 = alloca i64, align 8
  %5 = alloca i32, align 4
  store i8* %0, i8** %3, align 8
  store i64 %1, i64* %4, align 8
  store i32 0, i32* %5, align 4
  %6 = load i32, i32* %5, align 4
  %7 = load i64, i64* %4, align 8
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([55 x i8], [55 x i8]* @.str.5, i64 0, i64 0), i32 %6, i64 %7)
  %9 = load i8*, i8** %3, align 8
  %10 = load i64, i64* %4, align 8
  %11 = call i8* @realloc(i8* %9, i64 %10) #7
  ret i8* %11
}

; Function Attrs: nounwind
declare dso_local i8* @realloc(i8*, i64) #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @getAvailableId(%struct.VirtualMemory*) #2 {
  %2 = alloca i32, align 4
  %3 = alloca %struct.VirtualMemory*, align 8
  %4 = alloca i32, align 4
  store %struct.VirtualMemory* %0, %struct.VirtualMemory** %3, align 8
  store i32 0, i32* %4, align 4
  br label %5

5:                                                ; preds = %24, %1
  %6 = load i32, i32* %4, align 4
  %7 = icmp slt i32 %6, 100
  br i1 %7, label %8, label %27

8:                                                ; preds = %5
  %9 = load %struct.VirtualMemory*, %struct.VirtualMemory** %3, align 8
  %10 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %9, i32 0, i32 1
  %11 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %10, i32 0, i32 1
  %12 = load i32, i32* %4, align 4
  %13 = sext i32 %12 to i64
  %14 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %11, i64 0, i64 %13
  %15 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %14, i32 0, i32 0
  %16 = load i8, i8* %15, align 2
  %17 = zext i8 %16 to i32
  %18 = icmp ne i32 %17, 1
  br i1 %18, label %19, label %23

19:                                               ; preds = %8
  %20 = load i32, i32* %4, align 4
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @.str.8, i64 0, i64 0), i32 %20)
  %22 = load i32, i32* %4, align 4
  store i32 %22, i32* %2, align 4
  br label %29

23:                                               ; preds = %8
  br label %24

24:                                               ; preds = %23
  %25 = load i32, i32* %4, align 4
  %26 = add nsw i32 %25, 1
  store i32 %26, i32* %4, align 4
  br label %5

27:                                               ; preds = %5
  %28 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.1.9, i64 0, i64 0))
  store i32 -1, i32* %2, align 4
  br label %29

29:                                               ; preds = %27, %19
  %30 = load i32, i32* %2, align 4
  ret i32 %30
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @VirtualMemory_set(%struct.VirtualMemory*, i8*, i8 zeroext, i8 zeroext, i16 zeroext, i8 zeroext) #2 {
  %7 = alloca %struct.VirtualPointer, align 4
  %8 = alloca %struct.VirtualMemory*, align 8
  %9 = alloca i8*, align 8
  %10 = alloca i8, align 1
  %11 = alloca i8, align 1
  %12 = alloca i16, align 2
  %13 = alloca i8, align 1
  %14 = alloca i32, align 4
  %15 = alloca %struct.ShieldObject, align 8
  %16 = alloca %struct.ShadowMetadata_Entry, align 2
  store %struct.VirtualMemory* %0, %struct.VirtualMemory** %8, align 8
  store i8* %1, i8** %9, align 8
  store i8 %2, i8* %10, align 1
  store i8 %3, i8* %11, align 1
  store i16 %4, i16* %12, align 2
  store i8 %5, i8* %13, align 1
  %17 = load %struct.VirtualMemory*, %struct.VirtualMemory** %8, align 8
  %18 = call i32 @getAvailableId(%struct.VirtualMemory* %17)
  store i32 %18, i32* %14, align 4
  %19 = load i32, i32* %14, align 4
  %20 = icmp eq i32 %19, -1
  br i1 %20, label %21, label %23

21:                                               ; preds = %6
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([33 x i8], [33 x i8]* @.str.2.12, i64 0, i64 0))
  call void @exit(i32 0) #8
  unreachable

23:                                               ; preds = %6
  %24 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %7, i32 0, i32 0
  %25 = load i32, i32* %14, align 4
  store i32 %25, i32* %24, align 4
  %26 = getelementptr inbounds %struct.ShieldObject, %struct.ShieldObject* %15, i32 0, i32 0
  %27 = bitcast %struct.VirtualPointer* %26 to i8*
  %28 = bitcast %struct.VirtualPointer* %7 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 8 %27, i8* align 4 %28, i64 4, i1 false)
  %29 = getelementptr inbounds %struct.ShieldObject, %struct.ShieldObject* %15, i32 0, i32 1
  %30 = load i8*, i8** %9, align 8
  store i8* %30, i8** %29, align 8
  %31 = load %struct.VirtualMemory*, %struct.VirtualMemory** %8, align 8
  %32 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %31, i32 0, i32 2
  %33 = getelementptr inbounds %struct.PointerCast, %struct.PointerCast* %32, i32 0, i32 1
  %34 = load i32, i32* %14, align 4
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds [100 x %struct.ShieldObject], [100 x %struct.ShieldObject]* %33, i64 0, i64 %35
  %37 = bitcast %struct.ShieldObject* %36 to i8*
  %38 = bitcast %struct.ShieldObject* %15 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 8 %37, i8* align 8 %38, i64 16, i1 false)
  %39 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %16, i32 0, i32 0
  store i8 1, i8* %39, align 2
  %40 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %16, i32 0, i32 1
  %41 = load i8, i8* %10, align 1
  store i8 %41, i8* %40, align 1
  %42 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %16, i32 0, i32 2
  %43 = load i8, i8* %11, align 1
  store i8 %43, i8* %42, align 2
  %44 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %16, i32 0, i32 3
  %45 = load i16, i16* %12, align 2
  store i16 %45, i16* %44, align 2
  %46 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %16, i32 0, i32 4
  %47 = load i8, i8* %13, align 1
  store i8 %47, i8* %46, align 2
  %48 = load %struct.VirtualMemory*, %struct.VirtualMemory** %8, align 8
  %49 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %48, i32 0, i32 1
  %50 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %49, i32 0, i32 1
  %51 = load i32, i32* %14, align 4
  %52 = sext i32 %51 to i64
  %53 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %50, i64 0, i64 %52
  %54 = bitcast %struct.ShadowMetadata_Entry* %53 to i8*
  %55 = bitcast %struct.ShadowMetadata_Entry* %16 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 2 %54, i8* align 2 %55, i64 8, i1 false)
  %56 = load %struct.VirtualMemory*, %struct.VirtualMemory** %8, align 8
  %57 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %56, i32 0, i32 1
  %58 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %57, i32 0, i32 0
  %59 = load i8, i8* %58, align 4
  %60 = add i8 %59, 1
  store i8 %60, i8* %58, align 4
  %61 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %7, i32 0, i32 0
  %62 = load i32, i32* %61, align 4
  ret i32 %62
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1 immarg) #6

; Function Attrs: noinline nounwind optnone uwtable
define dso_local { i32, i8* } @VirtualMemory_get(%struct.VirtualMemory*, i32) #2 {
  %3 = alloca %struct.ShieldObject, align 8
  %4 = alloca %struct.VirtualPointer, align 4
  %5 = alloca %struct.VirtualMemory*, align 8
  %6 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %4, i32 0, i32 0
  store i32 %1, i32* %6, align 4
  store %struct.VirtualMemory* %0, %struct.VirtualMemory** %5, align 8
  %7 = load %struct.VirtualMemory*, %struct.VirtualMemory** %5, align 8
  %8 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %7, i32 0, i32 2
  %9 = getelementptr inbounds %struct.PointerCast, %struct.PointerCast* %8, i32 0, i32 1
  %10 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %4, i32 0, i32 0
  %11 = load i32, i32* %10, align 4
  %12 = sext i32 %11 to i64
  %13 = getelementptr inbounds [100 x %struct.ShieldObject], [100 x %struct.ShieldObject]* %9, i64 0, i64 %12
  %14 = bitcast %struct.ShieldObject* %3 to i8*
  %15 = bitcast %struct.ShieldObject* %13 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 8 %14, i8* align 8 %15, i64 16, i1 false)
  %16 = bitcast %struct.ShieldObject* %3 to { i32, i8* }*
  %17 = load { i32, i8* }, { i32, i8* }* %16, align 8
  ret { i32, i8* } %17
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @__shield_malloc__(%struct.VirtualMemory*, i64, i8 zeroext, i8 zeroext) #2 {
  %5 = alloca %struct.VirtualPointer, align 4
  %6 = alloca %struct.VirtualMemory*, align 8
  %7 = alloca i64, align 8
  %8 = alloca i8, align 1
  %9 = alloca i8, align 1
  %10 = alloca i8*, align 8
  store %struct.VirtualMemory* %0, %struct.VirtualMemory** %6, align 8
  store i64 %1, i64* %7, align 8
  store i8 %2, i8* %8, align 1
  store i8 %3, i8* %9, align 1
  %11 = load i64, i64* %7, align 8
  %12 = call noalias i8* @malloc(i64 %11) #7
  store i8* %12, i8** %10, align 8
  %13 = load %struct.VirtualMemory*, %struct.VirtualMemory** %6, align 8
  %14 = load i8*, i8** %10, align 8
  %15 = load i8, i8* %8, align 1
  %16 = load i64, i64* %7, align 8
  %17 = trunc i64 %16 to i16
  %18 = load i8, i8* %9, align 1
  %19 = call i32 @VirtualMemory_set(%struct.VirtualMemory* %13, i8* %14, i8 zeroext 0, i8 zeroext %15, i16 zeroext %17, i8 zeroext %18)
  %20 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %5, i32 0, i32 0
  store i32 %19, i32* %20, align 4
  %21 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %5, i32 0, i32 0
  %22 = load i32, i32* %21, align 4
  ret i32 %22
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @test() #2 {
  %1 = alloca %struct.VirtualMemory*, align 8
  %2 = alloca %struct.VirtualPointer, align 4
  %3 = call noalias i8* @malloc(i64 2416) #7
  %4 = bitcast i8* %3 to %struct.VirtualMemory*
  store %struct.VirtualMemory* %4, %struct.VirtualMemory** %1, align 8
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @.str.3.13, i64 0, i64 0))
  %6 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %7 = call i32 @__shield_malloc__(%struct.VirtualMemory* %6, i64 4, i8 zeroext 1, i8 zeroext 1)
  %8 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %2, i32 0, i32 0
  store i32 %7, i32* %8, align 4
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.4.14, i64 0, i64 0))
  %10 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %11 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %10, i32 0, i32 1
  %12 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %11, i32 0, i32 0
  %13 = load i8, i8* %12, align 4
  %14 = zext i8 %13 to i32
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.5.15, i64 0, i64 0), i32 %14)
  %16 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %17 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %16, i32 0, i32 1
  %18 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %17, i32 0, i32 1
  %19 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %2, i32 0, i32 0
  %20 = load i32, i32* %19, align 4
  %21 = sext i32 %20 to i64
  %22 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %18, i64 0, i64 %21
  %23 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %22, i32 0, i32 1
  %24 = load i8, i8* %23, align 1
  %25 = zext i8 %24 to i32
  %26 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.6.16, i64 0, i64 0), i32 %25)
  %27 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %28 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %27, i32 0, i32 1
  %29 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %28, i32 0, i32 1
  %30 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %2, i32 0, i32 0
  %31 = load i32, i32* %30, align 4
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %29, i64 0, i64 %32
  %34 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %33, i32 0, i32 2
  %35 = load i8, i8* %34, align 2
  %36 = zext i8 %35 to i32
  %37 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.7.17, i64 0, i64 0), i32 %36)
  %38 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %39 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %38, i32 0, i32 1
  %40 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %39, i32 0, i32 1
  %41 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %2, i32 0, i32 0
  %42 = load i32, i32* %41, align 4
  %43 = sext i32 %42 to i64
  %44 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %40, i64 0, i64 %43
  %45 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %44, i32 0, i32 4
  %46 = load i8, i8* %45, align 2
  %47 = zext i8 %46 to i32
  %48 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.8.18, i64 0, i64 0), i32 %47)
  %49 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %50 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %49, i32 0, i32 1
  %51 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %50, i32 0, i32 1
  %52 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %2, i32 0, i32 0
  %53 = load i32, i32* %52, align 4
  %54 = sext i32 %53 to i64
  %55 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %51, i64 0, i64 %54
  %56 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %55, i32 0, i32 0
  %57 = load i8, i8* %56, align 2
  %58 = zext i8 %57 to i32
  %59 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.9, i64 0, i64 0), i32 %58)
  %60 = load %struct.VirtualMemory*, %struct.VirtualMemory** %1, align 8
  %61 = getelementptr inbounds %struct.VirtualMemory, %struct.VirtualMemory* %60, i32 0, i32 1
  %62 = getelementptr inbounds %struct.ShadowMetadata, %struct.ShadowMetadata* %61, i32 0, i32 1
  %63 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %2, i32 0, i32 0
  %64 = load i32, i32* %63, align 4
  %65 = sext i32 %64 to i64
  %66 = getelementptr inbounds [100 x %struct.ShadowMetadata_Entry], [100 x %struct.ShadowMetadata_Entry]* %62, i64 0, i64 %65
  %67 = getelementptr inbounds %struct.ShadowMetadata_Entry, %struct.ShadowMetadata_Entry* %66, i32 0, i32 3
  %68 = load i16, i16* %67, align 2
  %69 = zext i16 %68 to i32
  %70 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.10, i64 0, i64 0), i32 %69)
  %71 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([42 x i8], [42 x i8]* @.str.11, i64 0, i64 0))
  %72 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.12, i64 0, i64 0))
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "carmorsplit_1" "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline nounwind optnone uwtable "carmorsplit_0" "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { argmemonly nounwind }
attributes #7 = { nounwind }
attributes #8 = { noreturn nounwind }

!llvm.ident = !{!0, !0, !0}
!llvm.module.flags = !{!1}

!0 = !{!"clang version 9.0.1-svn374861-1~exp1~20191015075537.63 (branches/release_90)"}
!1 = !{i32 1, !"wchar_size", i32 4}
