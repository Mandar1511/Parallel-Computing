/*
 Parallel Merge sort.
 Author: Mandar Deshpande
 Date: 07/02/2024
 */
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <chrono>
#include <unordered_map>
#ifdef CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#else
#define cilk_spawn
#define cilk_sync
#define cilk_for for
#endif

#define MERGESIZE (10000)
using namespace std;

#ifdef CILK
void identity(void *view)
{
    *(int64_t *)view = 0;
}
void addInt64(void *left, void *right)
{
    *(int64_t *)left += *(int64_t *)right;
}
#endif

/* Validate returns a count of the number of times the i-th number is less than (i-1)th number. */
int64_t Validate(const vector<int64_t> &input)
{
#ifdef CILK
    int64_t cilk_reducer(identity, addInt64) count;
    count = 0;
#else
    int64_t count = 0;
#endif
    cilk_for(int64_t i = 1; i < input.size(); i++)
    {
        if (input[i] < input[i - 1])
        {
            count += 1;
        }
    }
    return count;
}
/* SerialMerge merges two sorted arrays vec[Astart..Aend] and vec[Bstart..Bend] into vector tmp starting at index tmpIdx. */
void SerialMerge(vector<int64_t> &vec, int64_t Astart, int64_t Aend, int64_t Bstart, int64_t Bend, vector<int64_t> &tmp, int64_t tmpIdx)
{
    if (Astart <= Aend && Bstart <= Bend)
    {
        for (;;)
        {
            if (vec[Astart] < vec[Bstart])
            {
                tmp[tmpIdx] = vec[Astart];
                tmpIdx++;
                Astart++;
                if (Astart > Aend)
                    break;
            }
            else
            {
                tmp[tmpIdx] = vec[Bstart];
                Bstart++;
                tmpIdx++;
                if (Bstart > Bend)
                    break;
            }
        }
    }
    if (Astart > Aend)
    {
        memcpy(&tmp[tmpIdx], &vec[Bstart], sizeof(int64_t) * (Bend - Bstart + 1));
    }
    else
    {
        memcpy(&tmp[tmpIdx], &vec[Astart], sizeof(int64_t) * (Aend - Astart + 1));
    }
}

void parallelMerge(vector<int64_t> &vec, int64_t Astart, int64_t Aend, int64_t Bstart, int64_t Bend, vector<int64_t> &tmp, int64_t tmpIdx)
{
    int64_t s1 = Aend - Astart + 1;      // size of first sorted half (A)
    int64_t s2 = Bend - Bstart + 1;      // size of second sorted half (B)
    int64_t amid = vec[Astart + s1 / 2]; // median of A
    if (s2 <= MERGESIZE || s1 <= MERGESIZE)
    {
        // two halves are sufficiently small or all elements are less than amid
        SerialMerge(vec, Astart, Aend, Bstart, Bend, tmp, tmpIdx);
        return;
    }
    if (s1 < s2) // Always keeping A as the larger subarray, this will ensure that Bigger subarray A is divided into 2 halves for sure
    {
        parallelMerge(vec, Bstart, Bend, Astart, Aend, tmp, tmpIdx);
        return;
    }
    auto it = lower_bound(vec.begin() + Bstart, vec.begin() + Bend + 1, amid);
    int j = it - vec.begin(); // gives index of first element >= amid inside B
    if (vec[j] > amid)
    {
        j--;
    }
    if (j < Bstart || j > Bend)
    {
        // if the first element of B is > amid
        SerialMerge(vec, Astart, Aend, Bstart, Bend, tmp, tmpIdx);
        return;
    }
    cilk_spawn parallelMerge(vec, Astart, Astart + s1 / 2, Bstart, j, tmp, tmpIdx);
    parallelMerge(vec, Astart + s1 / 2 + 1, Aend, j + 1, Bend, tmp, tmpIdx + (s1 / 2) + j - Bstart + 2); // incremented tmpIdx by size of of A and B
}

void parallelMergeSort(vector<int64_t> &vec, int64_t start, vector<int64_t> &tmp, int64_t tmpStart, int64_t sz, int64_t depth, int64_t limit)
{
    if (sz < MERGESIZE || depth > limit)
    {
        // ensures that we are not spawning too many tasks
        sort(vec.begin() + start, vec.begin() + start + sz);
        return;
    }
    auto half = sz / 2;
    auto A = start;
    auto tmpA = tmpStart;
    auto B = start + half;
    auto tmpB = tmpStart + half;
    // Sort left half.
    cilk_spawn parallelMergeSort(vec, A, tmp, tmpA, half, depth + 1, limit);
    // Sort right half.
    parallelMergeSort(vec, B, tmp, tmpB, start + sz - B, depth + 1, limit);
    cilk_sync; // we want first half and second half to be sorted before merging them.
    // Merge sorted parts into tmp.
    parallelMerge(vec, A, A + half - 1, B, start + sz - 1, tmp, tmpStart);
    // Copy result back to vec.
    memcpy(&vec[A], &tmp[tmpStart], sizeof(int64_t) * (sz));
}

// Initialize vec[start..end) with random numbers.
void Init(vector<int64_t> &vec, int64_t start, int64_t end)
{
    struct drand48_data buffer;
    srand48_r(time(NULL), &buffer);
    for (int64_t i = start; i < end; i++)
    {
        lrand48_r(&buffer, &vec[i]);
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Usage " << argv[0] << " <vector size> <cutoff>\n";
        return -1;
    }
    int64_t sz = atol(argv[1]);
    cout << "\nvector size=" << sz << "\n";
    if (sz == 0)
    {
        cout << "Usage " << argv[0] << " <vector size> <cutoff>\n";
        return -1;
    }
    int64_t cutoff = atol(argv[2]);
    vector<int64_t> input(sz);
    vector<int64_t> tmp(sz);

    const int64_t SZ = 100000;
    cilk_for(int64_t i = 0; i < input.size(); i += SZ)
    {
        Init(input, i, min(static_cast<int64_t>(input.size()), static_cast<int64_t>(i + SZ)));
    }
    auto now = chrono::system_clock::now();
    parallelMergeSort(input, 0, tmp, 0, input.size(), /*depth=*/0, /*limit=*/cutoff);
    cout << "\nmillisec=" << std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - now).count() << "\n";
    auto mistakes = Validate(input);
    cout << "Mistakes=" << mistakes << "\n";
    assert((mistakes == 0) && " Validate() failed");
    return 0;
}
