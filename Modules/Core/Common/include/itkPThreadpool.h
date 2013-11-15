#ifndef __itkPthreadPool_h
#define __itkPthreadPool_h


#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <vector>

#include "itkLightObject.h"


//using namespace std;

namespace itk {


class WorkerPThread {
public:
    struct ThreadArgs{
        
	void *otherArgs;
    };
    int id;
    pthread_t ptid;
    void *UserData; //any data user wants the function to use
    int ActiveFlag; 
    void* (*ThreadFunction)(void *ptr);
    pthread_t threadHandle;
    ThreadArgs threadArgs;
    WorkerPThread(int myId) : id(myId), ActiveFlag(1) {}
    virtual ~WorkerPThread() {}
};

class PThreadPool// : public LightObject
{
public:

    static PThreadPool * New();
    static PThreadPool * New(int maxThreads);
    
    void Delete() { delete this; }

    virtual ~PThreadPool();

    void DestroyPool(int maxPollSecs);
    bool AssignWork(WorkerPThread *worker, bool wait);
    bool FetchWork(WorkerPThread **worker);
    void InitializeThreads();
    WorkerPThread* getWorker(pthread_t *id);
    static void *ThreadExecute(void *param);
    static pthread_mutex_t mutexSync;
    static pthread_mutex_t mutexWorkCompletion;
    
    //ThreadArgs **threadArgs;// = new ThreadArgs;
    pthread_t *threadHandles;
   
    bool WaitForThread(WorkerPThread **workerThread);



private:
    PThreadPool();
    PThreadPool(int maxThreadsTemp);
    int maxThreads;
    pthread_cond_t  condCrit;
    sem_t availableWork;
    sem_t availableThreads;
    std::vector<WorkerPThread *> workerQueue;
    int topIndex;
    int bottomIndex;
    int incompleteWork;
    int queueSize;
    bool waitFlag;
 
};



}
#endif
