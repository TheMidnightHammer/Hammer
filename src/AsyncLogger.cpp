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
        std::unique_lock<std::mutex> lock(queueMutex);
        // Wait until there is a log message or the logger is stopped
        cv.wait(lock, [this] { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            // Use standard \n, NEVER std::endl here to keep the background thread fast too
            std::cout << logQueue.front() << "\n";
            logQueue.pop();
        }
    }
}

HammerAsyncLogger::HammerAsyncLogger() {
    // make the background thread immediately
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