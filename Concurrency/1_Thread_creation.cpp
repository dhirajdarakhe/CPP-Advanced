// Thread_creation.cpp
// clang++ -std=c++17 -pthread 1_Thread_creation.cpp -o a; ./a
// @author :  DhiraxD
// @brief  Creating and managing threads using detach()
// References:
// https://en.cppreference.com/w/cpp/thread/thread/detach
// https://en.cppreference.com/w/cpp/thread/joinable
// https://en.cppreference.com/w/cpp/thread/join        

#include <iostream>
#include <list>
#include <thread>
#include <string>
#include <functional>

// Global shared data
std::list<int> g_Data;
const int SIZE = 5000000;

// Function to simulate downloading a file
void Download() {
    std::cout << "[Downloader] Started download of file" << std::endl; 
    for (int i = 0; i < SIZE; ++i) {
        g_Data.push_back(i);  // Append data (shared resource)
    }
    std::cout << "[Downloader] Finished download" << std::endl; 
}

int main() {
    std::cout << "[main] User started an operation" << std::endl; 

    // Step 1: Create a new thread to run Download() function
    std::thread threadObj(Download);
    // At this point:
    // - A new thread is created
    // - It starts executing Download() in parallel with main()
    
    std::cout << "[main] User started another operation" << std::endl;

    // Option 1: Detach the thread
    // threadObj.detach();
    // - Detaches the thread from threadObj
    // - The thread runs independently in the background
    // - Main thread cannot join or control this thread anymore
    // - If main() exits before detached thread finishes, it is forcibly terminated

    // Option 2: Join the thread
    // if(threadObj.joinable()) {
    //     threadObj.join();
    // }
    // - Main thread waits here until Download() finishes
    // - Ensures thread completes before main exits
    // - Safe way to manage threads

    std::cout << "we are done" << std::endl;

    // Step 3: Return from main
    // Note: If threadObj was not joined or detached, destroying std::thread object
    // while it is still joinable will call std::terminate() → program crashes

    return 0;
}

/*
-----------------------------------------
THEORY: Using std::thread and detach()
-----------------------------------------

1. Thread creation:
   - std::thread threadObj(Download);
   - Creates a new thread immediately
   - Function Download() starts running in parallel

2. Thread join:
   - threadObj.join();
   - Main thread waits until the child thread finishes
   - Ensures all threads complete safely
   - Prevents program from crashing

3. Thread detach:
   - threadObj.detach();
   - Makes thread independent
   - Main thread does not wait
   - Detached thread runs until it finishes or process exits
   - Risk: If main exits early, thread may be killed abruptly

4. Unsafe case:
   - If a std::thread object is destroyed without join() or detach(),
     program terminates with std::terminate()

5. Key points:
   - Always join() or detach() threads to avoid undefined behavior
   - Detach = "fire and forget" → thread runs independently
   - Join = wait for completion → safe and controlled

-----------------------------------------
EXAMPLES:

Case 1: No join or detach
std::thread threadObj(Download);
// No join/detach → UNSAFE, may crash on program exit

Case 2: Detached thread
threadObj.detach();
// Main continues, thread runs in background
// May be killed if main exits early

Case 3: Joined thread
if(threadObj.joinable()) threadObj.join();
// Main waits, ensures thread finishes safely
*/


/*
If the main thread finishes and the process terminates 
before the detached thread finishes, the detached thread is forcibly terminated.


What detach() does
	1.	After calling threadObj.detach(), the thread runs independently in the background.
	2.	The std::thread object no longer represents the thread — it cannot be joined or controlled anymore.
	3.	The detached thread will continue running until it finishes, or until the program exits.

    case 1 Your current program (without detach or join)
    std::thread threadObj(Download);
    // threadObj.detach();
    // if(threadObj.joinable()) {
    //     threadObj.join();
    // }

    •	You create a thread threadObj to run Download().
	•	But you don’t detach or join the thread.
	•	Problem: When main() returns, threadObj is still a live thread.
	•	In C++, if a std::thread object is destroyed while still joinable (not detached), your program terminates with std::terminate().
	•	So this program is actually unsafe and may crash


    case 2  With detach()
    threadObj.detach();

    •	The thread runs independently.
	•	main() doesn’t wait for it.
	•	If main() exits before the download finishes, the thread is forcibly terminated.
	•	No crash, but you might not see the download complete.

*/
