/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include <stdlib.h>
#include "itkPThreadpool.h"

namespace itk
{

pthread_mutex_t PThreadPool:: MutexSync = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PThreadPool:: MutexWorkCompletion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t PThreadPool:: ActiveThreadMutex = PTHREAD_MUTEX_INITIALIZER;

PThreadPool * PThreadPool:: PThreadPoolInstance = 0;
bool PThreadPool::          InstanceFlag = false;

PThreadPool & PThreadPool::GetPoolInstance()
{
  if( !InstanceFlag )
    {
    PThreadPoolInstance = new PThreadPool();
    PThreadPoolInstance->InitializeThreads(2);
    }
  return *PThreadPoolInstance;
}

PThreadPool & PThreadPool::GetPoolInstance(int maxThreads)
{
  if( !InstanceFlag )
    {
    PThreadPoolInstance = new PThreadPool();
    PThreadPoolInstance->InitializeThreads(maxThreads);
    }
  return *PThreadPoolInstance;
}

PThreadPool::PThreadPool() : IdCounter(1)
{
  InstanceFlag = true;
}

void PThreadPool::InitializeThreads(int maximumThreads)
{

  try
    {
    if( maximumThreads < 1 )
      {
      maximumThreads = 1;
      }
    this->MaxThreads = maximumThreads;
    this->QueueSize = maximumThreads;
    Destroy = false;
    ThreadHandles = new pthread_t[MaxThreads];
    IncompleteWork = 0;
    sem_init(&WorkAvailable, 0, 0);
    Destroy = false;
    ThreadHandles = new pthread_t[MaxThreads];
    for( int i = 0; i < MaxThreads; i++ )
      {
      int rc;
      rc = pthread_create(&ThreadHandles[i], NULL, &PThreadPool::ThreadExecute, (void *)this );
      if( rc )
        {
        std::cout << "ERROR; return code from pthread_create() is " << rc << std::endl;
        throw "Cannot create thread. Error in return code from pthread_create()";
        }
      std::cout << "Thread createdd with ptid :" << ThreadHandles[i] << std::endl;
      }
    }
  catch( std::exception& e )
    {
    ExceptionOccured = true;
    std::cout << std::endl << "Initialization failure\n" << e.what() << std::endl;
    }
  std::cout << "Created thread pool with threads :" << maximumThreads << std::endl;

}

// Can call if needed to delete the created singleton instance
void PThreadPool::DeleteInstance()
{
  try
    {
    if( InstanceFlag == true )
      {
      std::cout << "Deleting instance" << std::endl;
      InstanceFlag = false;
      PThreadPoolInstance->DestroyPool(2);
      PThreadPoolInstance->WorkerQueue.clear();

      delete[] PThreadPoolInstance->ThreadHandles;

      delete PThreadPoolInstance;

      std::cout << "Destroyed" << std::endl;

      }
    }
  catch( std::exception& e )
    {
    std::cout << std::endl << "Cannot delete instance \n" << e.what() << std::endl;
    PThreadPoolInstance->ExceptionOccured = true;
    }
}

PThreadPool::~PThreadPool()
{

}

/*
  This method first checks if all threads are finished executing their jobs.
  If now, it will wait for "poolTime" seconds and then re-check.
  Once all are done it will Destroy the thread pool.
*/
void PThreadPool::DestroyPool(int pollTime = 2)
{
  Destroy = true;
  while( IncompleteWork > 0 )
    {
    std::cout << "Work is still incomplete=" << IncompleteWork << std::endl;
    sleep(pollTime);
    }

  std::cout << "All threads are done" << std::endl << "Terminating threads" << std::endl;
  for( int i = 0; i < MaxThreads; i++ )
    {
    int s = pthread_cancel(ThreadHandles[i]);
    if( s != 0 )
      {
      std::cout << "Cannot cancel thread with id : " << ThreadHandles[i] << std::endl;
      }
    }

  sem_destroy(&WorkAvailable);

  pthread_mutex_destroy(&MutexSync);
  pthread_mutex_destroy(&MutexWorkCompletion);
}

int PThreadPool::AssignWork(PThreadJob pthreadJob)
{
  try
    {
    pthread_mutex_lock(&MutexWorkCompletion);
    IncompleteWork++;
    pthread_mutex_unlock(&MutexWorkCompletion);

    pthread_mutex_lock(&MutexSync);
    // adding to queue
    pthreadJob.Id = IdCounter++;
    WorkerQueue.push_back(pthreadJob);
    std::cout << "Assigning Worker[" << (pthreadJob).Id << "] Address:[" << &pthreadJob << "] to Queue " << std::endl;
    pthread_mutex_unlock(&MutexSync);

    pthread_mutex_lock(&ActiveThreadMutex);
    std::cout << std::endl << "Adding id " << pthreadJob.Id << " to ActiveThreadIds" << std::endl;
    ActiveThreadIds.push_back(pthreadJob.Id);
    std::cout << std::endl << "ActiveThreadids size : " << ActiveThreadIds.size() << std::endl;
    pthread_mutex_unlock(&ActiveThreadMutex);

    sem_post(&WorkAvailable);

    return (pthreadJob).Id;
    }
  catch( std::exception& e )
    {
    ExceptionOccured = true;
    std::cout << std::endl << "Failed to assign work. \n" << e.what() << std::endl;
    throw e;
    }
}

bool PThreadPool::WaitForThread(int id)
{

  bool found = true;

  try
    {
    while( found )
      {
      pthread_mutex_lock(&ActiveThreadMutex);
      /*
       We know that a thread id will be in here because the AssignWork fn adds it in
       So if a thread id is not found in ActiveThreadIds it means it is done.
      */
      found = false;
      for( std::vector<int>::size_type i = 0; i != ActiveThreadIds.size(); i++ )
        {

        if( ActiveThreadIds[i] == id )
          {
          found = true;
          break;
          }
        }
      pthread_mutex_unlock(&ActiveThreadMutex);
      }

    return true;
    }
  catch( std::exception& e )
    {
    ExceptionOccured = true;
    std::cout << "Exception occured while waiting for job with id : " << id << std::endl << e.what() << std::endl;

    }
}

PThreadJob PThreadPool::FetchWork()
{

  int indexToReturn = -100;

  sem_wait(&WorkAvailable);   // wait to get work
  pthread_mutex_lock(&MutexSync);
  // get the index of workerthread from queue
  for( std::vector<int>::size_type i = 0; i != WorkerQueue.size(); i++ )
    {
    // if not Assigned already
    if( WorkerQueue[i].Assigned == false )
      {
      indexToReturn = i;
      WorkerQueue[i].Assigned = true;
      break;
      }
    }
  std::cout << std::endl << "Getting work from queue at index : " << indexToReturn << std::endl;
  PThreadJob ret = WorkerQueue[indexToReturn];
  pthread_mutex_unlock(&MutexSync);

  return ret;
}

void PThreadPool::RemoveActiveId(int id)
{
  try
    {
    std::cout << std::endl << "ActiveThreadids size : " << ActiveThreadIds.size() << ". Removing id " << id
              << std::endl;
    pthread_mutex_lock(&ActiveThreadMutex);
    int index = -1;
    std::cout << std::endl << "Looping" << std::endl;
    for( std::vector<int>::size_type i = 0; i != ActiveThreadIds.size(); i++ )
      {
      std::cout << "Id is : " << ActiveThreadIds[i] << std::endl;
      if( ActiveThreadIds[i] == id )
        {
        index = i;
        break;
        }
      }

    if( index >= 0 )
      {
      ActiveThreadIds.erase(ActiveThreadIds.begin() + index);
      std::cout << std::endl << "Removed id " << id << " from ActiveThreadIds. Now vector size is "
                << ActiveThreadIds.size() << std::endl;
      }
    else
      {

      std::cout << std::endl << "Error occured, couldnt find id : " << id << std::endl;
      throw "Error occured in RemoveActiveId, couldnt find id in ActiveThreadIds queue to erase.";
      }
    pthread_mutex_unlock(&ActiveThreadMutex);

    // Delete from vorker queue
    pthread_mutex_lock(&MutexSync);
    bool foundToDelete = false;
    int  delIndex = -1;
    for( std::vector<int>::size_type i = 0; i != WorkerQueue.size(); i++ )
      {
      if( WorkerQueue[i].Id == id )
        {
        delIndex = i;
        foundToDelete = true;
        break;
        }
      }

    WorkerQueue.erase(WorkerQueue.begin() + delIndex);
    std::cout << std::endl << "Removed index " << delIndex << " from WorkerQueue. Now vector size is "
              << WorkerQueue.size() << std::endl;
    pthread_mutex_unlock(&MutexSync);
    if( foundToDelete == false )
      {
      std::cout << std::endl << "Error occured, couldnt find id in WorkerQueue to mark executed. Id is : " << id
                << std::endl;
      throw "Error occured in RemoveActiveId, couldnt find id in WorkerQueue to mark executed. ";
      }
    }
  catch( std::exception & e )
    {
    throw e;
    }

}

void * PThreadPool::ThreadExecute(void *param)
{
  // get the parameters passed in
  PThreadPool *pThreadPool = (PThreadPool *)param;
  int          s = pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  if( s != 0 )
    {
    std::cout << std::endl << "Error setting cancel state for thread" << std::endl;
    }

  while( !pThreadPool->Destroy )
    {
    try
      {
      PThreadJob pthreadJob = pThreadPool->FetchWork();
      pthreadJob.Ptid = pthread_self();
      std::cout << "Fetched workerthread (executing now) with ptid: " << pthreadJob.Ptid << std::endl;
      pthreadJob.ThreadFunction(pthreadJob.ThreadArgs.otherArgs);
      std::cout << std::endl << "Execution done for: " << &pthreadJob << std::endl;

      pThreadPool->RemoveActiveId(pthreadJob.Id);

      std::cout << "Deleted worker thread" << std::endl;
      pthread_mutex_lock( &(pThreadPool)->MutexWorkCompletion);
      pThreadPool->IncompleteWork--;
      pthread_mutex_unlock( &(pThreadPool)->MutexWorkCompletion);
      }
    catch( std::exception& e )
      {
      pThreadPool->ExceptionOccured = true;
      std::cout << "Exception occured in thread execution\n" << e.what() << std::endl;
      }

    }

  std::cout << std::endl << "Thread exited ptid:" << pthread_self() << std::endl;
  return 0;
}

}
