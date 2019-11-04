; ModuleID = 'llvm-link'
source_filename = "llvm-link"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [15 x i8] c"tutorialspoint\00", align 1
@.str.1 = private unnamed_addr constant [28 x i8] c"String = %s,  Address = %u\0A\00", align 1
@.str.2 = private unnamed_addr constant [5 x i8] c".com\00", align 1
@.str.3 = private unnamed_addr constant [38 x i8] c"This is the changed malloc of size %d\00", align 1
@.str.1.6 = private unnamed_addr constant [39 x i8] c"This is the changed realloc of size %d\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i8*, align 8
  store i32 0, i32* %1, align 4
  %3 = call noalias i8* @__carmor_malloc_(i64 15) #3
  store i8* %3, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = call i8* @strcpy(i8* %4, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i64 0, i64 0))
  %6 = load i8*, i8** %2, align 8
  %7 = load i8*, i8** %2, align 8
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.1, i64 0, i64 0), i8* %6, i8* %7)
  %9 = load i8*, i8** %2, align 8
  %10 = call i8* @__carmor_realloc_(i8* %9, i64 25) #3
  store i8* %10, i8** %2, align 8
  %11 = load i8*, i8** %2, align 8
  %12 = call i8* @strcat(i8* %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0))
  %13 = load i8*, i8** %2, align 8
  %14 = load i8*, i8** %2, align 8
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.1, i64 0, i64 0), i8* %13, i8* %14)
  %16 = load i8*, i8** %2, align 8
  call void @free(i8* %16) #3
  ret i32 0
}

declare dso_local i8* @strcpy(i8*, i8*) #1

declare dso_local i32 @printf(i8*, ...) #1

declare dso_local i8* @strcat(i8*, i8*) #1

; Function Attrs: nounwind
declare dso_local void @free(i8*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @__carmor_malloc_(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.3, i64 0, i64 0), i64 %3)
  %5 = load i64, i64* %2, align 8
  %6 = call noalias i8* @malloc(i64 %5) #3
  ret i8* %6
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @__carmor_realloc_(i8*, i64) #0 {
  %3 = alloca i8*, align 8
  %4 = alloca i64, align 8
  store i8* %0, i8** %3, align 8
  store i64 %1, i64* %4, align 8
  %5 = load i64, i64* %4, align 8
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([39 x i8], [39 x i8]* @.str.1.6, i64 0, i64 0), i64 %5)
  %7 = load i8*, i8** %3, align 8
  %8 = load i64, i64* %4, align 8
  %9 = call i8* @realloc(i8* %7, i64 %8) #3
  ret i8* %9
}

; Function Attrs: nounwind
declare dso_local i8* @realloc(i8*, i64) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.ident = !{!0, !0}
!llvm.module.flags = !{!1}

!0 = !{!"clang version 9.0.1-svn374861-1~exp1~20191015075537.63 (branches/release_90)"}
!1 = !{i32 1, !"wchar_size", i32 4}
