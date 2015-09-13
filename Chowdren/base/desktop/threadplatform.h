#include "SDL_thread.h"
#include "SDL_mutex.h"

class Thread
{
public:
    SDL_Thread * thread;

    Thread()
    : thread(NULL)
    {
    }

    void start(ThreadFunction f, void * data,
               const char * name = "ChowdrenThread")
    {
        thread = SDL_CreateThread(f, name, data);
    }

    bool is_running()
    {
        return thread != NULL;
    }

    void detach()
    {
        SDL_DetachThread(thread);
        thread = NULL;
    }

    int wait()
    {
        int status;
        SDL_WaitThread(thread, &status);
        thread = NULL;
        return status;
    }
};

class Mutex
{
public:
    SDL_mutex * mutex;

    Mutex()
    {
        mutex = SDL_CreateMutex();
    }

    ~Mutex()
    {
        SDL_DestroyMutex(mutex);
    }

    int lock()
    {
        return SDL_LockMutex(mutex);
    }

    int unlock()
    {
        return SDL_UnlockMutex(mutex);
    }
};
