@SET myCompilerOptions=/options:strict /nologo /TC /Od /Z7 /WX /W4 /Fa /std:c11
@SET myLinkerOptions=/DEBUG /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
@SET myInclude=/I.
@CALL cl -D_DEBUG -D_HARD_DEBUG -D_DEVELOPER %myCompilerOptions% %myInclude% main.c /link %myLinkerOptions%
@REM @CALL cl -D_DEBUG -D_HARD_DEBUG -D_DEVELOPER %myCompilerOptions% %myInclude% ccmvm.c /link %myLinkerOptions%
@REM @CALL cl -D_DEVELOPER %myCompilerOptions% /O2 %myInclude% main.c /Fetest-fast.exe /link %myLinkerOptions%

