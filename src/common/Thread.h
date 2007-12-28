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
 
#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Runnable {
public:
    virtual ~Runnable() {};
    virtual void run() = 0;
};

#define CALL_MEMBER_FN(object, ptr_to_member)  ((object).*(ptr_to_member))

template <class Type = Runnable>
class Thread
{
public:
    typedef void(Type::*FuncType)();

private:    
    pthread_t m_thread;
    Type* m_obj;
    FuncType m_func;

    static void* process(void* obj) {
	Thread<Type>* m_this = reinterpret_cast<Thread<Type>*>(obj);
	CALL_MEMBER_FN(m_this->m_obj, m_this->m_func);
	return NULL;
    };
    
public:	
    Thread() :
	m_obj(NULL),
	m_func(NULL)
	{
	};

    Thread(Type* obj) :
	m_obj(obj),
	m_func(NULL)
	{};

    Thread(Type* obj, FuncType func) :
	m_obj(obj),
	m_func(func)
	{};

    void start() {
	pthread_create(&m_thread, NULL, &Thread<Type>::process, this);
    };
    
    void start(FuncType func) {
	m_func = func;
	pthread_create(&m_thread, NULL, &Thread<Type>::process, this);
    };

    void start(Type* obj) {
	m_obj = obj;
	pthread_create(&m_thread, NULL, &Thread<Type>::process, this);
    };

    void start(FuncType func, Type* obj) {
	m_obj = obj;
	m_func = func;
	pthread_create(&m_thread, NULL, &Thread<Type>::process, this);
    };
	
    void join() {
	pthread_join(m_thread, NULL);
    };
	
    pthread_t getThreadId() {
	return m_thread;
    };
};

/*
 * TODO: Why is the code in Thread.h_gcc_bug not working? Also see
 *       wether we must not allow initialization of m_obj* after creation.
 */
template<>
class Thread<Runnable>
{
    pthread_t m_thread;
    Runnable* m_obj;

    static void* process(void* obj) {
	reinterpret_cast<Runnable*>(obj)->run();
	return NULL;
    };
	
public:	
    Thread(Runnable* obj = NULL)
	: m_obj(obj) {};
	
    void start(Runnable* obj) {
	m_obj = obj;
	pthread_create(&m_thread, NULL, &Thread::process, m_obj);
    };

    void start() {
	pthread_create(&m_thread, NULL, &Thread::process, m_obj);
    };
	
    void join() {
	pthread_join(m_thread, NULL);
    };
	
    pthread_t getThreadId() {
	return m_thread;
    };
};

class SelfThread : public Runnable, public Thread<Runnable> {
public:
    virtual ~SelfThread() {};
    SelfThread() : Thread<Runnable>(this) {};
};

#endif /* THREAD_H */
