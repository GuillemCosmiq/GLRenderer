@echo off

Sharpmake.Application.exe /sources(@"main.sharpmake.cs")

echo.

if errorlevel 0 (
   color 2f
   echo Bootstrap finished with no errors
   pause
   exit /b %errorlevel%
) else (
   color 4f
   echo Solution/Project could not be generated
   pause
   exit /b %errorlevel%
)

@echo off