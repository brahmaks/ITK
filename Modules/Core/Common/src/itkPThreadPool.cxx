#include <stdlib.h>
#include "itkPThreadpool.h"




namespace itk {

pthread_mutex_t PThreadPool::mutexSync = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PThreadPool::mutexWorkCompletion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PThreadPool::activeThreadMutex = PTHREAD_MUTEX_INITIALIZER;

PThreadPool* PThreadPool::pThreadPoolInstance = 0;
bool PThreadPool::instanceFlag = false;

PThreadPool & PThreadPool::New()
{
    if(!instanceFlag) {
        pThreadPoolInstance = new PThreadPool(2);
    }
    return *pThreadPoolInstance;
}

PThreadPool & PThreadPool::New(int maxThreads)
{
    if(!instanceFlag) {
        pThreadPoolInstance = new PThreadPool(maxThreads);
    }
    return *pThreadPoolInstance;
}

PThreadPool::PThreadPool() : idCounter(1)
{
    PThreadPool(2); //will create a thread pool with at least 2 threads
}

PThreadPool::PThreadPool(int maximumThreads) : idCounter(1)
{
    instanceFlag = true;
    //need at least 1 thread
    if (maximumThreads < 1)
        maximumThreads=1;
    /*
    This check is not needed since we are detecting the num of threads in the MultiThreader
        if(maximumThreads > 25)
            maximumThreads = 25;
    */

    std::cout<<"Created thread pool with threads :"<<maximumThreads<<std::endl;

    this->maxThreads = maximumThreads;
    this->queueSize = maximumThreads;
    destroy = false;
    threadHandles = new pthread_t[maxThreads];

    incompleteWork = 0;
    sem_init(&workAvailable, 0, 0);
    InitializeThreads();

}

void PThreadPool::InitializeThreads()
{
    destroy = false;
    threadHandles = new pthread_t[maxThreads];
    for(int i = 0; i<maxThreads; i++)
    {
        int rc;
        rc = pthread_create(&threadHandles[i], NULL, &PThreadPool::ThreadExecute, (void *)this );
        if (rc) {
            std::cout<<"ERROR; return code from pthread_create() is "<<rc<<std::endl;
            exit(-1);
        }
        std::cout<<"Thread createdd with ptid :"<<threadHandles[i]<<std::endl;
    }

}

//Can call if needed to delete the created singleton instance
void PThreadPool::deleteInstance() {
    if(instanceFlag == true) {
        std::cout <<"Deleting instance"<<std::endl;
        instanceFlag = false;

        pThreadPoolInstance->workerQueue.clear();

        delete[] pThreadPoolInstance->threadHandles;

        delete pThreadPoolInstance;

        std::cout << "Destroyed" << std::endl;
        pThreadPoolInstance->DestroyPool(2);


    }
}


PThreadPool::~PThreadPool()
{

}


/*
	This method first checks if all threads are finished executing their jobs.
	If now, it will wait for "poolTime" seconds and then re-check.
	Once all are done it will destroy the thread pool.
*/
void PThreadPool::DestroyPool(int pollTime = 2)
{
    destroy = true;
    while( incompleteWork>0 )
    {
        std::cout << "Work is still incomplete=" << incompleteWork << std::endl;
        sleep(pollTime);
    }

    std::cout << "All threads are done" << std::endl<<"Terminating threads"<<std::endl;
    for(int i = 0; i<maxThreads; i++) {
        int s = pthread_cancel(threadHandles[i]);
        if(s!=0)
            std::cout << "Cannot cancel thread with id : "<<threadHandles[i]<<std::endl;
    }

    sem_destroy(&workAvailable);

    pthread_mutex_destroy(&mutexSync);
    pthread_mutex_destroy(&mutexWorkCompletion);
}


int PThreadPool::AssignWork(WorkerPThread workerThread)
{

    pthread_mutex_lock(&mutexWorkCompletion);
    incompleteWork++;
    pthread_mutex_unlock(&mutexWorkCompletion);

    pthread_mutex_lock(&mutexSync);
    //adding to queue
    workerThread.id = idCounter ++;
    workerQueue.push_back(workerThread);
    std::cout << "Assigning Worker[" << (workerThread).id << "] Address:[" << &workerThread << "] to Queue " << std::endl;
    pthread_mutex_unlock(&mutexSync);

    pthread_mutex_lock(&activeThreadMutex);
    std::cout<<std::endl<<"Adding id "<<workerThread.id<<" to activeThreadIds"<<std::endl;
    activeThreadIds.push_back(workerThread.id);
    std::cout<<std::endl<<"ActiveThreadids size : "<<activeThreadIds.size()<<std::endl;
    pthread_mutex_unlock(&activeThreadMutex);

    sem_post(&workAvailable);

    return (workerThread).id;
}

bool PThreadPool::WaitForThread(int id)
{
    bool found = true;
    while(found) {
        pthread_mutex_lock(&activeThreadMutex);
        /*
         We know that a thread id will be in here because the AssignWork fn adds it in
         So if a thread id is not found in activeThreadIds it means it is done.
        */
        found = false;
        for(std::vector<int>::size_type i = 0; i != activeThreadIds.size(); i++) {

            if(activeThreadIds[i] == id) {
                found = true;
                break;
            }
        }
        pthread_mutex_unlock(&activeThreadMutex);
    }

    return true;

}
WorkerPThread PThreadPool::FetchWork()
{
    int indexToReturn = -100;
    sem_wait(&workAvailable); // wait to get work
    pthread_mutex_lock(&mutexSync);

    //get the index of workerthread from queue
    for(std::vector<int>::size_type i = 0; i != workerQueue.size(); i++) {
        //if not assigned already
        if(workerQueue[i].assigned == false) {
            indexToReturn = i;
            workerQueue[i].assigned = true;
            break;
        }
    }
    std::cout<<std::endl<<"Getting work from queue at index : "<<indexToReturn<<std::endl;
    WorkerPThread ret = workerQueue[indexToReturn];
    pthread_mutex_unlock(&mutexSync);

    return ret;
}

void PThreadPool::removeActiveId(int id) {

    std::cout<<std::endl<<"ActiveThreadids size : "<<activeThreadIds.size()<<". Removing id "<<id<<std::endl;
    pthread_mutex_lock(&activeThreadMutex);
    int index = -1;
    std::cout<<std::endl<<"Looping"<<std::endl;
    for(std::vector<int>::size_type i = 0; i != activeThreadIds.size(); i++) {
        std::cout<<"Id is : "<<activeThreadIds[i]<<std::endl;
        if(activeThreadIds[i] == id) {
            index = i;
            break;
        }
    }

    if(index>=0) {
        activeThreadIds.erase(activeThreadIds.begin() + index);
        std::cout<<std::endl<<"Removed id "<<id<<" from activeThreadIds. Now vector size is "<<activeThreadIds.size()<<std::endl;
    }
    else {
        std::cout<<std::endl<<"Error occured, couldnt find id : "<<id<<std::endl;
    }
    pthread_mutex_unlock(&activeThreadMutex);

    //Delete from vorker queue
    pthread_mutex_lock(&mutexSync);
    bool foundToDelete = false;
    int delIndex = -1;
    for(std::vector<int>::size_type i = 0; i != workerQueue.size(); i++) {
        if(workerQueue[i].id == id) {
            delIndex = i;
            foundToDelete = true;
            break;
        }
    }

    workerQueue.erase(workerQueue.begin() + delIndex);
    std::cout<<std::endl<<"Removed index "<<delIndex<<" from workerQueue. Now vector size is "<<workerQueue.size()<<std::endl;
    pthread_mutex_unlock(&mutexSync);
    if(foundToDelete == false) {
        std::cout<<std::endl<<"Error occured, couldnt find id in queue to mark executed. Id is : "<<id<<std::endl;
    }

}



void *PThreadPool::ThreadExecute(void *param)
{
    //get the parameters passed in
    PThreadPool *pThreadPool = (PThreadPool *)param;
    int s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (s != 0)
        std::cout<<std::endl<<"Error setting cancel state for thread"<<std::endl;

    while(!pThreadPool->destroy)
    {
        WorkerPThread workerThread = pThreadPool->FetchWork();
        workerThread.ptid = pthread_self();
        std::cout<<"Fetched workerthread (executing now) with ptid: "<<workerThread.ptid<<std::endl;
        workerThread.ThreadFunction(workerThread.threadArgs.otherArgs);
        std::cout<<std::endl<<"Execution done for: "<<& workerThread<<std::endl;

        pThreadPool->removeActiveId(workerThread.id);

        std::cout<<"Deleted worker thread"<<std::endl;
        pthread_mutex_lock( &(pThreadPool)->mutexWorkCompletion) ;
        pThreadPool->incompleteWork--;
        pthread_mutex_unlock( &(pThreadPool)->mutexWorkCompletion) ;

    }


    std::cout<<std::endl<<"Thread exited ptid:"<<pthread_self()<<std::endl;
    return 0;
}



}
