set -ex

export PATH=/home/iit/cs410software/opencilk_2_0_0/bin/:$PATH

clang++ -g -fopencilk -fsanitize=cilk -std=c++11 $1.cpp -o $1.cilk
CILK_NWORKERS=1 ./$1.cilk $2

