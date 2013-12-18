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

#ifndef __itkPthreadPool_h
#define __itkPthreadPool_h

#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <vector>
#include "itkPThreadJob.h"
#include "itkObject.h"
#include "itkObjectFactory.h"

namespace itk
{

class ITKCommon_EXPORT PThreadPool : public Object
{
public:

  /** Standard class typedefs. */
  typedef PThreadPool              Self;
  typedef Object                   Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PThreadPool, Object);

  static PThreadPool & GetPoolInstance();

  static PThreadPool & GetPoolInstance(int maxThreads);

  static void DeleteInstance();

  void DestroyPool(int maxPollSecs);

  int AssignWork(PThreadJob worker);

  void InitializeThreads(int maxThreads);

  bool WaitForThread(int id);

protected:

  PThreadPool();
  ~PThreadPool();
private:
  bool Destroy;
  static PThreadPool * PThreadPoolInstance;
  static bool InstanceFlag;
  PThreadPool(PThreadPool const &);              // copy constructor is private
  PThreadPool & operator=(PThreadPool const &);  // assignment operator is
                                                 // private

  void RemoveActiveId(int id);

  int                     MaxThreads;
  sem_t                   WorkAvailable;
  std::vector<PThreadJob> WorkerQueue;
  std::vector<int>        ActiveThreadIds;
  int                     IncompleteWork;
  int                     QueueSize;
  int                     IdCounter;
  bool                    ExceptionOccured;
  static pthread_mutex_t  MutexSync;
  static pthread_mutex_t  ActiveThreadMutex;
  static pthread_mutex_t  MutexWorkCompletion;
  pthread_t *             ThreadHandles;
  PThreadJob FetchWork();

  static void * ThreadExecute(void *param);

};

}
#endif
