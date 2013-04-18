#!/bin/bash

echo "****************************Program 4 Start*************************************"
echo "Start to wake up 2 writers and 4 readers..."
gnome-terminal -x process/w1
gnome-terminal -x process/w2
gnome-terminal -x process/r1
gnome-terminal -x process/r2
gnome-terminal -x process/r3
gnome-terminal -x process/r4
echo "Succeed to wake up 2 writers and 4 readers..."

