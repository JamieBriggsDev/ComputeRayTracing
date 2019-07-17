

@echo off
if [%1]==[] goto :eof
:loop
"M:/Programming Libraries/Vulkan SDK/1.1.108.0/Bin32/glslangValidator.exe" -V "%1"
shift
if not [%1]==[] goto loop

pause