@SETLOCAL
@SET myCompilerOptions=/options:strict /nologo /TC /Od /Z7 /WX /W4 /Fa /std:c11
@SET myLinkerOptions=/DEBUG /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
@SET myInclude=/I.
@REM @CALL cl -D_DEBUG -D_HARD_DEBUG -D_DEVELOPER %myCompilerOptions% %myInclude% /TP cctest.c /link %myLinkerOptions%
@CALL cl -D_DEBUG -D_HARD_DEBUG -D_DEVELOPER %myCompilerOptions% %myInclude% cctest.c /link %myLinkerOptions%

