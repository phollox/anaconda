// Copyright (c) Mathias Kaerlev 2012-2015.
//
// This file is part of Anaconda.
//
// Anaconda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Anaconda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Anaconda.  If not, see <http://www.gnu.org/licenses/>.

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
