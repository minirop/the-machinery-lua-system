#!/bin/sh
if [ -z "$TM_SDK_DIR" ]
then
    echo "TM_SDK_DIR environment variable is not set. Please point it to your The Machinery directory.\n"
else
    if [ -d "$TM_SDK_DIR/headers" ]
    then
        ${TM_SDK_DIR}/bin/tmbuild
    elif [ -f "$TM_SDK_DIR/bin/Debug/tmbuild" ]
    then
        ${TM_SDK_DIR}/bin/Debug/tmbuild
    elif [ -f "$TM_SDK_DIR/bin/Release/tmbuild" ]
    then
        ${TM_SDK_DIR}/bin/Release/tmbuild
    fi
fi


