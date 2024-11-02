set -ex
export PATH=/home/iit/cs410software/opencilk_2_0_0/bin/:$PATH

clang++ -O2 -g -fopencilk -std=c++11 sum_race.cpp -o sum_race.cilk
clang++ -O2 -g -fopencilk -std=c++11 sum_race.cpp -DSYNC -o sum_mutex.cilk
clang++ -O2 -g -fopencilk -std=c++11 sum_reducer2.cpp -o sum_reducer2.cilk
time CILK_NWORKERS=16 ./sum_race.cilk $1
time CILK_NWORKERS=16 ./sum_mutex.cilk $1
time CILK_NWORKERS=16 ./sum_reducer2.cilk $1
