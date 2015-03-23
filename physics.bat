REM the following call starts a basic omegalib/cyclops example
REM the -D %~dp0% option is used to ignore the OMEGA_HOME environment variable 
REM (if present), and set the default data dir to the path of this batch file
.\bin\orun.exe -D %~dp0% -s examples/cyclops/physics.py