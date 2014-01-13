@echo off
@setLocal EnableDelayedExpansion

set batloc=%~dp0
set easygensource=%batloc%

CALL:UNCRUSTCODE
GOTO:EOF

:UNCRUSTCODE
	echo Starting uncrustify %easygensource%
	FOR /R "%easygensource%\src" %%G IN (*.h *.c *.cpp *.glsl) DO call:UNCRUSTFILE %%G
GOTO:EOF

:UNCRUSTFILE
	echo %~1
	uncrustify --no-backup -c %easygensource%\uncrustify.cfg %~1
GOTO:EOF