@echo off
setlocal enabledelayedexpansion

REM
REM Copyright (c) Ian Pike
REM Copyright (c) Snap contributors
REM
REM Snap is provided under the Apache-2.0 License WITH LLVM-exception.
REM See LICENSE for more information.
REM
REM SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
REM

if not defined SNAP_LINT_CLANG_TIDY set "SNAP_LINT_CLANG_TIDY=clang-tidy"
if not defined SNAP_LINT_BUILD_DIR set "SNAP_LINT_BUILD_DIR=out\clang-tidy"
if not defined SNAP_LINT_SOURCE_DIR set "SNAP_LINT_SOURCE_DIR=include\snap"
if not defined SNAP_LINT_FILE_GLOB set "SNAP_LINT_FILE_GLOB=*.hpp"
if not defined SNAP_LINT_STD set "SNAP_LINT_STD=c++20"

set "CLANG_TIDY_BIN=%SNAP_LINT_CLANG_TIDY%"
call :ensure_cmd "%CLANG_TIDY_BIN%"
if errorlevel 1 exit /b 1

call :ensure_compile_db
if errorlevel 1 exit /b 1

set "status=0"
set "files_seen=0"
echo Beginning linting...
for /r "%SNAP_LINT_SOURCE_DIR%" %%f in (%SNAP_LINT_FILE_GLOB%) do (
    set "files_seen=1"
    call "%CLANG_TIDY_BIN%" "%%f" -p="%SNAP_LINT_BUILD_DIR%" --quiet --extra-arg=-std=%SNAP_LINT_STD% %SNAP_LINT_EXTRA_ARGS%
    if errorlevel 1 (
        echo Error in %%f
        set "status=1"
    )
)
if "%files_seen%"=="0" (
    echo No headers matched in %SNAP_LINT_SOURCE_DIR% (glob: %SNAP_LINT_FILE_GLOB%)
)

if "%status%"=="0" (
    echo Linting complete.
) else (
    echo Linting finished with errors.
)

exit /b %status%

:ensure_cmd
where %~1 >nul 2>nul
if errorlevel 1 (
    echo Missing dependency: %~1
    exit /b 1
)
exit /b 0

:ensure_compile_db
if exist "%SNAP_LINT_BUILD_DIR%\compile_commands.json" exit /b 0
if /i "%SNAP_LINT_SKIP_CONFIGURE%"=="1" (
    echo Missing compile_commands.json in %SNAP_LINT_BUILD_DIR%.
    exit /b 1
)
call :ensure_cmd cmake
if errorlevel 1 exit /b 1
if not defined SNAP_LINT_CMAKE_GENERATOR (
    where ninja >nul 2>nul
    if not errorlevel 1 (
        set "SNAP_LINT_CMAKE_GENERATOR=Ninja"
    ) else (
        set "SNAP_LINT_CMAKE_GENERATOR=NMake Makefiles"
    )
)
echo Generating compile_commands.json in %SNAP_LINT_BUILD_DIR%...
cmake -S . -B "%SNAP_LINT_BUILD_DIR%" -G "%SNAP_LINT_CMAKE_GENERATOR%" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON %SNAP_LINT_CMAKE_FLAGS%
exit /b %errorlevel%
