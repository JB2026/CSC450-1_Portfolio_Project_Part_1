#include<iostream>
#include<mutex>
#include<chrono>
#include<thread>
#include<condition_variable>

// Standard namespace declaration
using namespace std;

/// A class that is a thread safe counter that can be incremented or decremented
class Counter {
    // Declare the class properties
    private: int counter = 0;
    private: std::mutex counterLock;
    
    /// A method to safely increment the counter
    public: void incrementCounter() {
        std::lock_guard<std::mutex> lock(counterLock);
        counter++;
    }
    
    /// A method to safely decrement the counter
    public: void decrementCounter() {
        std::lock_guard<std::mutex> lock(counterLock);
        counter--;
    }
    
    /// A method to safely get the current counter value
    public: int getCounterValue() {
        std::lock_guard<std::mutex> lock(counterLock);
        return counter;
    }
};

/// A method to increment the local counter the number of times passed in
void incrementCounter(int numberOfTimes);

/// A method to decrement the local counter the number of times passed in
void decrementCounter(int numberOfTimes);

// Create the lock, conditional variable, and counter
std::mutex mutexLock;
std::condition_variable condVar;
bool incrementerFinished = false;
Counter counter;

int main() {
    // Create the threads and call the counter methods with the number of iterations
    std::thread incrementThread(incrementCounter, 20);
    std::thread decrementThread(decrementCounter, 20);

    // Join the threads
    incrementThread.join();
    decrementThread.join();

    // Main Function return Statement
    return 0;
}

void incrementCounter(int numberOfTimes) {
    // Create a lock and immediately lock to stop other thread access
    std::unique_lock<std::mutex> lock(mutexLock);
    
    // Ouput the we are incrementing and the current counter value
    std::cout<< "Incrementing" <<std::endl;
    std::cout<< counter.getCounterValue() <<std::endl;

    // For the number of times passed in, increment the counter
    for (int i = 0; i < numberOfTimes; i++) {
        counter.incrementCounter();
        std::cout<< counter.getCounterValue() <<std::endl;
    }
    
    // Update the increment finished flag and also notify the conditional variable
    incrementerFinished = true;
    condVar.notify_all();
}

void decrementCounter(int numberOfTimes) {
    // Create the lock and lock it
    std::unique_lock<std::mutex> lock(mutexLock);

    // Use the conditional variable and boolean to wait on the increment thread to finish
    // Then get the lock, lock it, and execute the rest of themethod
    condVar.wait(lock, [] {
        return incrementerFinished;
    });
    
    // Ouput the we are incrementing and the current counter value
    std::cout<< "\nDecrementing" <<std::endl;
    std::cout<< counter.getCounterValue() <<std::endl;

    for (int i = 0; i < numberOfTimes; i++) {
        counter.decrementCounter();
        std::cout<< counter.getCounterValue() <<std::endl;
    }
}
