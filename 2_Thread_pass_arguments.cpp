// Thread_pass_arguments.cpp
// clang++ -std=c++17 -pthread 2_Thread_pass_arguments.cpp -o a; ./a
// @author : Dhiraj Darakhe
// @date   : 2024-06-10
// @brief  Passing arguments to thread functions
//
// References:
// https://en.cppreference.com/w/cpp/thread/thread/thread
// https://en.cppreference.com/w/cpp/thread/ref
// https://en.cppreference.com/w/cpp/thread/cref
// https://en.cppreference.com/w/cpp/thread/joinable    

#include <iostream>
#include <list>
#include <thread>
#include <string>
#include <functional>

std::list<int> g_Data;
const int SIZE = 5000000;

// -----------------------------------------------------------
// Example class to demonstrate object passing
class String {
public:
    String() {
        std::cout << "String()" << std::endl;
    }
    String(const String &) {
        std::cout << "String(const String&)" << std::endl;
    }
    String & operator=(const String&) {
        std::cout << "operator=(const String&)" << std::endl;
        return *this;
    }
    ~String() {
        std::cout << "~String()" << std::endl;
    }
};

// -----------------------------------------------------------
// Download function takes a reference to String object
// This avoids unnecessary copying of the object
void Download(String &fileName) {
    std::cout << "[Downloader] Started download of file :" << std::endl; 
    for (int i = 0; i < SIZE; ++i) {
        g_Data.push_back(i);
    }
    std::cout << "[Downloader] Finished download" << std::endl; 
}

// -----------------------------------------------------------
int main() {
    std::cout << "[main] User started an operation" << std::endl; 

    String file;  // create a String object

    // Step 1: Pass arguments to thread function
    // std::ref(file) ensures the thread receives a reference instead of copying the object
    std::thread threadObj(Download, std::ref(file));

    std::cout << "[main] User started another operation" << std::endl;

    // Step 2: Wait for the thread to finish
    if(threadObj.joinable()) {
        threadObj.join();  // main thread blocks until Download() completes
    }

    std::cout << "we are done" << std::endl;

    return 0;
}

/*
-----------------------------------------
THEORY: Passing arguments to thread functions
-----------------------------------------

🔑 Key Points & Theory
	1.	Default Behavior
	•	When passing objects to a thread, C++ copies the object by default.
	•	This triggers constructor/copy constructor calls, which may be expensive for large objects.
	2.	Passing by Reference
	•	Use std::ref() to pass a non-const reference.
	•	Use std::cref() to pass a const reference.
	•	This avoids copying and allows the thread to modify the original object if needed.
	3.	Object Lifetime
	•	Ensure the referenced object lives longer than the thread.
	•	If the object is destroyed while the thread still accesses it, this causes undefined behavior.
	4.	Thread Safety
	•	If multiple threads modify the same object (like g_Data), protect it with mutexes.
	5.	Why Join?
	•	join() ensures the main thread waits for the worker thread to finish.
	•	Without join/detach, destroying a joinable thread triggers std::terminate().
	6.	Observed Output
	•	Only one default constructor call because we pass by reference.
	•	Copy constructor calls are avoided.
	•	Destructor called at the end when file goes out of scope.

⸻

✅ Summary
    •	Use std::ref()/std::cref() to pass references to thread functions.
    •	Ensure object lifetime exceeds thread duration.
    •	Protect shared data with mutexes.
    •	Always join or detach threads to avoid crashes.


💡 Memory Tip / Analogy
	•	std::thread copying args = giving the thread its own copy of the book.
	•	std::ref() = thread reads/writes your original book.
	•	std::cref() = thread reads the original book but cannot modify it.


*/