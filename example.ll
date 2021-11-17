; ModuleID = 'example.c'
source_filename = "example.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx11.0.0"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @foo(i32 %n) #0 !dbg !9 {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %n.addr, metadata !13, metadata !DIExpression()), !dbg !14
  %0 = load i32, i32* %n.addr, align 4, !dbg !15
  %cmp = icmp eq i32 %0, 3, !dbg !17
  br i1 %cmp, label %if.then, label %if.else, !dbg !18

if.then:                                          ; preds = %entry
  store i32 0, i32* %retval, align 4, !dbg !19
  br label %return, !dbg !19

if.else:                                          ; preds = %entry
  store i32 -1, i32* %retval, align 4, !dbg !21
  br label %return, !dbg !21

return:                                           ; preds = %if.else, %if.then
  %1 = load i32, i32* %retval, align 4, !dbg !23
  ret i32 %1, !dbg !23
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !24 {
entry:
  %retval = alloca i32, align 4
  %number = alloca i32, align 4
  %output = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata i32* %number, metadata !27, metadata !DIExpression()), !dbg !28
  %call = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i32* %number), !dbg !29
  call void @llvm.dbg.declare(metadata i32* %output, metadata !30, metadata !DIExpression()), !dbg !31
  %0 = load i32, i32* %number, align 4, !dbg !32
  %call1 = call i32 @foo(i32 %0), !dbg !33
  store i32 %call1, i32* %output, align 4, !dbg !31
  %1 = load i32, i32* %output, align 4, !dbg !34
  ret i32 %1, !dbg !35
}

declare i32 @scanf(i8*, ...) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.dbg.cu = !{!5}
!llvm.ident = !{!8}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 11, i32 3]}
!1 = !{i32 7, !"Dwarf Version", i32 4}
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = distinct !DICompileUnit(language: DW_LANG_C99, file: !6, producer: "Apple clang version 13.0.0 (clang-1300.0.29.3)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !7, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk", sdk: "MacOSX.sdk")
!6 = !DIFile(filename: "example.c", directory: "/Users/triciadang/Documents/CS6340/good_one")
!7 = !{}
!8 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
!9 = distinct !DISubprogram(name: "foo", scope: !6, file: !6, line: 27, type: !10, scopeLine: 27, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!10 = !DISubroutineType(types: !11)
!11 = !{!12, !12}
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DILocalVariable(name: "n", arg: 1, scope: !9, file: !6, line: 27, type: !12)
!14 = !DILocation(line: 27, column: 13, scope: !9)
!15 = !DILocation(line: 28, column: 9, scope: !16)
!16 = distinct !DILexicalBlock(scope: !9, file: !6, line: 28, column: 9)
!17 = !DILocation(line: 28, column: 11, scope: !16)
!18 = !DILocation(line: 28, column: 9, scope: !9)
!19 = !DILocation(line: 29, column: 9, scope: !20)
!20 = distinct !DILexicalBlock(scope: !16, file: !6, line: 28, column: 17)
!21 = !DILocation(line: 31, column: 9, scope: !22)
!22 = distinct !DILexicalBlock(scope: !16, file: !6, line: 30, column: 12)
!23 = !DILocation(line: 33, column: 1, scope: !9)
!24 = distinct !DISubprogram(name: "main", scope: !6, file: !6, line: 35, type: !25, scopeLine: 35, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!25 = !DISubroutineType(types: !26)
!26 = !{!12}
!27 = !DILocalVariable(name: "number", scope: !24, file: !6, line: 36, type: !12)
!28 = !DILocation(line: 36, column: 9, scope: !24)
!29 = !DILocation(line: 37, column: 5, scope: !24)
!30 = !DILocalVariable(name: "output", scope: !24, file: !6, line: 38, type: !12)
!31 = !DILocation(line: 38, column: 9, scope: !24)
!32 = !DILocation(line: 38, column: 22, scope: !24)
!33 = !DILocation(line: 38, column: 18, scope: !24)
!34 = !DILocation(line: 39, column: 12, scope: !24)
!35 = !DILocation(line: 39, column: 5, scope: !24)
