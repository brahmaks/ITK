

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
#pragma once
#ifndef __itkThreadPool_h
#define __itkThreadPool_h


#include <windows.h>
#include <iostream>
#include <vector>
#include "itkWinJob.h"


namespace itk
{

class WinThreadPool
{
public:

  

  static WinThreadPool & GetPoolInstance();

  static WinThreadPool & GetPoolInstance(int maxThreads);

  static void DeleteInstance();

  void DestroyPool(int maxPollSecs);

  int AssignWork(WinJob worker);

  void InitializeThreads(int maxThreads);

  bool WaitForThread(int id);

protected:

  WinThreadPool();
  ~WinThreadPool();
private:
  bool Destroy;
  static WinThreadPool * WinThreadPoolInstance;
  static bool InstanceFlag;
  WinThreadPool(WinThreadPool const &);              // copy constructor is private
  WinThreadPool & operator=(WinThreadPool const &);  // assignment operator is
                                                 // private

  void RemoveActiveId(int id);

  int                     MaxThreads;
  HANDLE                  WorkAvailable; //semaphore
  std::vector<WinJob> WorkerQueue;
  std::vector<int>        ActiveThreadIds;
  int                     IncompleteWork;
  int                     QueueSize;
  int                     IdCounter;
  bool                    ExceptionOccured;
  HANDLE*				  ThreadHandles;
  static HANDLE  MutexSync; //mutex
  static HANDLE  ActiveThreadMutex; //mutex
  static HANDLE  MutexWorkCompletion; //mutex

  WinJob FetchWork();

  static void * ThreadExecute(void *param);

};

}
#endif

