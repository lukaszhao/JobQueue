// JobQueue.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "JobQueue.h"






JobQueue::JobQueue(const std::function<void(const std::string&)>& errorCallback
                   ): d_errorCallback(errorCallback),
                      d_exitFlag(false)
{
    std::cout << "\n[constructor] start thread... \n";

    std::function<void()> doWork = std::bind(&JobQueue::doWorkWhenConditionVarNotifies,
                                             this);

    std::thread jobThread(doWork);

    jobThread.detach();
}


JobQueue::~JobQueue()
{
    std::cout << "\n[destructor] set exit flag true, condition variable notifies...\n";

    d_exitFlag.store(true);

    d_conditionVar.notify_one();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\n[destructor] exit\n";
}



void JobQueue::defaultErrorCallback(const std::string& errorMsg)
{
    std::cout << "\n[default error callback] errorMsg = " << errorMsg << "\n";
}



void JobQueue::doWorkWhenConditionVarNotifies()
{
    while (d_exitFlag.load() == false) {

        std::vector<DataType> dataToProcess;
        
        std::unique_lock<std::mutex> lockForConditonVar(d_conditionVarMutex);

        d_conditionVar.wait(lockForConditonVar, [this] {return this->shouldWakeUp();});

        bool dataQueueIsEmpty;

        do {
            { // lock mutex for data queue
                std::unique_lock<std::mutex> lockForDataQueue(d_dataQueueMutex);
                dataToProcess.clear();
                dataToProcess.swap(d_dataQueue);
            } // unlock mutext for data queue, so new data can be enqueue now

            // process all data units in dataToProcess
            processData(dataToProcess);

            // check if thre is more data in d_dataQueue (may be added after above swap)
            {
                std::unique_lock<std::mutex> lockForDataQueue(d_dataQueueMutex);
                dataQueueIsEmpty = d_dataQueue.size() == 0;
            }

        } while(!dataQueueIsEmpty);
    }
}



bool JobQueue::shouldWakeUp()
{
    return d_dataQueue.size() != 0 || d_exitFlag.load();
}



void JobQueue::processData(const std::vector<DataType>& dataToProcess)
{
// process data, this function is the actual work being done to data
// at the testing stage, we will simply print out data

    std::cout << "\n[DEBUG] processData(...) called, dataToProcess.size() = "
              << dataToProcess.size() << "\n";

    if (dataToProcess.size() == 0) {
        std::cout << "\ndataToProcess is empty, nothing to do!\n";
        return;
    }

    for (std::vector<DataType>::const_iterator itr = dataToProcess.cbegin();
         itr != dataToProcess.cend();
         ++itr)
    {
        std::cout << "\n      Processing data: " << *itr << "\n";
    }
}



void JobQueue::enqueu(const DataType& data)
{
    //std::cout << "[DEBUG] enqueue(...) called\n";
    
    if (d_exitFlag.load() == true) {
        std::cout << "[Warning] exit flag is true, decline enqueue new data! data = " << data
                  << "\n";
        return;
    }
    
    {
        std::unique_lock<std::mutex> lockForDataQueue(d_dataQueueMutex);
        d_dataQueue.push_back(data);
    } // unlock d_dataQueueMutex

    d_conditionVar.notify_one();

    //std::cout << "[DEBUG] notified!\n";
}

