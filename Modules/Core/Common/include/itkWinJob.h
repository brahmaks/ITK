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
#ifndef __itkPthreadJob_h
#define __itkPthreadJob_h


#include <iostream>

namespace itk
{

class WinJob
{
public:
  struct ThreadArgs
    {

    void *otherArgs;
    };
  int       Id;
  bool      Assigned;
  bool      Executed;
  int       Ptid;
  void *    UserData; // any data user wants the function to use

  void *     (*ThreadFunction)(void *ptr);
  
  ThreadArgs ThreadArgs;
  WinJob() : Assigned(false)
  {

    std::cout << "Starting thread \t address=" << this << std::endl;
  }

  ~WinJob()
  {
    std::cout << std::endl << "Thread finished. pid is  " << Id << "\t address=" << this << std::endl;
  }

};
}
#endif