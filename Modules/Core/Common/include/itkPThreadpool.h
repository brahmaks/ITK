#ifndef __itkPthreadPool_h
#define __itkPthreadPool_h


#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <vector>


namespace itk {


class WorkerPThread {
public:
    struct ThreadArgs {

        void *otherArgs;
    };
    int id;
    bool assigned;
    bool executed;
    pthread_t ptid;
    void *UserData; //any data user wants the function to use

    void* (*ThreadFunction)(void *ptr);
    pthread_t threadHandle;
    ThreadArgs threadArgs;
    WorkerPThread() : assigned(false) {

        std::cout << "Starting thread \t address=" << this << std::endl;
    }
    ~WorkerPThread() {
        std::cout <<std::endl<< "Thread finished. pid is  " << ptid << "\t address=" << this << std::endl;
    }


};

class PThreadPool
{
public:


    static PThreadPool & New();
    static PThreadPool & New(int maxThreads);
    static void deleteInstance();
    void DestroyPool(int maxPollSecs);
    int AssignWork(WorkerPThread worker);
    WorkerPThread FetchWork();
    void InitializeThreads();
    static void *ThreadExecute(void *param);
    static pthread_mutex_t mutexSync;
    static pthread_mutex_t activeThreadMutex;
    static pthread_mutex_t mutexWorkCompletion;
    pthread_t *threadHandles;
    bool WaitForThread(int id);



private:
    bool destroy;
    static PThreadPool* pThreadPoolInstance;
    static bool instanceFlag;
    PThreadPool(PThreadPool const&);             // copy constructor is private
    PThreadPool& operator=(PThreadPool const&);  // assignment operator is private
    PThreadPool();
    ~PThreadPool();
    PThreadPool(int maxThreadsTemp);
    void removeActiveId(int id);
    int maxThreads;
    pthread_cond_t  condCrit;
    sem_t workAvailable;
    std::vector<WorkerPThread> workerQueue;
    std::vector<int> activeThreadIds;
    int incompleteWork;
    int queueSize;
    int idCounter;

};



}
#endif
