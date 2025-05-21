#!/bin/bash
set -x  # Enable debug output

# Set the DISPLAY environment variable
export DISPLAY=:0

# Allow root user to access the display
xhost +local:root

# Launch your Qt application
/home/vboxuser/EX/build/Desktop-Debug/EX
