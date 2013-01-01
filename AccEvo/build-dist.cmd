rem		David Oguns
rem 	Batch script called after builds of Accevo game engine
rem 	is built.  This script will copy the necessary header files
rem		to the shared include directory

@echo off

set BIN_FILE=%1
set TARGET_DIR=%2
set INCLUDE_SRC=%3

mkdir %TARGET_DIR%include
mkdir %TARGET_DIR%include\accevo

xcopy %INCLUDE_SRC%\*.h %TARGET_DIR%include\accevo /E /Y /I

exit 0

