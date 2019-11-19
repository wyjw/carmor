; ModuleID = '../lib/mem.c'
source_filename = "../lib/mem.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.SizeClassMap = type { i64, i64, i64, i64, i64, i64, i64, i64 }
%struct.VirtualMemory = type { i32, %struct.ShadowMetadata, %struct.PointerCast }
%struct.ShadowMetadata = type { i8, [100 x %struct.ShadowMetadata_Entry] }
%struct.ShadowMetadata_Entry = type { i8, i8, i8, i16, i8 }
%struct.PointerCast = type { i8, [100 x %struct.ShieldObject] }
%struct.ShieldObject = type { %struct.VirtualPointer, i8* }
%struct.VirtualPointer = type { i32 }

@g_total_mmaped = internal global i64 0, align 8
@.str = private unnamed_addr constant [97 x i8] c"ReserveShadowMemoryRange failed while trying to map 0x%zx bytes. Perhaps you're using ulimit -v\0A\00", align 1
@__carmor_shadow_memory_dynamic_address = common dso_local global i64 0, align 8
@.str.1 = private unnamed_addr constant [11 x i8] c"low shadow\00", align 1
@kHighMemEnd = internal global i64 140737488355327, align 8
@.str.2 = private unnamed_addr constant [12 x i8] c"high shadow\00", align 1
@.str.3 = private unnamed_addr constant [54 x i8] c"This is the changed malloc at address %d of size %lu\0A\00", align 1
@.str.4 = private unnamed_addr constant [59 x i8] c"Shadow mapped address is mapped at address %d of size %lu\0A\00", align 1
@.str.5 = private unnamed_addr constant [55 x i8] c"This is the changed realloc at address %d of size %lu\0A\00", align 1
@.str.6 = private unnamed_addr constant [38 x i8] c"ERROR: failed to allocate (%zd bytes)\00", align 1
@.str.7 = private unnamed_addr constant [11 x i8] c"shadow gap\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @IncreaseTotalMmap(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = load i64, i64* @g_total_mmaped, align 8
  %5 = add i64 %4, %3
  store i64 %5, i64* @g_total_mmaped, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @DecreaseTotalMmap(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = load i64, i64* @g_total_mmaped, align 8
  %5 = sub i64 %4, %3
  store i64 %5, i64* @g_total_mmaped, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local zeroext i1 @internal_iserror(i64, i32*) #0 {
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
define dso_local i64 @MmapNamed(i8*, i64, i32, i32, i8*) #0 {
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
  %16 = call i8* @mmap(i8* %11, i64 %12, i32 %13, i32 %14, i32 %15, i64 0) #4
  %17 = ptrtoint i8* %16 to i64
  ret i64 %17
}

; Function Attrs: nounwind
declare dso_local i8* @mmap(i8*, i64, i32, i32, i32, i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @ReserveShadowMemoryRange(i64, i64, i8*) #0 {
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
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([97 x i8], [97 x i8]* @.str, i64 0, i64 0), i64 %19)
  call void @exit(i32 -1) #5
  unreachable

21:                                               ; preds = %3
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define internal i64 @MmapFixed(i64, i64, i32, i8*) #0 {
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

declare dso_local i32 @printf(i8*, ...) #2

; Function Attrs: noreturn nounwind
declare dso_local void @exit(i32) #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @GetMaxVirtualAddresses() #0 {
  ret i64 140737488355327
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @InitializeShadowMemory() #0 {
  %1 = alloca i64, align 8
  store i64 -1, i64* @__carmor_shadow_memory_dynamic_address, align 8
  store i64 536870912, i64* %1, align 8
  %2 = load i64, i64* %1, align 8
  call void @ReserveShadowMemoryRange(i64 %2, i64 16777245, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.1, i64 0, i64 0))
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
  call void @ReserveShadowMemoryRange(i64 %10, i64 %14, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.2, i64 0, i64 0))
  call void @ProtectGap(i64 16777245, i64 6425673730)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define internal void @ProtectGap(i64, i64) #0 {
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
define dso_local i32 @_BitScanReverse(i32) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = sub nsw i32 63, %3
  ret i32 %4
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @MostSignificantSetBitIndex(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = trunc i64 %3 to i32
  %5 = call i32 @_BitScanReverse(i32 %4)
  %6 = sext i32 %5 to i64
  ret i64 %6
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i64 @getSizeClass(%struct.SizeClassMap*, i8*) #0 {
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
define dso_local i8* @__carmor_init_(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca %struct.VirtualMemory*, align 8
  %4 = alloca %struct.SizeClassMap*, align 8
  store i64 %0, i64* %2, align 8
  %5 = call noalias i8* @malloc(i64 2416) #4
  %6 = bitcast i8* %5 to %struct.VirtualMemory*
  store %struct.VirtualMemory* %6, %struct.VirtualMemory** %3, align 8
  %7 = call noalias i8* @malloc(i64 64) #4
  %8 = bitcast i8* %7 to %struct.SizeClassMap*
  store %struct.SizeClassMap* %8, %struct.SizeClassMap** %4, align 8
  %9 = load %struct.SizeClassMap*, %struct.SizeClassMap** %4, align 8
  call void @init_SizeClassMap(i64 3, i64 4, i64 8, i64 17, i64 128, i64 16, %struct.SizeClassMap* %9)
  %10 = load %struct.VirtualMemory*, %struct.VirtualMemory** %3, align 8
  %11 = bitcast %struct.VirtualMemory* %10 to i8*
  ret i8* %11
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define internal void @init_SizeClassMap(i64, i64, i64, i64, i64, i64, %struct.SizeClassMap*) #0 {
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
define dso_local i8* @__carmor_malloc_(i64, %struct.VirtualMemory*) #0 {
  %3 = alloca i64, align 8
  %4 = alloca %struct.VirtualMemory*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i8*, align 8
  %7 = alloca %struct.VirtualPointer, align 4
  store i64 %0, i64* %3, align 8
  store %struct.VirtualMemory* %1, %struct.VirtualMemory** %4, align 8
  store i32 0, i32* %5, align 4
  %8 = load i64, i64* %3, align 8
  %9 = call noalias i8* @malloc(i64 %8) #4
  store i8* %9, i8** %6, align 8
  %10 = load %struct.VirtualMemory*, %struct.VirtualMemory** %4, align 8
  %11 = load i8*, i8** %6, align 8
  %12 = call i32 @VirtualMemory_set(%struct.VirtualMemory* %10, i8* %11, i8 zeroext 0, i8 zeroext 4, i16 zeroext 1, i8 zeroext 1)
  %13 = getelementptr inbounds %struct.VirtualPointer, %struct.VirtualPointer* %7, i32 0, i32 0
  store i32 %12, i32* %13, align 4
  %14 = load i32, i32* %5, align 4
  %15 = load i64, i64* %3, align 8
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([54 x i8], [54 x i8]* @.str.3, i64 0, i64 0), i32 %14, i64 %15)
  %17 = load i32, i32* %5, align 4
  %18 = load i64, i64* %3, align 8
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([59 x i8], [59 x i8]* @.str.4, i64 0, i64 0), i32 %17, i64 %18)
  %20 = load i64, i64* %3, align 8
  %21 = call noalias i8* @malloc(i64 %20) #4
  ret i8* %21
}

declare dso_local i32 @VirtualMemory_set(%struct.VirtualMemory*, i8*, i8 zeroext, i8 zeroext, i16 zeroext, i8 zeroext) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @__carmor_realloc_(i8*, i64) #0 {
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
  %11 = call i8* @realloc(i8* %9, i64 %10) #4
  ret i8* %11
}

; Function Attrs: nounwind
declare dso_local i8* @realloc(i8*, i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define internal i64 @RoundUpTo(i64, i64) #0 {
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
define internal i64 @RoundDownTo(i64, i64) #0 {
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

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.1-svn374861-1~exp1~20191015075537.63 (branches/release_90)"}
