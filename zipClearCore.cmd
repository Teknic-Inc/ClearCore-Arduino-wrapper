set ver=1.7.0
rem set version=shell git describe --dirty --always --tags

rem Build libClearCore and LwIP
"C:\Program Files (x86)\Atmel\Studio\7.0\AtmelStudio.exe" Teknic\libClearCore\ClearCore.atsln /build release
"C:\Program Files (x86)\Atmel\Studio\7.0\AtmelStudio.exe" Teknic\LwIP\LwIP.atsln /build release

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
rd /s /q .\.vscode
rd /s /q .\Teknic\libClearCore\inc-doc
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

rem Hash it
"C:\Program Files\7-Zip\7z.exe" h -scrcsha256 "%zipDir%.zip"

rem Remove the temp directory
rd /s /q .\%zipDir%