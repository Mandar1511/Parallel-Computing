#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <numeric>
#include <stdint.h>
using namespace std;
struct QNode
{
    atomic<QNode *> next;
    union
    {
        atomic<bool> locked;
        atomic<QNode *> tail;
    };
};

QNode cust_lock;

uint64_t count;
std::atomic<bool> stop(false);
std::vector<std::atomic<uint64_t>> perThreadCounts;

void Acquire(QNode *L)
{
    QNode I;
    while (true)
    {
        QNode *pred = L->tail;
        if (pred == nullptr)
        {
            // lock appears not to be held
            QNode *tmp = nullptr;
            QNode *nxt = (QNode *)&L->next;
            if (L->tail.compare_exchange_strong(tmp, nxt))
            {
                // I have the lock
                return;
            }
        }
        else
        {
            // lock appears to be held
            I.next = nullptr;
            QNode *iptr = &I;
            if (L->tail.compare_exchange_strong(pred, iptr))
            {
                I.locked = true;
                pred->next = &I;
                // wait for lock
                while (I.locked)
                    ;
                QNode *succ = I.next;
                if (succ == nullptr)
                {
                    L->next = nullptr;
                    QNode *iptr = &I;
                    QNode *nxt = (QNode *)&L->next;
                    if (!(L->tail.compare_exchange_strong(iptr, nxt)))
                    {
                        // somebody got into the timing window
                        do
                        {
                            succ = I.next;
                        } while (succ == nullptr); // wait for successor
                        L->next = succ;
                    }
                    return;
                }
                else
                {
                    L->next = succ;
                    return;
                }
            }
        }
    }
}

void Release(QNode *L)
{
    QNode *succ = L->next;
    if (succ == nullptr) // no known successor
    {
        QNode *tmp = nullptr;
        QNode *nxt = (QNode *)&L->next;
        if (L->tail.compare_exchange_strong(nxt, tmp))
        {
            return;
        }
        do
        {
            succ = L->next;
        } while (succ == nullptr); // wait for successor
    }
    succ->locked = false;
}

void lockLoop(size_t index)
{
    uint64_t trips = 0;
    while (!stop.load())
    {
        Acquire(&cust_lock);
#ifdef CHECK_CORRECTNESS
        count++;
#endif
        Release(&cust_lock);
        trips++;
    }
    // Update perThreadCounts[index] only once at the end to avoid false sharing between threads.
    perThreadCounts[index].store(trips);
}

void timerThread(int maxTime)
{
    std::this_thread::sleep_for(std::chrono::seconds(maxTime));
    stop.store(true);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <maxThreads> <maxTime in sec per experiment>" << std::endl;
        return 1;
    }

    int maxThreads = std::stoi(argv[1]);
    int maxTime = std::stoi(argv[2]);

    for (int nThreads = 1; nThreads <= maxThreads; ++nThreads)
    {
        count = 0;
        stop.store(false);
        perThreadCounts = std::vector<std::atomic<uint64_t>>(nThreads);

        for (auto &lc : perThreadCounts)
        {
            lc.store(0);
        }

        std::vector<std::thread> threads;

        auto start = std::chrono::high_resolution_clock::now();

        // Start the timer thread
        std::thread timer(timerThread, maxTime);

        for (int i = 0; i < nThreads; ++i)
        {
            threads.emplace_back(lockLoop, i);
        }

        // Wait for the timer to finish
        timer.join();

        for (auto &t : threads)
        {
            t.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;

        uint64_t totalTrips = 0;
        for (auto &lc : perThreadCounts)
        {
            totalTrips += lc.load(std::memory_order_relaxed);
        }

        std::cout << "Threads: " << nThreads << ", Total Locks/sec: " << totalTrips / duration << std::endl;
#ifdef CHECK_CORRECTNESS
        std::cout << "Total Trips: " << totalTrips << ", Count: " << count << std::endl;
        if (totalTrips != count)
        {
            std::cout << "Mismatch in total trips and count!" << std::endl;
        }
#endif
    }

    return 0;
}