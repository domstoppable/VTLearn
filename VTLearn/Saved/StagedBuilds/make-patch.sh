#!/bin/bash

if [[ "$1" == "" ]]; then
    echo "Missing argument"
    echo "Usage: $0 patch-folder"
    exit 1
fi

PATCH_NAME=$1

echo "Zipping patch..."
7z a -tzip "$PATCH_NAME.zip" "$PATCH_NAME"/VTLearn/Content/Paks/*_P.pak

echo "Uploading patch..."
scp "$PATCH_NAME.zip" greenlightgo@greenlightgo.org:public_html/vtlearn/"$PATCH_NAME-windows.zip"
