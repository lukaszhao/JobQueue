#pragma once

#include <vector>

#include <string>

#include <thread>

#include <mutex>

#include <condition_variable>

#include <functional>

#include <atomic>

#include <chrono>


#include <iostream>



            // ==============
            // class JobQueue
            // ==============


class JobQueue
{
    using DataType = std::string;      // this is the data type stored in d_dataQueue

private:

    std::vector<DataType>                      d_dataQueue;

    std::condition_variable                    d_conditionVar;

    std::function<void(const std::string&)>    d_errorCallback;

    std::atomic<bool>                          d_exitFlag;

    std::mutex                                 d_dataQueueMutex;

    std::mutex                                 d_conditionVarMutex;



    static void defaultErrorCallback(const std::string& errorMsg);
        // if user doesn't pass in an errorCallback function, will use this function as default error callback
        // this function simply prints out errorMsg

    
    void doWorkWhenConditionVarNotifies();
        // do work when condition variable notifies
        // this function is for the child thread's job


    inline bool shouldWakeUp();
        // when thread wakes up, call this function to check if thread should really wake up
        // this aproach combats thread spurious wake up
        // function returns true if there really are jobs to do (if d_dataQueue is not empty)
        // function also returns true if d_exitFlag is true


    void processData(const std::vector<DataType>& dataToProcess);
        // process data, this function is the actual work being done to data
        // at the testing stage, we will simply print out data

    

    JobQueue(const JobQueue&) = delete;

    JobQueue& operator=(const JobQueue&) = delete;


public:
    JobQueue(const std::function<void(const std::string&)>& errorCallback =
                     std::bind(&JobQueue::defaultErrorCallback,
                               std::placeholders::_1)
             );

    ~JobQueue();

    void enqueu(const DataType& data);
        // push data into d_dataQueue, and condition variable signals
        
};

