/**
 *  Time-stamp:  <2010-10-19 02:03:24 raskolnikov>
 *
 *  @file        task_manager.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Task manager interface.
 */

/*
 *  Copyright (C) 2007, 2009 Juan Pedro Bolívar Puente
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>
#include <psynth/base/singleton.hpp>

class task_manager;

class task
{
    friend class task_manager;

    bool m_is_finished;
    bool m_is_paused;

    task* m_next;
    task* m_prev;

    task_manager* m_mgr;

public:
    task() :
	m_is_finished(false),
	m_is_paused(false),
	m_next(NULL),
	m_prev(NULL),
	m_mgr (NULL)
	{};

    virtual ~task () {}

    inline void finish ();

    bool is_finished () {
	return m_is_finished;
    };

    bool toggle_pause () {
	return (m_is_paused = !m_is_paused);
    };

    bool is_paused () {
	return m_is_paused;
    };

    task* set_next (task* next) {
	m_next = next;
	return next;
    };

    task* get_next () {
	return m_next;
    };

    task_manager* get_manager () {
	return m_mgr;
    }

    virtual void update (int ms) = 0;
};

/**
 * @todo separate from singleton_holder
 */
class task_manager
{
    typedef std::list<task*> task_list;
    typedef std::list<task*>::iterator task_iter;

    task_list m_tasks;

    void delete_tasks () {
	for (task_iter i = m_tasks.begin(); i != m_tasks.end(); i++)
	    delete *i;
    }

public:
    ~task_manager ()
    {
	delete_tasks();
    };

    void attach (task* task) {
	m_tasks.push_back(task);
	task->m_mgr = this;
    };

    void detach (task* task) {
	m_tasks.remove(task);
	task->m_mgr = NULL;
    };

    void update (int ms);

    bool has_tasks () {
	return !m_tasks.empty();
    }
};

typedef psynth::base::singleton_holder<task_manager> global_task_manager;

void task::finish ()
{
    m_is_finished = true;
    if (m_next)
	m_mgr->attach(m_next);
}

#endif
