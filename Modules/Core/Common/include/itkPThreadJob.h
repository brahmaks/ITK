#include <pthread.h>
#include <iostream>


class PThreadJob {
public:
    struct ThreadArgs {

        void *otherArgs;
    };
    int Id;
    bool Assigned;
    bool Executed;
    pthread_t Ptid;
    void *UserData; //any data user wants the function to use

    void* (*ThreadFunction)(void *ptr);
    pthread_t ThreadHandle;
    ThreadArgs ThreadArgs;
    PThreadJob() : Assigned(false) {

        std::cout << "Starting thread \t address=" << this << std::endl;
    }
    ~PThreadJob() {
        std::cout <<std::endl<< "Thread finished. pid is  " << Ptid << "\t address=" << this << std::endl;
    }


};

