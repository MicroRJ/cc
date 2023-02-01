@REM # For debug builds I can't compile with /Za because windows complains about it...
@SET myCompilerOptions=/options:strict /nologo /TP /Od /MDd /Z7 /WX /W4 /Fa /std:c11
@SET myLinkerOptions=/DEBUG /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
@SET myInclude=/I.

@CALL cl %myCompilerOptions% %myInclude% test.c /link %myLinkerOptions%

