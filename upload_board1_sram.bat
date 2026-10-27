@echo off
setlocal
cd /d "%~dp0"
echo ========================================================
echo   Uploading board1_top.bit to SRAM (Temporary)
echo ========================================================
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ofl = 'C:\Users\saire\Desktop\KMITL\Digital\FPGA_Builder_Package\tools\openFPGALoader\openFPGALoader.exe';" ^
  "$bit = '%~dp0board1_top.bit';" ^
  "$env:PATH = 'C:\Users\saire\Desktop\KMITL\Digital\FPGA_Builder_Package\tools\openFPGALoader;' + $env:PATH;" ^
  "$scan = & $ofl --scan-usb 2>&1 | Out-String;" ^
  "$match = [regex]::Match($scan, '(?m)^\s*(\d+)\s+(\d+)\s+0x0403');" ^
  "if ($match.Success) { $bd = \"$([int]$match.Groups[1].Value):$([int]$match.Groups[2].Value)\"; & $ofl -c ft2232 --busdev-num $bd -m $bit } else { & $ofl -c ft2232 -m $bit }"
if errorlevel 1 (
    echo.
    echo [ERROR] Upload failed!
    pause
    exit /b 1
)
echo.
echo [SUCCESS] SRAM programmed successfully!
pause
