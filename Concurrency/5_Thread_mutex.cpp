// Thread_mutex.cpp
// clang++ -std=c++17 -pthread 5_Thread_mutex.cpp -o a; ./a
// @author :  DhiraxD
// @brief  : Using mutex to protect shared data in multithreaded environment
// References:
// https://en.cppreference.com/w/cpp/thread/mutex
// https://en.cppreference.com/w/cpp/thread/lock_guard

#include <iostream>
#include <list>
#include <thread>
#include <string>
#include <mutex>

// -----------------------------------------------------------
// Shared global data
std::list<int> g_Data;
const int SIZE = 100000;

// Mutex to protect g_Data from race conditions
std::mutex g_Mutex;

// -----------------------------------------------------------
// Worker thread function 1
void Download() {
    for (int i = 0; i < SIZE; ++i) {
        /*
        // Manual lock/unlock (dangerous if exceptions occur)
        g_Mutex.lock();
        g_Data.push_back(i);
        g_Mutex.unlock();
        */

        // Safe alternative: std::lock_guard (RAII)
        // Automatically locks on creation and unlocks when scope ends
        std::lock_guard<std::mutex> mtx(g_Mutex);
        g_Data.push_back(i);
    }
}

// -----------------------------------------------------------
// Worker thread function 2 (similar to Download)
void Download2() {
    for (int i = 0; i < SIZE; ++i) {
        std::lock_guard<std::mutex> mtx(g_Mutex);
        g_Data.push_back(i);
    }
}

// -----------------------------------------------------------
int main() {
    // Step 1: Create threads
    std::thread thDownloader(Download);
    std::thread thDownloader2(Download2);

    /*
    • thDownloader starts running Download() immediately in a new thread.
    • thDownloader2 starts running Download2() immediately in another thread.
    • Now we have 3 threads: main + thDownloader + thDownloader2
    */

    // Step 2: Join threads (wait for completion)
    thDownloader.join();
    /*
    • Main thread blocks here until Download() finishes.
    • Meanwhile, Download2() continues in parallel.
    */

    thDownloader2.join();
    /*
    • Main thread now waits for Download2() to finish.
    • After this point, both worker threads are done.
    */

    // Step 3: Print final data size
    std::cout << g_Data.size() << std::endl; // Should be 2 * SIZE = 200000

    return 0;
}

/* ------------------------------------------------------------------
THEORY & KEY POINTS: Mutex and Lock Guards

1. Race Conditions:
   - Multiple threads modifying shared data simultaneously can corrupt data.
   - Example: g_Data.push_back() without mutex may result in undefined behavior.

2. Mutex:
   - std::mutex provides mutual exclusion.
   - Only one thread can lock a mutex at a time.
   - Other threads trying to lock are blocked until mutex is unlocked.

3. Locking:
   - Manual: g_Mutex.lock() / g_Mutex.unlock() → risky if exception occurs.
   - RAII: std::lock_guard<std::mutex> → locks on creation, unlocks automatically at scope end.

4. Threads:
   - std::thread thDownloader(Download); → new thread starts immediately.
   - main continues concurrently.

5. Joining:
   - thDownloader.join() → main waits for the thread to finish.
   - Must join all joinable threads to avoid std::terminate().

6. Parallelism:
   - Both threads operate on shared data safely using mutex.
   - Actual execution order of threads is non-deterministic.

7. Key Rule:
   - Always protect shared mutable data with a mutex.
   - Prefer RAII (std::lock_guard) over manual lock/unlock.
   - Join all threads before main() exits.

------------------------------------------------------------------

✅ Summary / Memory Tips
	•	Mutex = Traffic light for shared resources → only one thread enters the critical section at a time.
	•	Lock Guard = Automatic traffic cop → locks when entering scope, unlocks automatically.
	•	Join threads → wait until worker threads finish before main exits.
	•	Parallel execution → multiple threads can run at the same time safely when protected by mutex.


*/
