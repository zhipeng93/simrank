#/bin/bash 
echo $1 >> www05
./analysis BA1000-4-9 | sed -n "17p" >> www05
./analysis BA1000-10 | sed -n "17p" >> www05
./analysis BA5000-10 | sed -n "17p" >> www05
./analysis ER1000-5 | sed -n "17p" >> www05
./analysis ER5000-10 | sed -n "17p" >> www05
