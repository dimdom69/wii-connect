#!/bin/sh

export WIILOAD=tcp:192.168.0.196;cd ../;wiiload wiiconnect.dol

echo "

------------------
(program exited with code: $?)" 		


echo "Press return to continue"
#to be more compatible with shells like dash
dummy_var=""
read dummy_var
rm $0
