; ModuleID = 'example.c'
source_filename = "example.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx11.0.0"

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
  %1 = load i32, i32* %n.addr, align 4, !dbg !19
  store i32 %1, i32* %retval, align 4, !dbg !21
  br label %return, !dbg !21

if.else:                                          ; preds = %entry
  %2 = load i32, i32* %n.addr, align 4, !dbg !22
  %sub = sub nsw i32 %2, 1, !dbg !24
  store i32 %sub, i32* %retval, align 4, !dbg !25
  br label %return, !dbg !25

return:                                           ; preds = %if.else, %if.then
  %3 = load i32, i32* %retval, align 4, !dbg !26
  ret i32 %3, !dbg !26
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !27 {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call i32 @foo(i32 10), !dbg !30
  ret i32 %call, !dbg !31
}

attributes #0 = { noinline nounwind optnone ssp uwtable "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.dbg.cu = !{!5}
!llvm.ident = !{!8}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 11, i32 3]}
!1 = !{i32 7, !"Dwarf Version", i32 4}
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = distinct !DICompileUnit(language: DW_LANG_C99, file: !6, producer: "Apple clang version 13.0.0 (clang-1300.0.29.3)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !7, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk", sdk: "MacOSX.sdk")
!6 = !DIFile(filename: "example.c", directory: "/Users/triciadang/Documents/CS6340/SVF-example")
!7 = !{}
!8 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
!9 = distinct !DISubprogram(name: "foo", scope: !6, file: !6, line: 13, type: !10, scopeLine: 13, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!10 = !DISubroutineType(types: !11)
!11 = !{!12, !12}
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DILocalVariable(name: "n", arg: 1, scope: !9, file: !6, line: 13, type: !12)
!14 = !DILocation(line: 13, column: 13, scope: !9)
!15 = !DILocation(line: 14, column: 9, scope: !16)
!16 = distinct !DILexicalBlock(scope: !9, file: !6, line: 14, column: 9)
!17 = !DILocation(line: 14, column: 11, scope: !16)
!18 = !DILocation(line: 14, column: 9, scope: !9)
!19 = !DILocation(line: 15, column: 16, scope: !20)
!20 = distinct !DILexicalBlock(scope: !16, file: !6, line: 14, column: 17)
!21 = !DILocation(line: 15, column: 9, scope: !20)
!22 = !DILocation(line: 17, column: 16, scope: !23)
!23 = distinct !DILexicalBlock(scope: !16, file: !6, line: 16, column: 12)
!24 = !DILocation(line: 17, column: 18, scope: !23)
!25 = !DILocation(line: 17, column: 9, scope: !23)
!26 = !DILocation(line: 19, column: 1, scope: !9)
!27 = distinct !DISubprogram(name: "main", scope: !6, file: !6, line: 21, type: !28, scopeLine: 21, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!28 = !DISubroutineType(types: !29)
!29 = !{!12}
!30 = !DILocation(line: 22, column: 12, scope: !27)
!31 = !DILocation(line: 22, column: 5, scope: !27)
