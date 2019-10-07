; ModuleID = 'inputs/input_for_cc.c'
source_filename = "inputs/input_for_cc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [16 x i8] c"  Inside a ...\0A\00", align 1
@.str.1 = private unnamed_addr constant [16 x i8] c"  Inside b ...\0A\00", align 1
@.str.2 = private unnamed_addr constant [16 x i8] c"  Inside c ...\0A\00", align 1
@.str.3 = private unnamed_addr constant [19 x i8] c"  Inside foo ... \0A\00", align 1
@.str.4 = private unnamed_addr constant [19 x i8] c"  Inside bar ... \0A\00", align 1
@.str.5 = private unnamed_addr constant [17 x i8] c"Inside main ...\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @a() #0 {
entry:
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str, i64 0, i64 0))
  ret void
}

declare dso_local i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @b() #0 {
entry:
  %ii = alloca i32, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.1, i64 0, i64 0))
  store i32 0, i32* %ii, align 4
  store i32 0, i32* %ii, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %ii, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  call void @a()
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %1 = load i32, i32* %ii, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, i32* %ii, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @c() #0 {
entry:
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.2, i64 0, i64 0))
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @foo(i32 %a) #0 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.3, i64 0, i64 0))
  %0 = load i32, i32* %retval, align 4
  ret i32 %0
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @bar(i32 %a, i32 %b) #0 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.4, i64 0, i64 0))
  %0 = load i32, i32* %retval, align 4
  ret i32 %0
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %g = alloca i32 (i32)*, align 8
  %h = alloca i32 (i32, i32)*, align 8
  store i32 0, i32* %retval, align 4
  call void @b()
  call void @a()
  call void @b()
  store i32 (i32)* @foo, i32 (i32)** %g, align 8
  store i32 (i32, i32)* bitcast (i32 (i32)* @foo to i32 (i32, i32)*), i32 (i32, i32)** %h, align 8
  %0 = load i32 (i32)*, i32 (i32)** %g, align 8
  %call = call i32 %0(i32 5)
  %1 = load i32 (i32, i32)*, i32 (i32, i32)** %h, align 8
  %call1 = call i32 %1(i32 5, i32 6)
  %call2 = call i32 @foo(i32 10)
  %call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.5, i64 0, i64 0))
  %call4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.5, i64 0, i64 0))
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0 (https://github.com/llvm/llvm-project.git 1ff955305768b772be19ce8ee76246ab249bbe1c)"}
