#!/bin/bash

###########################
# DO NOT MODIFY THIS FILE #
###########################

URL="https://github.com/nasa/fprime/archive/refs/tags/v3.4.1.zip"
ZIP_FILE="v3.4.1.zip"
EXTRACT_DIR="fprime-3.4.1"
TARGET_DIR="fprime"
FILE_TO_MODIFY="$TARGET_DIR/cmake/settings.cmake"
SEARCH_STRING="set(CMAKE_CXX_STANDARD 11)"
REPLACE_STRING="set(CMAKE_CXX_STANDARD 17)"

# Download the file
echo "Downloading $ZIP_FILE..."
wget -q --show-progress $URL

# Check if download was successful
if [ $? -ne 0 ]; then
    echo "Download failed!"
    exit 1
fi

# Unzip the file
echo "Unzipping $ZIP_FILE..."
unzip -q $ZIP_FILE

# Check if unzip was successful
if [ $? -ne 0 ]; then
    echo "Failed to unzip $ZIP_FILE!"
    exit 1
fi

# Rename the extracted directory
echo "Renaming $EXTRACT_DIR to $TARGET_DIR..."
mv $EXTRACT_DIR $TARGET_DIR

# Modify the CMake settings file
if [ -f "$FILE_TO_MODIFY" ]; then
    echo "Set C++17 as default"
    sed -i "s/$SEARCH_STRING/$REPLACE_STRING/" "$FILE_TO_MODIFY"
    echo "File $FILE_TO_MODIFY was modified successfully."
else
    echo "File $FILE_TO_MODIFY not found!"
    exit 1
fi

# Clean up the zip file
echo "Cleaning up..."
rm $ZIP_FILE

echo "Done."
