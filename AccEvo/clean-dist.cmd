rem		David Oguns
rem 	Batch script called before builds of Accevo game engine
rem 	is built.  This script will remove the previously built 
rem		header files from the shared include directory.

@echo off

set BIN_FILE=%1
set TARGET_DIR=%2

rd %TARGET_DIR%include /s /q

exit 0

