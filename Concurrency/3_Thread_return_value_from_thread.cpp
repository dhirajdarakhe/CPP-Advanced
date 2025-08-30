// Thread_return_value_from_thread.cpp
// clang++ -std=c++17 -pthread 3_Thread_return_value_from_thread.cpp -o a; ./a
// @author : DhiraxD
// @brief  : Getting return value from thread using std::async and std::packaged_task
// References:
// https://en.cppreference.com/w/cpp/thread/async
// https://en.cppreference.com/w/cpp/thread/future
// https://en.cppreference.com/w/cpp/thread/packaged_task

#include <iostream>
#include <thread>
#include <future>       

// -----------------------------------------------------------
// Sample functions to compute results
int Add(int a, int b) {
    std::cout << "Adding numbers..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10)); // simulate work
    return a + b;
}

int mul(int a, int b) {
    std::cout << "Multiplying numbers..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10)); // simulate work
    return a * b;
}

// -----------------------------------------------------------
// Case 1: Using std::async to run tasks asynchronously
int main1() {
    std::cout << "[main] User started an operation" << std::endl; 

    // std::async automatically runs the function asynchronously and returns a future
    std::future<int> result1 = std::async(std::launch::async, Add, 10, 20);
    std::future<int> result2 = std::async(std::launch::async, mul, 10, 20);

    std::cout << "[main] User started another operation" << std::endl;

    // .get() blocks until the result is ready
    std::cout << "Addition is : " << result1.get() << std::endl;
    std::cout << "Multiplication is : " << result2.get() << std::endl;
    std::cout << "we are done" << std::endl;      

    return 0;
}

// -----------------------------------------------------------
// Case 2: Synchronous execution using std::packaged_task
int main2() {
    // Wrap functions in packaged_task
    std::packaged_task<int(int,int)> task1{Add};
    std::packaged_task<int(int,int)> task2{mul};

    // Get futures to retrieve results later
    std::future<int> result1 = task1.get_future();
    std::future<int> result2 = task2.get_future();

    // Call tasks **synchronously** in main thread
    task1(10,20);
    task2(10,20);

    // .get() will return immediately because tasks are already executed
    std::cout<< result1.get() << std::endl;
    std::cout<< result2.get() << std::endl;

    return 0;
}

// -----------------------------------------------------------
// Case 3: Asynchronous execution using packaged_task + thread
int main() {
    // Wrap functions in packaged_task
    std::packaged_task<int(int,int)> task1{Add};
    std::packaged_task<int(int,int)> task2{mul};

    // Get futures to retrieve results later
    std::future<int> result1 = task1.get_future();
    std::future<int> result2 = task2.get_future();

    // Run packaged_task in separate threads
    std::thread t1{std::move(task1),10,20};
    std::thread t2{std::move(task2),10,20};

    // .get() blocks until result is ready
    std::cout<< "Addition is : " << result1.get() << std::endl;
    std::cout<< "Multiplication is : " << result2.get() << std::endl;

    std::cout<<"main thread is waiting for child threads to complete"<<std::endl;

    // Wait for threads to finish
    t1.join();
    t2.join();

    std::cout<< "we are done" << std::endl;

    return 0;
}

/* ------------------------------------------------------------------
THEORY & KEY POINTS: Return value from threads

1. std::async:
   - High-level way to run a function asynchronously.
   - Returns a std::future, which allows main thread to get the result later.
   - Exceptions in async functions are propagated through future.get().

2. std::packaged_task:
   - Wraps a callable object (function, lambda, functor) and connects it to a future.
   - You can execute it **synchronously** in main thread or **asynchronously** using std::thread.
   - Provides more control over task execution compared to std::async.

3. Case 1: Sequential async
   - Main thread launches tasks via async.
   - Main can continue other work until future.get() is called.
   - Tasks run in background threads.

4. Case 2: Synchronous packaged_task
   - Tasks executed in main thread.
   - Slower if multiple tasks exist.
   - Useful when you want main thread to control execution order.

5. Case 3: Asynchronous packaged_task + thread
   - Tasks run in separate threads.
   - Main thread blocks only when calling future.get().
   - Faster because tasks can run concurrently.

6. Future & get():
   - std::future stores the eventual result of a task.
   - .get() blocks until the result is ready.
   - Once get() is called, future becomes invalid (cannot call again).

7. Exceptions:
   - Exceptions thrown inside the task propagate through future.get().

8. Rule of thumb:
   - Use std::async for **simple task-based concurrency**.
   - Use std::packaged_task + thread when you need **manual control** over threads and task execution.

------------------------------------------------------------------

💡 Memory Tip / Analogy
	•	std::async = “order food online → delivery comes when ready” (automatic threading + future).
	•	std::packaged_task = “cook food yourself → you control when and how it’s executed” (manual threading + future).

*/

 /*
 std::packaged_task is a C++ wrapper around a callable object (like a function, lambda, or functor) that connects it to a std::future.
 
 It allows you to execute a function asynchronously and retrieve its result later.
 


Case 1: main thread runs everything sequentially → slower if you have multiple tasks.
int main() {
    // Step 1: Wrap the function
    std::packaged_task<int(int,int)> task(Add);

    // Step 2: Get future
    std::future<int> result = task.get_future();

    // Step 3: Run task (synchronously here)
    task(10, 20); < main thread runs the task > main thread runs the task
    // task 1   main thread runs the task 2sec
    // task 2   main thread runs the task 2sec
    // task 3   main thread runs the task 2sec
    // main thread runs the task 8sec
    // Step 4: Get the result
    std::cout << "Result = " << result.get() << std::endl;
}

Case 2: worker threads run tasks concurrently → faster overall (main thread freed up).

int main() {
    std::packaged_task<int(int,int)> task(mul);
    std::future<int> result = task.get_future();

    // Run packaged_task in a new thread
    std::thread t(std::move(task), 10, 20); // new thread runs the task  2sec
    // task 1   main thread runs the task 2sec
    // task 2   main thread runs the task 2sec
    // task 3   main thread runs the task 2sec
    // thread runs the task 6sec
    // Get the result (blocks until ready)
    std::cout << "Multiplication = " << result.get() << std::endl;

    t.join();
}
 */
