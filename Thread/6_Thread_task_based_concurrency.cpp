// Thread_task_based_concurrency.cpp
// clang++ -std=c++17 -pthread 6_Thread_task_based_concurrency.cpp -o a; ./a
// @author :  DhiraxD
// @brief  : Task-based concurrency in C++ using std::async and std::future
//
// References:
// https://en.cppreference.com/w/cpp/thread/thread
// https://en.cppreference.com/w/cpp/thread/joinable
// https://en.cppreference.com/w/cpp/thread/detach
// https://en.cppreference.com/w/cpp/thread/join
// https://en.cppreference.com/w/cpp/thread/ref
// https://en.cppreference.com/w/cpp/thread/cref
// https://en.cppreference.com/w/cpp/thread/async
// https://en.cppreference.com/w/cpp/thread/future      

#include <iostream>
#include <thread>
#include <future>

// -----------------------------------------------------------
// Simulate a task that adds two numbers
int Add(int a, int b) {
    std::cout << "[Add] Adding numbers..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10)); // simulate long-running task
    return a + b;
}

// Simulate a task that multiplies two numbers
int mul(int a, int b) {
    std::cout << "[mul] Multiplying numbers..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(20)); // simulate longer task
    return a * b;
}

// -----------------------------------------------------------
int main() {
    std::cout << "[main] User started an operation" << std::endl; 

    // -------------------------------
    // Task-based concurrency
    // -------------------------------
    // Launch Add and mul asynchronously using std::async with std::launch::async
    // Each call returns a std::future which holds the eventual result
    std::future<int> result1 = std::async(std::launch::async, Add, 10, 20);
    std::future<int> result2 = std::async(std::launch::async, mul, 10, 20);

    std::cout << "[main] User started another operation" << std::endl;
    // At this point:
    // - Add() runs in a separate thread
    // - mul() runs in another separate thread
    // - Main thread continues executing concurrently

    // -------------------------------
    // Retrieve results from futures
    // -------------------------------
    // .get() blocks until the task completes and returns the result
    std::cout << "[main] Addition is : " << result1.get() << std::endl;
    std::cout << "[main] Multiplication is : " << result2.get() << std::endl;

    // Once both futures are retrieved, the tasks are completed and cleaned up
    std::cout << "[main] we are done" << std::endl;      

    return 0;
}

/* ------------------------------------------------------------------
🔑 Key Takeaways:

1. std::async with std::launch::async → starts a new thread immediately.
2. std::future → holds the eventual result of the asynchronous task.
3. .get() → blocks until the task is finished; retrieves the result.
4. No need to manually join() the threads; async + future handle it automatically.
5. std::launch::deferred → task executes only when .get() is called.
6. Exceptions thrown inside async tasks propagate to the calling thread via future.get().
7. Multiple async tasks can be managed with a vector of futures.
8. Always check if a future is valid before calling .get().

--------------------------------------------------------------
Comparison: std::async vs std::thread

Aspect                | std::async                       | std::thread
----------------------|---------------------------------|--------------------------------
What it does          | Runs function asynchronously     | Creates a new thread to run function
Return value          | Automatic via std::future       | Manual (use promise/packaged_task or shared data)
Exception handling    | Propagated through future.get() | Must catch inside thread manually
Thread management     | Automatic, no join() needed     | Must join() or detach() manually
Ease of use           | High-level, safe                | Low-level, more control
Flexibility           | Less control over scheduling    | Full control (join/detach, affinity)
Use case              | Simple task-based concurrency   | Long-running worker threads, thread pools
Overhead              | Slightly higher (future setup) | Leaner, more code to manage results

✅ Rule of Thumb:
- Use std::async for task-based concurrency where you need results.
- Use std::thread for fine-grained control over threads.

🔹 Analogy:
- std::async = "Order food online → delivery arrives asynchronously"
- std::thread = "Cook food yourself → you control timing and process"
------------------------------------------------------------------

💡 Notes to Remember
	•	std::async handles thread management for you → no join/detach needed.
	•	std::future acts as a communication channel between main thread and task.
	•	Multiple tasks can be launched concurrently → main thread continues until results are needed.
	•	Use std::launch::deferred if you want the task to run lazily, only when .get() is called.

⸻
🔹 Real-world analogy:
    •	std::async = ordering food at a restaurant (it starts being prepared right away in the kitchen).
    •	std::thread = cooking food yourself (you control when and how it’s done).

*/