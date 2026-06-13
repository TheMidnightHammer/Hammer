#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "../include/HammerEngine/HammerEngine.h"

void HammerAsyncLogger::ProcessLogs() {
    while (running || !logQueue.empty()) {
        std::queue<std::string> localQueue;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this] { return !logQueue.empty() || !running; });
            std::swap(logQueue, localQueue);
        }
        while (!localQueue.empty()) {
            std::cout << localQueue.front() << "\n";
            localQueue.pop();
        }
    }
}

HammerAsyncLogger::HammerAsyncLogger() {
    running = true;
    workerThread = std::thread(&HammerAsyncLogger::ProcessLogs, this);
}

HammerAsyncLogger::~HammerAsyncLogger() {
    running = false;
    cv.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void HammerAsyncLogger::Log(const std::string& message) {
    std::lock_guard<std::mutex> lock(queueMutex);
    logQueue.push(message);
    cv.notify_one();
}