#include "stdafx.h"


#include <iostream>

#include "JobQueue.h"


void testConstructor()
{
    JobQueue jobQeue;
}


void testEnqueue()
{
    JobQueue jobQueue;

    const std::string data = "AJOB";

    for (size_t i = 0; i < 10; ++i) {
        jobQueue.enqueu(data);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}



void dev_test()
{
    //testConstructor();

    testEnqueue();
}


int main()
{
    std::cout << "Job Queue dev test\n";

    dev_test();

    return 0;
}