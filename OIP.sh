#!/bin/bash
execute(){
    echo "in method execute"
    for i in `cat $1`
    do
        echo "hello"
        echo $i
        echo "./SRbenchmark -c $i | tail -n 1 $2"
        ./SRbenchmark -c $i | tail -n 1 >> $2
    done
}

echo "OIP whole" >> $2
for i in `cat $1`
do
    sed -i "22s/true/false/" $i
done
execute $1 $2

echo "OIP half" >> $2
for i in `cat $1`
do
    sed -i "22s/false/true/" $i
done
execute $1 $2
