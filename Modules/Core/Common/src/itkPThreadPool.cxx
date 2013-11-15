#include <stdlib.h>
#include "itkPThreadpool.h"

//using namespace std;


namespace itk {

pthread_mutex_t PThreadPool::mutexSync = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PThreadPool::mutexWorkCompletion = PTHREAD_MUTEX_INITIALIZER;


PThreadPool * PThreadPool::New()
{
    return new PThreadPool(2);
}

PThreadPool * PThreadPool::New(int maxThreads)
{
    return new PThreadPool(maxThreads);
}

PThreadPool::PThreadPool()
{
    PThreadPool(2); //will create a thread pool with at least 2 threads
}

PThreadPool::PThreadPool(int maximumThreads)
{

    //need at least 1 thread
    if (maximumThreads < 1)  
	maximumThreads=1;
    //find PTHREAD_THREADS_MAX and change this condition accordingly
    if(maximumThreads > 25)
	maximumThreads = 25;

    std::cout<<"Created thread pool with threads :"<<maximumThreads<<std::endl;
    pthread_mutex_lock(&mutexSync);
    this->maxThreads = maximumThreads;
    this->queueSize = maximumThreads;

    workerQueue.resize(maximumThreads, NULL);
    for(std::vector<int>::size_type i = 0; i != workerQueue.size(); i++) {
        workerQueue[i] = NULL;
    }
    topIndex = 0;
    bottomIndex = 0;
    incompleteWork = 0;
    sem_init(&availableWork, 0, 0);
    sem_init(&availableThreads, 0, queueSize);
    pthread_mutex_unlock(&mutexSync);
}

void PThreadPool::InitializeThreads()
{
   
   threadHandles = new pthread_t[maxThreads];
    for(int i = 0; i<maxThreads; i++)
    {
	int rc;
        //pthread_t tempThread;
        rc = pthread_create(&threadHandles[i], NULL, &PThreadPool::ThreadExecute, (void *)this );
        if (rc){
            std::cout<<"ERROR; return code from pthread_create() is "<<rc<<std::endl;
            exit(-1);
        }
        std::cout<<"Thread createdd with ptid :"<<threadHandles[i]<<std::endl;
    }
 
}

PThreadPool::~PThreadPool()
{
    DestroyPool(2);
    workerQueue.clear();
   
    delete[] threadHandles;   
}


/*
	This method first checks if all threads are finished executing their jobs.
	If now, it will wait for "poolTime" seconds and then re-check.
	Once all are done it will destroy the thread pool.
*/
void PThreadPool::DestroyPool(int pollTime = 2)
{
    while( incompleteWork>0 )
    {
        std::cout << "Work is still incomplete=" << incompleteWork << std::endl;
        sleep(pollTime);
    }

    std::cout << "All threads are done" << std::endl;
    sem_destroy(&availableWork);
    sem_destroy(&availableThreads);
    pthread_mutex_destroy(&mutexSync);
    pthread_mutex_destroy(&mutexWorkCompletion);
  
}

WorkerPThread* PThreadPool::getWorker(pthread_t* id){

    std::cout<<std::endl<<"Getting worker with ptid "<<*id<<std::endl;
    pthread_mutex_lock(&mutexSync);
    /*for (std::vector<WorkerPThread *>::iterator itr = workerQueue.begin();
                           itr != workerQueue.end();
                           ++itr)
    {
        try{
        WorkerPThread *w = *itr;
	    if(w==NULL){
		//std::cout<<"Null worker thread in queue";
		continue;
	    }
	    
	    if(w->id == id){
	        pthread_mutex_unlock(&mutexSync);
		return w;
	    }
	}
	catch( ... ){
	    std::cout<<"Error looping workerqueue"<<std::endl;
	}
    }*/
    for(std::vector<int>::size_type i = 0; i != workerQueue.size(); i++) {
        try{
        
	    if(workerQueue[i]==NULL){
		//std::cout<<"Null worker thread in queue";
		continue;
	    }
	    
	    //if(workerQueue[i]->ptid == id){
	    if(pthread_equal(workerQueue[i]->ptid, *id)){
	        pthread_mutex_unlock(&mutexSync);
		std::cout<<std::endl<<"Found worker"<<std::endl;
		return workerQueue[i];
	    }
	}
	catch( ... ){
	    std::cout<<"Error looping workerqueue"<<std::endl;
	}
    }
    std::cout<<std::endl<<"Couldnt find worker with id "<<*id;
    pthread_mutex_unlock(&mutexSync);
    return 0;
}

bool PThreadPool::AssignWork(WorkerPThread *workerThread, bool wait)
{
    waitFlag = wait;
    pthread_mutex_lock(&mutexWorkCompletion);
    incompleteWork++;
    pthread_mutex_unlock(&mutexWorkCompletion);

    sem_wait(&availableThreads);

    pthread_mutex_lock(&mutexSync);

    workerQueue[topIndex] = workerThread;
    std::cout << "Assigning Worker[" << workerThread->id << "] Address:[" << workerThread << "] to Queue index [" << topIndex << "]" << std::endl;
    if(queueSize !=1 )
        topIndex = (topIndex+1) % (queueSize-1);
    sem_post(&availableWork);
    pthread_mutex_unlock(&mutexSync);
    return true;
}

bool PThreadPool::WaitForThread(WorkerPThread **workerThread)
{

    while( incompleteWork>0 )
    {
        std::cout << "Work is still incomplete=" << incompleteWork << std::endl;
        sleep(2);
    }
/*
    if((*workerThread)==NULL){
        std::cout<<std::endl<<"Null workerthread";
        return false;
    }

    std::cout<<std::endl<<"Waiting for myThread. id is "<<(**workerThread).id;
    while((**workerThread).ActiveFlag!=0){	
        
	//sleep(5);
    }
    std::cout<<std::endl<<"DONE Waiting for myThread. id is "<<(**workerThread).id;
    delete (*workerThread);
    (*workerThread) = NULL;
*/
    return true;

}
bool PThreadPool::FetchWork(WorkerPThread **workerArg)
{
    sem_wait(&availableWork); // wait to get work
    pthread_mutex_lock(&mutexSync);
    WorkerPThread * workerThread = workerQueue[bottomIndex];
    workerQueue[bottomIndex] = NULL;
    *workerArg = workerThread;
    if(queueSize !=1 )
        bottomIndex = (bottomIndex+1) % (queueSize-1);
    sem_post(&availableThreads);
    pthread_mutex_unlock(&mutexSync);
    return true;
}

void *PThreadPool::ThreadExecute(void *param)
{
    
    WorkerPThread *workerThread = NULL;
    //ThreadArgs *threadArgs = (ThreadArgs *)param;	
    PThreadPool *pThreadPool = (PThreadPool *)param;//threadArgs->pThreadPool;

    while(pThreadPool->FetchWork(&workerThread))
    {
        if(workerThread)
        {
            workerThread->ptid = pthread_self();
	    std::cout<<"Fetched workerthread (executing now):"<<workerThread<<std::endl;
            workerThread->ThreadFunction(workerThread->threadArgs.otherArgs);
	    std::cout<<"Execution done:"<<workerThread<<std::endl;
	    workerThread->ActiveFlag = 0;
	    
	    if(!pThreadPool->waitFlag){
                delete workerThread;
	        workerThread = NULL;
	        std::cout<<"Deleted worker thread"<<std::endl;
            }
        }

        pthread_mutex_lock( &(pThreadPool)->mutexWorkCompletion) ;
        //can notify the user here - job is complete
        pThreadPool->incompleteWork--;
        pthread_mutex_unlock( &(pThreadPool)->mutexWorkCompletion) ;
    }
    return 0;
}



}
