@REM switch to /TC
@SET myCompilerOptions=/options:strict /nologo /TP /MTd /Od /Z7 /WX /W4 /Fa /std:c11
@SET myLinkerOptions=/DEBUG /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
@SET myInclude=/I.

@CALL cl -D_HARD_DEBUG -D_DEVELOPER %myCompilerOptions% %myInclude% test.c /link %myLinkerOptions%

