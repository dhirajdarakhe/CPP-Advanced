// 9_Thread_condition_variable.cpp
// clang++ -std=c++17 -pthread 9_Thread_condition_variable.cpp -o a; ./a
// @author :  DhiraxD
// @brief  Demonstrating producer-consumer using condition_variable
// References:
// https://en.cppreference.com/w/cpp/thread/condition_variable
// https://en.cppreference.com/w/cpp/thread/condition_variable/wait
// https://en.cppreference.com/w/cpp/thread/condition_variable/notify_one

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::mutex mtx;                       // mutex for protecting shared resource
std::condition_variable cv;           // condition variable for thread sync
std::queue<int> g_data;               // shared resource (queue)
const int SIZE = 5;                   // number of items to produce/consume

// Producer thread: generates data and notifies consumer
void Producer() {
    for (int i = 1; i <= SIZE; i++) {
        {
            std::lock_guard<std::mutex> lg(mtx);  // lock mutex
            g_data.push(i);                       // produce item
            std::cout << "[Producer] Produced: " << i << std::endl;
        }
        cv.notify_one(); // wake up consumer if waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Consumer thread: waits until data is available
void Consumer() {
    for (int i = 1; i <= SIZE; i++) {
        std::unique_lock<std::mutex> ul(mtx);     // lock mutex
        cv.wait(ul, [] { return !g_data.empty(); });  
        // wait releases lock until condition is true (queue not empty)
        // avoids busy waiting
        // 1. It releases the lock (ul → unique_lock) so the producer can acquire the mutex and put something into g_data.
        // 2. It puts the consumer thread to sleep (not consuming CPU).
        // 3. When producer pushes data and calls cv.notify_one(), the consumer wakes up.
        // 4. It re-acquires the lock (ul) and checks the condition again:
        // 5. If the queue is not empty, it continues.
        // 6. If it’s still empty (spurious wakeups can happen), it goes back to waiting.
        int item = g_data.front();
        g_data.pop();
        std::cout << "[Consumer] Consumed: " << item << std::endl;
    }
}

int main() {
    std::cout << "[main] Starting Producer and Consumer" << std::endl;

    std::thread t1(Producer);
    std::thread t2(Consumer);

    t1.join();
    t2.join();

    std::cout << "[main] Finished all operations" << std::endl;
    return 0;
}

/*
-----------------------------------------
THEORY: Using condition_variable
-----------------------------------------

1. Motivation:
   - Without condition variables, threads would poll in a loop:
     while(queue.empty()) {  keep checking  }
   - This wastes CPU (busy-waiting).
   - Condition variables let a thread sleep until notified.

2. Key parts:
   - std::condition_variable cv;
   - cv.wait(lock, predicate);
   - cv.notify_one(); OR cv.notify_all();

3. Producer-Consumer logic:
   - Producer locks mutex, adds item to queue, calls notify_one().
   - Consumer locks mutex, calls wait().
       - wait() automatically unlocks mutex and suspends thread.
       - When notified, it re-locks mutex and checks condition.

4. Functions:
   - wait(unique_lock, predicate):
     • Unlocks mutex and sleeps until predicate == true.
     • Handles spurious wakeups safely by re-checking predicate.
   - notify_one():
     • Wakes one waiting thread.
   - notify_all():
     • Wakes all waiting threads.

5. Safe practices:
   - Always protect shared resource with a mutex.
   - Always use wait() with a predicate to avoid spurious wakeups.
   - Prefer unique_lock (not lock_guard) with wait(), since wait() needs lock.unlock() internally.

-----------------------------------------
EXAMPLES:

Case 1: Busy waiting (bad)
while(queue.empty()) { }

Case 2: With condition_variable (good)
cv.wait(lock, []{ return !queue.empty(); });

-----------------------------------------
KEY POINTS (for interview):
- Condition variables are for thread communication.
- Used when one thread needs to wait until another finishes some work.
- Always pair with a mutex + shared state.
- wait() puts thread to sleep efficiently, notify wakes it up.
- Use notify_one() when only one thread needs to wake; notify_all() when many may wait.

*/
