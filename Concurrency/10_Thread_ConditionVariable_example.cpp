// 10_Thread_ConditionVariable_example.cpp
// clang++ -std=c++17 -pthread 10_Thread_ConditionVariable_example.cppp -o a; ./a
// @author :  Dhiraj
// @brief  Demonstration of condition variables in C++
// References:
// https://en.cppreference.com/w/cpp/thread/condition_variable
// https://en.cppreference.com/w/cpp/thread/unique_lock

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

// Global shared data
std::list<int> g_Data;
const int SIZE = 10;

// Synchronization primitives
std::mutex g_Mutex;
std::condition_variable g_CV; 
bool g_DownloadFinished = false; // flag to indicate producer done

// Producer: simulates downloading data
void Download() {
    std::cout << "[Downloader] Started download" << std::endl; 

    for (int i = 0; i < SIZE; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // simulate work
        {
            std::lock_guard<std::mutex> lock(g_Mutex); 
            g_Data.push_back(i); 
            std::cout << "[Downloader] Pushed " << i << std::endl;
        }
        g_CV.notify_one(); // notify consumer that new data is available
    }

    {
        std::lock_guard<std::mutex> lock(g_Mutex);
        g_DownloadFinished = true; 
    }
    g_CV.notify_one(); // notify consumer download is finished

    std::cout << "[Downloader] Finished download" << std::endl; 
}

// Consumer: waits until data is available
void ProcessData() {
    std::cout << "[Processor] Waiting for data..." << std::endl;

    while (true) {
        std::unique_lock<std::mutex> lock(g_Mutex);

        // Wait until data available OR download finished
        g_CV.wait(lock, [] { return !g_Data.empty() || g_DownloadFinished; });

        // Process all available data
        while (!g_Data.empty()) {
            int value = g_Data.front();
            g_Data.pop_front();
            lock.unlock();  // unlock while processing
            std::cout << "[Processor] Processed " << value << std::endl;
            lock.lock();    // re-lock before checking again
        }

        // If download finished and no data left, exit
        if (g_DownloadFinished && g_Data.empty()) {
            break;
        }
    }

    std::cout << "[Processor] All data processed, exiting." << std::endl;
}

int main() {
    std::cout << "[main] User started download + processing" << std::endl;

    std::thread producer(Download);
    std::thread consumer(ProcessData);

    // Ensure safe termination
    if (producer.joinable()) producer.join();
    if (consumer.joinable()) consumer.join();

    std::cout << "[main] Program finished safely." << std::endl;
    return 0;
}

/*
-----------------------------------------
THEORY: Condition Variables
-----------------------------------------

1. What is a condition variable?
   - A synchronization primitive used for thread communication.
   - One thread can "wait" until another thread "signals".
   - Always used with a mutex.

2. Key functions:
   - wait(lock, predicate):
     * Releases the lock and puts thread to sleep.
     * Wakes up only when predicate == true and it has the lock again.
   - notify_one():
     * Wakes up one waiting thread.
   - notify_all():
     * Wakes up all waiting threads.

3. Flow:
   - Producer pushes data, then calls notify_one().
   - Consumer waits (releases lock) until notified and predicate true.
   - Prevents busy-waiting (CPU spin).

4. Why needed?
   - Without condition variables:
     * Consumer would constantly check ("busy waiting").
     * Wastes CPU.
   - With condition variables:
     * Consumer sleeps until producer signals.
     * Efficient and safe.

-----------------------------------------
EXAMPLE OUTPUT (timing may vary):

[main] User started download + processing
[Downloader] Started download
[Processor] Waiting for data...
[Downloader] Pushed 0
[Processor] Processed 0
[Downloader] Pushed 1
[Processor] Processed 1
...
[Downloader] Finished download
[Processor] All data processed, exiting.
[main] Program finished safely.

*/
