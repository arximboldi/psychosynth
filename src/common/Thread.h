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

class Thread {
	pthread_t m_thread;
	Runnable* m_obj;

	static void* process(void* obj) {
		reinterpret_cast<Runnable*>(obj)->run();
		return NULL;
	};
	
public:	
	Thread(Runnable* obj = NULL)
		: m_obj(obj) {};
	
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

class SelfThread : public Runnable, public Thread {
public:
	virtual ~SelfThread() {};
	SelfThread() : Thread(this) {};
};

#endif /* THREAD_H */
