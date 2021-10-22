@echo off
setlocal EnableDelayedExpansion

    if not defined TM_SDK_DIR (
        echo TM_SDK_DIR environment variable is not set. Please point it to your The Machinery directory.
        set errorlevel=1
    ) else (
        if exist "%TM_SDK_DIR%\\headers" call "%TM_SDK_DIR%\\bin\\tmbuild.exe"
        if exist "%TM_SDK_DIR%\\bin\\Debug\\tmbuild.exe" (call "%TM_SDK_DIR%\\bin\\Debug\\tmbuild.exe") else (
        if exist "%TM_SDK_DIR%\\bin\\Release\\tmbuild.exe" call "%TM_SDK_DIR%\\bin\\Release\\tmbuild.exe"
        )
    )

if NOT ["%errorlevel%"]==["0"] ( pause ) else (
    if not EXIST ".\build\.is_general_project" (
    choice /C YN /M "Is this plugin a general plugin? A general plugin is a plugin that should be available in all projects. If you press yes the plugin.dll & .pdb will be copied into the engines plugins folder. Otherweise you need to drag the plugin into your project."
        if !errorlevel! EQU 1 (
            echo "is_general_project">".\build\.is_general_project"
            if EXIST %TM_SDK_DIR%/bin/Debug/the-machinery.exe ( 
                xcopy /y /s "bin\Debug\*.dll" "%TM_SDK_DIR%\bin\Debug\plugins\" 
                xcopy /y /s "bin\Debug\*.pdb" "%TM_SDK_DIR%\bin\Debug\plugins\" 
            ) else (
                if EXIST %TM_SDK_DIR%/bin/Release/the-machinery.exe ( xcopy /y /s "..\..\bin\Release\*.dll" "%TM_SDK_DIR%\bin\Release\plugins\" )
                if EXIST %TM_SDK_DIR%/bin/the-machinery.exe (
                    xcopy /y /s "bin\Debug\*.dll" "%TM_SDK_DIR%\bin\plugins\" 
                    xcopy /y /s "bin\Debug\*.pdb" "%TM_SDK_DIR%\bin\plugins\" 
                    xcopy /y /s "bin\Release\*.dll" "%TM_SDK_DIR%\bin\plugins\" 
                )
            )
        ) else (
            exit
        )
    )
)