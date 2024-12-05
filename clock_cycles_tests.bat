@echo off
setlocal enabledelayedexpansion

:: Number of runs per test
set runs=100

:: Output CSV file
set csv_file=benchmarking_chacha20_v128.csv

:: Initialize CSV file
echo Initializing CSV file: %csv_file%
echo Test,Run,Clock Cycles > %csv_file%

:: Loop through tests
for /L %%t in (1,1,5) do (
    echo Testing Test %%t...
    
    :: Loop through runs
    for /L %%r in (1,1,%runs%) do (
        echo.
        echo Test %%t, Run %%r...
        
        :: Clean and compile
        make clean > NUL 2>&1
        make > NUL 2>&1

        :: Run the program and capture output
        for /F "tokens=*" %%o in ('chacha20 --clock-ct %%t 2^>nul') do (
            set "output=%%o"
        )

        :: Debugging step to ensure we have output
        echo Program output: !output!

        :: Extract clock cycles (corrected parsing logic)
        for /F "tokens=5" %%c in ('echo !output! ^| findstr "Clock cycles per byte"') do (
            set "clock_cycles=%%c"
        )

        :: Ensure that we have a valid clock cycle value
        if not defined clock_cycles (
            echo Error: Unable to extract clock cycles for Test %%t, Run %%r
        ) else (
            echo Extracted Clock Cycles: !clock_cycles!
            :: Append to CSV file
            echo %%t,%%r,!clock_cycles! >> %csv_file%
        )
        
        :: Clear clock_cycles variable for next iteration
        set clock_cycles=
    )
)

endlocal
