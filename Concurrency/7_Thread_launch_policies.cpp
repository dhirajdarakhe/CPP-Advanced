// Thread_launch_policies.cpp
// clang++ -std=c++17 -pthread 7_Thread_launch_policies.cpp -o a; ./a
// @author :  DhiraxD
// @brief  : Understanding std::launch policies with std::async
//
// Key references:
// https://en.cppreference.com/w/cpp/thread/async
// https://en.cppreference.com/w/cpp/thread/future
// https://en.cppreference.com/w/cpp/thread/launch
//
// ---------------------------------------------------------
// std::async() allows you to run a function asynchronously
// and it returns a std::future to retrieve the result later.
//
// std::launch policies control HOW the function is run:
//
// 1. std::launch::async   → runs in a NEW thread immediately
// 2. std::launch::deferred→ runs ONLY when .get() or .wait() is called (synchronously, in same thread)
// 3. std::launch::async | std::launch::deferred → implementation decides (default behavior)
//
// ---------------------------------------------------------

#include <future>
#include <iostream>
#include <thread>

// A dummy function that simulates "work"
int Operation(int count)
{
    using namespace std::chrono_literals;
    int sum{};
    for (int i = 0; i < count; ++i)
    {
        sum += i;
        std::cout << '.'; // progress indicator
        std::this_thread::sleep_for(300ms); // simulate work (0.3s per iteration)
    }
    return sum;
}

int main()
{
    using namespace std::chrono_literals;

    // Launch Operation(10) asynchronously with std::launch::async
    // → forces it to run on a NEW thread immediately
    std::future<int> result = std::async(std::launch::async, Operation, 10);

    // Simulate main thread doing something else for 1 second
    std::this_thread::sleep_for(1s);
    std::cout << "main() thread continues execution...\n";

    // Always check if the future is valid before using it
    if (result.valid())
    {
        // Set a deadline (timepoint = now + 1s)
        auto timepoint = std::chrono::system_clock::now();
        timepoint += 1s;

        // Check status of the async task
        // wait_until(timepoint) waits until either:
        // - task finishes
        // - or timeout occurs
        auto status = result.wait_until(timepoint);

        // You can also use wait_for(duration) instead of wait_until(timepoint)

        switch (status)
        {
        case std::future_status::deferred:
            // Task was launched with std::launch::deferred → not started yet
            std::cout << "Task is synchronous (deferred)\n";
            break;

        case std::future_status::ready:
            // Task has finished execution and result is ready
            std::cout << "Result is ready\n";
            break;

        case std::future_status::timeout:
            // Task is still running, not finished yet
            std::cout << "Task is still running\n";
            break;
        }

        // Finally, get the result (blocks until task is done if not ready)
        auto sum = result.get();
        std::cout << "Final sum = " << sum << std::endl;
    }
}


/*
🔑 Things to Remember:
	1.	Launch policies
	•	async → starts immediately on a new thread.
	•	deferred → runs only when .get() or .wait() is called.
	•	default → implementation may choose either.
	2.	Future states
	•	future_status::ready → result available.
	•	future_status::timeout → still running.
	•	future_status::deferred → hasn’t run yet, waiting for .get()/.wait().
	3.	Best practices
	•	Always check result.valid() before .get().
	•	Use wait_until or wait_for if you don’t want to block indefinitely.
	•	.get() can be called only once.

⸻
🔹 Real-world analogy:
    •	std::launch::async = ordering food at a restaurant (it starts being prepared right away in the kitchen).
    •	std::launch::deferred = cooking it yourself at home (you only start when you decide to eat).
    •	std::launch::async | std::launch::deferred = ordering takeout (the restaurant may prepare it immediately or wait until you arrive).

*/
