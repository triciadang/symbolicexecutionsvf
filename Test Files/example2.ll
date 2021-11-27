; ModuleID = 'example.c'
source_filename = "example.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @foo(i32 %a, i32 %b) #0 !dbg !7 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !11, metadata !DIExpression()), !dbg !12
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !13, metadata !DIExpression()), !dbg !14
  %0 = load i32, i32* %a.addr, align 4, !dbg !15
  %1 = load i32, i32* %b.addr, align 4, !dbg !17
  %cmp = icmp slt i32 %0, %1, !dbg !18
  br i1 %cmp, label %if.then, label %if.else, !dbg !19

if.then:                                          ; preds = %entry
  store i32 -1, i32* %retval, align 4, !dbg !20
  br label %return, !dbg !20

if.else:                                          ; preds = %entry
  store i32 1, i32* %retval, align 4, !dbg !22
  br label %return, !dbg !22

return:                                           ; preds = %if.else, %if.then
  %2 = load i32, i32* %retval, align 4, !dbg !24
  ret i32 %2, !dbg !24
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !25 {
entry:
  %retval = alloca i32, align 4
  %number1 = alloca i32, align 4
  %number2 = alloca i32, align 4
  %output = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata i32* %number1, metadata !28, metadata !DIExpression()), !dbg !29
  call void @llvm.dbg.declare(metadata i32* %number2, metadata !30, metadata !DIExpression()), !dbg !31
  %call = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i32* %number1), !dbg !32
  %call1 = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i32* %number2), !dbg !33
  call void @llvm.dbg.declare(metadata i32* %output, metadata !34, metadata !DIExpression()), !dbg !35
  %0 = load i32, i32* %number1, align 4, !dbg !36
  %1 = load i32, i32* %number2, align 4, !dbg !37
  %call2 = call i32 @foo(i32 %0, i32 %1), !dbg !38
  store i32 %call2, i32* %output, align 4, !dbg !35
  %2 = load i32, i32* %output, align 4, !dbg !39
  ret i32 %2, !dbg !40
}

declare dso_local i32 @__isoc99_scanf(i8*, ...) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5}
!llvm.ident = !{!6}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Ubuntu clang version 11.0.0-2", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "example.c", directory: "/home/paris")
!2 = !{}
!3 = !{i32 7, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{!"Ubuntu clang version 11.0.0-2"}
!7 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 3, type: !8, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DISubroutineType(types: !9)
!9 = !{!10, !10, !10}
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DILocalVariable(name: "a", arg: 1, scope: !7, file: !1, line: 3, type: !10)
!12 = !DILocation(line: 3, column: 13, scope: !7)
!13 = !DILocalVariable(name: "b", arg: 2, scope: !7, file: !1, line: 3, type: !10)
!14 = !DILocation(line: 3, column: 20, scope: !7)
!15 = !DILocation(line: 4, column: 9, scope: !16)
!16 = distinct !DILexicalBlock(scope: !7, file: !1, line: 4, column: 9)
!17 = !DILocation(line: 4, column: 13, scope: !16)
!18 = !DILocation(line: 4, column: 11, scope: !16)
!19 = !DILocation(line: 4, column: 9, scope: !7)
!20 = !DILocation(line: 5, column: 9, scope: !21)
!21 = distinct !DILexicalBlock(scope: !16, file: !1, line: 4, column: 16)
!22 = !DILocation(line: 7, column: 9, scope: !23)
!23 = distinct !DILexicalBlock(scope: !16, file: !1, line: 6, column: 12)
!24 = !DILocation(line: 9, column: 1, scope: !7)
!25 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 11, type: !26, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!26 = !DISubroutineType(types: !27)
!27 = !{!10}
!28 = !DILocalVariable(name: "number1", scope: !25, file: !1, line: 12, type: !10)
!29 = !DILocation(line: 12, column: 9, scope: !25)
!30 = !DILocalVariable(name: "number2", scope: !25, file: !1, line: 12, type: !10)
!31 = !DILocation(line: 12, column: 18, scope: !25)
!32 = !DILocation(line: 13, column: 5, scope: !25)
!33 = !DILocation(line: 14, column: 5, scope: !25)
!34 = !DILocalVariable(name: "output", scope: !25, file: !1, line: 15, type: !10)
!35 = !DILocation(line: 15, column: 9, scope: !25)
!36 = !DILocation(line: 15, column: 22, scope: !25)
!37 = !DILocation(line: 15, column: 31, scope: !25)
!38 = !DILocation(line: 15, column: 18, scope: !25)
!39 = !DILocation(line: 16, column: 12, scope: !25)
!40 = !DILocation(line: 16, column: 5, scope: !25)
