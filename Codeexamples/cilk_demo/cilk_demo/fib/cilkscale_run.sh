set -ex

export PATH=/home/iit/cs410software/opencilk_2_0_0/bin/:$PATH

clang++ -O2 -g -fopencilk -fcilktool=cilkscale -std=c++11 $1.cpp -o $1.cilkscale
clang++ -O2 -g -fopencilk -fcilktool=cilkscale-benchmark -std=c++11 $1.cpp -o $1.cilkscale_bm
#python3 /home/iit/cs410software/OpenCilk-12.0.0-Linux/share/Cilkscale_vis/cilkscale.py  -c ./$1.cilkscale -b ./$1.cilkscale_bm -cpus 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 --output-csv $1.csv --output-plot $1.pdf -a $2 $3 $4
#python3 /usr/local/share/Cilkscale_vis/cilkscale.py -c ./$1.cilkscale -b ./$1.cilkscale_bm -cpus 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 --output-csv $1.csv --output-plot $1.pdf -a $2 $3 $4
python3 /home/iit/cs410software/OpenCilk-12.0.0-Linux/share/Cilkscale_vis/cilkscale.py -c ./$1.cilkscale -b ./$1.cilkscale_bm -cpus 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 --output-csv $1.csv --output-plot $1.pdf -a $2 $3 $4

