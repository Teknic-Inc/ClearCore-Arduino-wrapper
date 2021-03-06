SET version=1.0.12

SET root_teknic="%LOCALAPPDATA%\Arduino15\packages\ClearCore\hardware\sam\%version%\Teknic\libClearCore"
SET root_atmel_examples="%LOCALAPPDATA%\Arduino15\packages\ClearCore\hardware\sam\%version%\Teknic\Atmel_Examples"
SET root_arduino_examples="%LOCALAPPDATA%\Arduino15\packages\ClearCore\hardware\sam\%version%\libraries"
SET root_arduino="%LOCALAPPDATA%\Arduino15\packages\ClearCore\hardware\sam\%version%\cores\arduino"

.\tools\AStyle.exe --options=configuration\astyle.cfg -r "%root_teknic%\inc\*.h" "%root_teknic%\src\*.cpp" "%root_teknic%\UnitTests\src\*.cpp" "%root_arduino%\*.h" "%root_arduino%\*.cpp" "%root_arduino_examples%\*.ino" "%root_atmel_examples%\*.cpp"