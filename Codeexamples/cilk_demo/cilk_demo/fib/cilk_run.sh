set -ex

export PATH=/home/iit/cs410software/opencilk_2_0_0/bin:$PATH
#echo "SERIAL"
#clang++ -O2 -g -std=c++11 $1_serial.cpp -o $1.serial
#time ./$1.serial $2 $3 $4

clang++ -O2 -g -fopencilk -std=c++11 $1.cpp -o $1.cilk
for i in  1 2 3 4 8 16 32 64
do
time CILK_NWORKERS=$i ./$1.cilk $2 $3 $4
done

