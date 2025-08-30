// Thread_thread_function.cpp
// clang++ -std=c++17 -pthread 4_Thread_thread_function.cpp -o a; ./a
// @author : Dhiraj Darakhe
// @date   : 2024-06-10
// @brief  : Thread function examples and usage
//
// This example demonstrates:
// 1. Creating a thread using a function
// 2. Getting thread ID
// 3. Checking joinable status
// 4. Using native_handle()
// 5. Hardware concurrency (number of CPU cores)

#include <iostream>
#include <thread>

// -----------------------------------------------------------
// Simple thread function
void ThreadFunction() {
    // Print message and thread ID
    std::cout << "Hello from thread! Thread ID: " 
              << std::this_thread::get_id() << std::endl;

    // Simulate work: print some characters
    for (int i = 0; i < 5; ++i) {
        std::cout << "*";
        // Optionally simulate delay
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));       
    }
    std::cout << std::endl;
}

// -----------------------------------------------------------
int main() {
    // Step 1: Create a thread to run ThreadFunction
    std::thread t(ThreadFunction);

    // Step 2: Print information from main thread
    std::cout << "Hello from main thread! Thread ID: " 
              << std::this_thread::get_id() << std::endl;

    // Step 3: Get thread ID of the new thread
    std::cout << "Thread ID of t: " << t.get_id() << std::endl;

    // Step 4: Get native handle (OS-specific handle) of the thread
    std::cout << "Native Handle: " << t.native_handle() << std::endl;

    // Step 5: Check number of hardware threads supported (CPU cores)
    std::cout << "Number of hardware threads supported: " 
              << std::thread::hardware_concurrency() << std::endl;

    // Step 6: Check if thread is joinable
    if (t.joinable()) {
        std::cout << "Thread is joinable." << std::endl;
    } else {
        std::cout << "Thread is not joinable." << std::endl;
    }

    // Step 7: Wait for the thread to finish
    t.join();
    std::cout << "Thread has completed execution." << std::endl;

    return 0;
}

/* ------------------------------------------------------------------
THEORY & KEY POINTS: Thread Functions and Thread Info

1. Thread creation:
   - std::thread t(ThreadFunction);
   - A new thread is spawned and ThreadFunction starts executing concurrently with main().

2. Thread IDs:
   - std::this_thread::get_id() → returns the ID of the calling thread.
   - t.get_id() → returns the ID of thread t.

3. Native handle:
   - t.native_handle() → OS-specific handle of the thread (useful for low-level APIs).

4. Hardware concurrency:
   - std::thread::hardware_concurrency() → returns number of hardware threads (CPU cores) supported.
   - Useful for deciding number of threads to spawn in parallel.

5. Joinable threads:
   - t.joinable() → true if the thread has not been joined or detached.
   - Must join() or detach() before the thread object is destroyed; otherwise std::terminate() is called.

6. Joining threads:
   - t.join() → main thread waits until thread t finishes.
   - Ensures safe cleanup of resources.

7. Main vs Child thread:
   - Both main and child threads run concurrently.
   - Each has a unique thread ID.
   - Printing thread ID helps in debugging concurrent execution.

💡 Memory Tip / Analogy
	•	std::thread copying args = giving the thread its own copy of the book.
	•	std::ref() = thread reads/writes your original book.
	•	std::cref() = thread reads the original book but cannot modify it.

✅ Key Takeaways
	1.	Thread function is just a regular function executed in a new thread.
	2.	Thread ID distinguishes threads; each thread has a unique ID.
	3.	Native handle gives access to OS-level thread API if needed.
	4.	Joinable indicates whether a thread can be joined safely.
	5.	Always join or detach threads to prevent program termination.
	6.	Hardware concurrency can guide how many threads to spawn efficiently.

💡 Additional Note
	•	Always ensure proper management of thread lifetimes to avoid undefined behavior.

------------------------------------------------------------------ */