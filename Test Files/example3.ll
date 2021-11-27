; ModuleID = 'example3.c'
source_filename = "example3.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx11.0.0"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @foo(i32 %a, i32 %b) #0 !dbg !9 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !13, metadata !DIExpression()), !dbg !14
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !15, metadata !DIExpression()), !dbg !16
  %0 = load i32, i32* %a.addr, align 4, !dbg !17
  %1 = load i32, i32* %b.addr, align 4, !dbg !19
  %cmp = icmp slt i32 %0, %1, !dbg !20
  br i1 %cmp, label %if.then, label %if.else, !dbg !21

if.then:                                          ; preds = %entry
  store i32 -1, i32* %retval, align 4, !dbg !22
  br label %return, !dbg !22

if.else:                                          ; preds = %entry
  %2 = load i32, i32* %b.addr, align 4, !dbg !24
  %3 = load i32, i32* %a.addr, align 4, !dbg !26
  %cmp1 = icmp sge i32 %2, %3, !dbg !27
  br i1 %cmp1, label %if.then2, label %if.else3, !dbg !28

if.then2:                                         ; preds = %if.else
  store i32 1, i32* %retval, align 4, !dbg !29
  br label %return, !dbg !29

if.else3:                                         ; preds = %if.else
  store i32 0, i32* %retval, align 4, !dbg !31
  br label %return, !dbg !31

return:                                           ; preds = %if.else3, %if.then2, %if.then
  %4 = load i32, i32* %retval, align 4, !dbg !33
  ret i32 %4, !dbg !33
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !34 {
entry:
  %retval = alloca i32, align 4
  %number1 = alloca i32, align 4
  %number2 = alloca i32, align 4
  %output = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata i32* %number1, metadata !37, metadata !DIExpression()), !dbg !38
  call void @llvm.dbg.declare(metadata i32* %number2, metadata !39, metadata !DIExpression()), !dbg !40
  %call = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i32* %number1), !dbg !41
  %call1 = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i32* %number2), !dbg !42
  call void @llvm.dbg.declare(metadata i32* %output, metadata !43, metadata !DIExpression()), !dbg !44
  %0 = load i32, i32* %number1, align 4, !dbg !45
  %1 = load i32, i32* %number2, align 4, !dbg !46
  %call2 = call i32 @foo(i32 %0, i32 %1), !dbg !47
  store i32 %call2, i32* %output, align 4, !dbg !44
  %2 = load i32, i32* %output, align 4, !dbg !48
  ret i32 %2, !dbg !49
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
!6 = !DIFile(filename: "example3.c", directory: "/Users/triciadang/Downloads")
!7 = !{}
!8 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
!9 = distinct !DISubprogram(name: "foo", scope: !6, file: !6, line: 3, type: !10, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!10 = !DISubroutineType(types: !11)
!11 = !{!12, !12, !12}
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DILocalVariable(name: "a", arg: 1, scope: !9, file: !6, line: 3, type: !12)
!14 = !DILocation(line: 3, column: 13, scope: !9)
!15 = !DILocalVariable(name: "b", arg: 2, scope: !9, file: !6, line: 3, type: !12)
!16 = !DILocation(line: 3, column: 20, scope: !9)
!17 = !DILocation(line: 4, column: 9, scope: !18)
!18 = distinct !DILexicalBlock(scope: !9, file: !6, line: 4, column: 9)
!19 = !DILocation(line: 4, column: 13, scope: !18)
!20 = !DILocation(line: 4, column: 11, scope: !18)
!21 = !DILocation(line: 4, column: 9, scope: !9)
!22 = !DILocation(line: 5, column: 9, scope: !23)
!23 = distinct !DILexicalBlock(scope: !18, file: !6, line: 4, column: 16)
!24 = !DILocation(line: 6, column: 16, scope: !25)
!25 = distinct !DILexicalBlock(scope: !18, file: !6, line: 6, column: 16)
!26 = !DILocation(line: 6, column: 21, scope: !25)
!27 = !DILocation(line: 6, column: 18, scope: !25)
!28 = !DILocation(line: 6, column: 16, scope: !18)
!29 = !DILocation(line: 7, column: 9, scope: !30)
!30 = distinct !DILexicalBlock(scope: !25, file: !6, line: 6, column: 24)
!31 = !DILocation(line: 9, column: 9, scope: !32)
!32 = distinct !DILexicalBlock(scope: !25, file: !6, line: 8, column: 12)
!33 = !DILocation(line: 11, column: 1, scope: !9)
!34 = distinct !DISubprogram(name: "main", scope: !6, file: !6, line: 13, type: !35, scopeLine: 13, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!35 = !DISubroutineType(types: !36)
!36 = !{!12}
!37 = !DILocalVariable(name: "number1", scope: !34, file: !6, line: 14, type: !12)
!38 = !DILocation(line: 14, column: 9, scope: !34)
!39 = !DILocalVariable(name: "number2", scope: !34, file: !6, line: 14, type: !12)
!40 = !DILocation(line: 14, column: 18, scope: !34)
!41 = !DILocation(line: 15, column: 5, scope: !34)
!42 = !DILocation(line: 16, column: 5, scope: !34)
!43 = !DILocalVariable(name: "output", scope: !34, file: !6, line: 17, type: !12)
!44 = !DILocation(line: 17, column: 9, scope: !34)
!45 = !DILocation(line: 17, column: 22, scope: !34)
!46 = !DILocation(line: 17, column: 31, scope: !34)
!47 = !DILocation(line: 17, column: 18, scope: !34)
!48 = !DILocation(line: 18, column: 12, scope: !34)
!49 = !DILocation(line: 18, column: 5, scope: !34)
