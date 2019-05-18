if exist *.dll del *.dll
if exist *.exe del *.exe
cl program.cpp kernel32.lib user32.lib gdi32.lib advapi32.lib
cl /LD dynamic_lib.cpp kernel32.lib user32.lib gdi32.lib advapi32.lib
del *.obj *.lib *.exp
