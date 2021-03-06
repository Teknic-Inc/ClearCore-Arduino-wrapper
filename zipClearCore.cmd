set ver=1.1.2
rem set version=shell git describe --dirty --always --tags

.\keywordify.py -S -p .\ 

set zipDir=ClearCore-%ver%

rem Make the directory to zip
md %zipDir%

rem Copy everything into the zip directory
robocopy .\ .\%zipDir% *.* /E

cd %zipDir%

rem Remove the copied (empty) zip directory
rd /s /q .\%zipDir%

rem Remove various files not intended for release
rd /s /q .\Style
rd /s /q .\Debug
rd /s /q .\Release
del .\zipClearCore.cmd
del .\package_clearcore_index.json

rem Delete all the git repos in the directory structure
for /d /r . %%d in (.git) do @if exist "%%d" rd /s /q "%%d"
for /d /r . %%d in (.gitlab) do @if exist "%%d" rd /s /q "%%d"

rem Delete all git-related files in the directory structure
del /s .\*.git*

rem Delete all object and dependency files in the directory structure
del /s .\*.o .\*.d

cd ..

rem Zip it
"C:\Program Files\7-Zip\7z.exe" a -r "%zipDir%.zip" ".\%zipDir%"

rem Remove the temp directory
rd /s /q .\%zipDir%