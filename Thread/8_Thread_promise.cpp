// Thread_promise.cpp
// clang++ -std=c++17 -pthread 8_Thread_promise.cpp -o a; ./a
// @author :  DhiraxD     
// @brief  : Getting return value from thread using std::promise and std::future
//           Demonstrates asynchronous communication and thread-safe data sharing
// https://en.cppreference.com/w/cpp/thread/promise 

#include <future>
#include <iostream>
#include <thread>
#include <exception>

// Task function that will wait for data from main thread using std::promise
int Operation(std::promise<int> & data) {
	using namespace std::chrono_literals;

	// Get the future associated with this promise
	auto f = data.get_future();  

	try {
		std::cout << "[Task] Waiting for count\n";

		// Block here until the main thread sets the value in the promise
		auto count = f.get();  
		std::cout << "[Task] Count acquired: " << count << "\n";

		int sum{};
		// Perform some computation using the received count
		for (int i = 0; i < count; ++i) {
			sum += i;
			std::cout << '.';  // Progress indicator
			std::this_thread::sleep_for(300ms); // Simulate work
		}
		return sum;

	} catch(std::exception &ex) {
		// If an exception is set in the promise, it will be rethrown here
		std::cout << "[Task] Exception: " << ex.what() << std::endl; 
		return -1;
	}
}

int main() {
	using namespace std::chrono_literals ;

	// Step 1: Create a promise object
	// std::promise allows one thread (main) to set a value
	// and another thread (task) to get the value asynchronously
	std::promise<int> data ;

	// Step 2: Launch a task asynchronously using std::async
	// Pass the promise by reference using std::ref
	std::future<int> result = std::async(std::launch::async, Operation, std::ref(data));

	// Simulate some delay before main thread sets the value
	std::this_thread::sleep_for(1s) ;
	std::cout << "[main] Setting the data in promise\n";

	try {
		// Step 3: Set the value in the promise
		// Once set, the future in the task thread becomes ready and get() unblocks
		data.set_value(10);  // can only be called once

		// Step 4: Retrieve result from the task
		if (result.valid()) {
			auto sum = result.get();  // blocks until task finishes
			std::cout << "\n[main] Sum computed by task = " << sum << std::endl;
		}

	} catch(std::exception &ex) {
		// If some error occurs in main while setting value, propagate exception to task
		data.set_exception(std::make_exception_ptr(ex));
	}

	return 0;
}


/*
🔑 Theory: std::promise and std::future
	1.	Promise-Future Concept
	•	Think of promise and future as two ends of a communication channel between threads.
	•	One thread stores a value in a promise; the other thread retrieves it via a future.
	•	They are thread-safe and synchronized automatically.
	2.	Single Use
	•	A promise can be used only once. You can either set_value() or set_exception(), but only once.
	•	Once a future retrieves the value, it is consumed and cannot be get() again.
	3.	Waiting for Data
	•	If the future calls .get() before the value is set, it blocks the thread until the value is available.
	•	This allows a task to wait for data produced by another thread safely.
	4.	Exception Handling
	•	Exceptions can also be communicated through the promise using set_exception().
	•	When .get() is called on the future, the exception is re-thrown in the receiving thread.
	5.	Use Case Example
	•	Main thread produces some data (count) after a delay.
	•	Task thread waits for count to start computation.
	•	Promise/future ensures safe sharing and synchronization without explicit mutexes.
	6.	Advantages
	•	Eliminates the need for low-level locks (mutexes) when sharing single values.
	•	Makes communication between threads clear and structured.
	•	Allows asynchronous notification: task only starts work after data is ready.
	7.	Timeline of Operation
    [Task] Waiting for count        ← Task blocks on future.get()
    1-second delay in main
    [main] Setting the data in promise  ← Main sets value
    [Task] Count acquired             ← Task unblocks
    Task computes sum ...
    [main] Sum computed by task       ← Main retrieves result via future.get()
    [Task] Finished processing        ← Task completes execution


    💡 Memory Aid / Analogy
	•	Promise = Mailbox (main thread puts a letter inside)
	•	Future = Recipient waiting for mail (task thread waits at mailbox)
	•	Once letter is delivered (value set), recipient can read it and the mailbox is empty (cannot read again).

⸻

This covers:
	•	How promise and future communicate between threads
	•	Blocking and synchronization
	•	Exception handling
	•	Single-use restriction
	•	Practical use in asynchronous computation

⸻
🔹 Summary
    •	std::promise and std::future provide a clean, thread-safe way to share single values between threads.
    •	They handle synchronization automatically, allowing one thread to wait for data from another without complex locking.
    •	Exceptions can be propagated safely across threads.
    •	Ideal for scenarios where one thread produces data that another thread needs to consume asynchronously.
*/