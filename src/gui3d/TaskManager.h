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

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>

class TaskManager;

class Task {
	friend class TaskManager;
	
	bool m_is_finished;
	bool m_is_paused;
	
	Task* m_next;
	Task* m_prev;
	
	TaskManager* m_mgr;
	
public:
	Task() :
		m_is_finished(false),
		m_is_paused(false),
		m_next(NULL),
		m_prev(NULL),
		m_mgr(NULL)
	{};
	
	virtual ~Task() {}
	
	inline void finish();
	
	bool isFinished() {
		return m_is_finished;
	};
	
	bool togglePause() {
		return (m_is_paused = !m_is_paused);
	};
	
	bool isPaused() {
		return m_is_paused;
	};
	
	Task* setNext(Task* next) {
		m_next = next;
		return next;
	};
	
	Task* getNext() {
		return m_next;
	};
	
	TaskManager* getManager() {
		return m_mgr;
	}
	
	virtual void update(int ms) = 0;
};

class TaskManager {
	typedef std::list<Task*> TaskList;
	typedef std::list<Task*>::iterator TaskIter;
	
	TaskList m_tasks;
	
	void deleteTasks() {
		for (TaskIter i = m_tasks.begin(); i != m_tasks.end(); i++)
			delete *i;
	}
	
public:
	TaskManager()
		{}
	
	~TaskManager() {
		deleteTasks();
	};
	
	void attach(Task* task) {
		m_tasks.push_back(task);
		task->m_mgr = this;
	};
	
	void detach(Task* task) {
		m_tasks.remove(task);
		task->m_mgr = NULL;
	};
	
	void update(int ms) {
		TaskIter i = m_tasks.begin();
		while (i != m_tasks.end())
			if ((*i)->m_is_finished) {
				delete *i;
				i = m_tasks.erase(i);
			} else {
				if (!(*i)->m_is_paused)
					(*i)->update(ms);
				i++;
			}
	}
	
	bool hasTasks() {
		return !m_tasks.empty();
	}
};

void Task::finish()
{
	m_is_finished = true;
	if (m_next)
		m_mgr->attach(m_next);
}

#endif
