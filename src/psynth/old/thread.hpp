/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef PSYNTH_THREAD_H
#define PSYNTH_THREAD_H

#include <pthread.h>

namespace psynth
{

#include <libpsynth/common/FastDelegate.h>
using namespace fastdelegate;

/**
 * Class with a @c run function..
 *
 * If you inherit from this class and attach it to a Thread the @c run()
 * function will be run in a new thread. You can use it anyways for any
 * other purpose where a virtual run function makes sense.
 */
class runnable
{
public:
    virtual ~runnable() {};

    /**
     * Function to be run in a thread.
     */
    virtual void run() = 0;
};

/**
 * A delegate callable in a thread.
 */
typedef FastDelegate0<void> thread_delegate;

/**
 * A Thread is an objects to execute pieces of code concurrently allowing
 * shared access to the process memory.
 *
 * You can attach to a thread a Runnable object or a ThreadDelegate. When you
 * run @c start() the Runnable @c run() function or the function to which the
 * ThreadDelegate points will be called in a new thread, this means that the
 * normal flow of the code will contine but the function will run concurrently
 * too.
 *
 * Mutex, RWMutex and Condition classes provide some synchronization methods
 * to avoid race conditions in concurrent code.
 */
class thread
{
private:
    pthread_t m_thread;
    thread_delegate m_delegate;

    static void* process(void* obj) {
	thread* the_this = reinterpret_cast<thread*>(obj);
	the_this->m_delegate();

	return NULL;
    };

public:
    /**
     * Contructor that attaches a runnable object to the Thread.
     * @param obj The runnable object that will be attached to the thread.
     */
    thread (runnable& obj) {
	m_delegate = MakeDelegate(&obj, &runnable::run);
    }

    /**
     * Constructor that attaches a delegate to the Thread.
     * @param func The functor that will be attached to thread.
     */
    thread (thread_delegate func) {
	m_delegate = func;
    }

    /**
     * Sets a runnable object to attach to the Thread.
     * @param obj The Runnable to attach.
     */
    void set (runnable& obj) {
	m_delegate = MakeDelegate(&obj, &runnable::run);
    }

    /**
     * Sets a delegate to attach to the Thread.
     * @param func The delegate to attach.
     */
    void set (thread_delegate func) {
	m_delegate = func;
    }

    /**
     * Starts the concurrent execution of the attached entity.
     */
    void start () {
	pthread_create(&m_thread, NULL, &thread::process, this);
    }

    /**
     * Attaches a Runnable and starts its concurrent execution.
     * @param obj The runnable to attach.
     */
    void start (runnable& obj) {
	m_delegate = MakeDelegate(&obj, &runnable::run);
	pthread_create(&m_thread, NULL, &thread::process, this);
    }

    /**
     * Attaches a delegate and starts its concurrent execution.
     * @param func The delegate to attach.
     */
    void start (thread_delegate func) {
	m_delegate = func;
	pthread_create(&m_thread, NULL, &thread::process, this);
    }

    /**
     * Blocks untill the last started thread finished. Note that if you
     * call @c start() twice you will be only able to join the last called
     * function. If you want to be able to join both create a different Thread
     * instance for each.
     */
    void join () {
	pthread_join(m_thread, NULL);
    }

    /*
    pthread_t getThreadId() {
	return m_thread;
    };
    */
};

/**
 * A Thread that is also a Runnable which has itself automatically attached
 * to the thread, so every @c start() call will thread itself's @c run()
 * function. Inherit from it to build such automatically threadable objects.
 */
class self_thread : public runnable,
		    public thread
{
public:
    /** Virtual destructor. */
    virtual ~self_thread() {};

    /** Constructor. */
    self_thread () :
	thread (static_cast<runnable&>(*this))
	{};
};

} /* namespace psynth */

#endif /* PSYNTH_THREAD_H */
