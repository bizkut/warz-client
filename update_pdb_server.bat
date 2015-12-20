For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set date=%%a-%%b-%%c)
"C:\Program Files\Debugging Tools for Windows (x64)\symstore.exe" add /r /f D:\WarZ\bin\*.* /s D:\symbol_cache /t "WarZ" /v "ver1.0" /c "%date% build"
"C:\Program Files\Debugging Tools for Windows (x64)\symstore.exe" add /r /f D:\WarZ\server\bin\*.* /s D:\symbol_cache /t "WarZServer" /v "ver1.0" /c "%date% build"

pause