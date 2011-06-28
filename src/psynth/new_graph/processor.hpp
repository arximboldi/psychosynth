/**
 *  Time-stamp:  <2011-06-28 18:48:08 raskolnikov>
 *
 *  @file        processor.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 21:30:51 2011
 *
 *  @brief 
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_GRAPH_PROCESSOR_HPP_
#define PSYNTH_GRAPH_PROCESSOR_HPP_

#include <thread>
#include <atomic>
#include <list>

#include <psynth/new_graph/core/patch_fwd.hpp>
#include <psynth/new_graph/node_fwd.hpp>
#include <psynth/new_graph/port_fwd.hpp>
#include <psynth/new_graph/sink_node_fwd.hpp>
#include <psynth/new_graph/process_node_fwd.hpp>

#include <psynth/new_graph/exception.hpp>
#include <psynth/new_graph/event.hpp>
#include <psynth/new_graph/triple_buffer.hpp>

#include <psynth/base/hetero_deque.hpp>
#include <psynth/base/threads.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_ERROR (error, processor_error);
PSYNTH_DECLARE_ERROR (processor_error, processor_not_running_error);
PSYNTH_DECLARE_ERROR (processor_error, processor_not_idle_error);

constexpr std::size_t default_queue_size = 1 << 20;
constexpr std::size_t default_block_size = 1 << 6;
constexpr std::size_t default_frame_rate = 44100;

class processor;

typedef base::hetero_deque<rt_event>    rt_event_deque;
typedef base::hetero_deque<async_event> async_event_deque;

typedef triple_buffer<
    rt_event_deque,
    base::try_lock_guard<>,
    base::no_lock_guard<> >
rt_triple_buffer;

typedef triple_buffer<
    async_event_deque,
    base::no_lock_guard<>, // We handle locking ourselves in the
                           // processor class because it is not so simple.
    base::lock_guard<> >
async_triple_buffer;

class basic_process_context : private boost::noncopyable
{
public:    
    std::size_t block_size () const
    { return _block_size; }

    std::size_t frame_rate () const
    { return _frame_rate; }

protected:
    /** Only processor can create instances. */
    basic_process_context (std::size_t block_size,
                           std::size_t frame_rate,
                           std::size_t queue_size);
    
    rt_triple_buffer    _rt_buffers;
    async_triple_buffer _async_buffers;
    
    std::size_t _block_size;
    std::size_t _frame_rate;
    
    // FIXME: Factor the related functions a bit
    std::thread             _async_thread;
    std::condition_variable _async_cond;
    std::mutex              _async_mutex;
    bool                    _async_request_flip;

    friend class processor;
};

class rt_process_context : public virtual basic_process_context
{
public:
    template <class Event, typename... Args>
    bool push_rt_event (Args&&... args);

    template <class Event, typename... Args>
    bool push_async_event (Args&&... args);

    template <class Concrete>
    bool push_rt_event (Concrete&& arg)
    {
        return this->push_rt_event<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }

    template <class Concrete>
    bool push_async_event (Concrete&& arg)
    {
        return this->push_async_event<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }

protected:
    friend class processor;
    
    rt_process_context (std::size_t block_size,
                           std::size_t frame_rate,
                           std::size_t queue_size)
        : basic_process_context (block_size, frame_rate, queue_size)
    {}
    
    node_ptr        _curr_node;
    node_ptr        _request_node;
    out_port_base*  _request_output;
};

class async_process_context : public virtual basic_process_context
{
public:    
    template <class Event, typename... Args>
    bool push_rt_event (Args&&... args);

    template <class Event, typename... Args>
    bool push_async_event (Args&&... args);

    template <class Concrete>
    bool push_rt_event (Concrete&& arg)
    {
        return this->push_rt_event<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }

    template <class Concrete>
    bool push_async_event (Concrete&& arg)
    {
        return this->push_async_event<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }

protected:
    friend class processor;

    async_process_context (std::size_t block_size,
                           std::size_t frame_rate,
                           std::size_t queue_size)
        : basic_process_context (block_size, frame_rate, queue_size) {}
    
};

class user_process_context : public virtual basic_process_context
{
public:
    template <class Event, typename... Args>
    bool push_rt_event (Args&&... args);

    template <class Event, typename... Args>
    bool push_async_event (Args&&... args);

    template <class Concrete>
    bool push_rt_event (Concrete&& arg)
    {
        return this->push_rt_event<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }

    template <class Concrete>
    bool push_async_event (Concrete&& arg)
    {
        return this->push_async_event<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }

protected:
    friend class processor;

    user_process_context (std::size_t block_size,
                             std::size_t frame_rate,
                             std::size_t queue_size)
        : basic_process_context (block_size, frame_rate, queue_size) {}
};

class full_process_context : public rt_process_context
                           , public async_process_context
                           , public user_process_context
{
// private:
public: // Eases unit testing!
    friend class processor;
    
    full_process_context (std::size_t block_size = 1 << 6,
                          std::size_t frame_rate = 44100,
                          std::size_t queue_size = 1 << 10)
        : basic_process_context (block_size, frame_rate, queue_size)
        , rt_process_context (block_size, frame_rate, queue_size)
        , async_process_context (block_size, frame_rate, queue_size)
        , user_process_context (block_size, frame_rate, queue_size)
    {}
};


class processor : private boost::noncopyable
{
public:
    processor (core::patch_ptr root   = 0,
               std::size_t block_size = default_block_size,
               std::size_t frame_rate = default_frame_rate,
               std::size_t queue_size = default_queue_size);

    ~processor ();
    
    void start ();
    void stop ();

    core::patch_ptr root ()
    { return _root; }

    user_process_context& context ()
    { return _ctx; }
    
    const user_process_context& context () const
    { return _ctx; }
    
    void set_block_size (std::size_t new_size);
    void set_frame_rate (std::size_t new_frame_rate);
    
    void rt_request_process (std::ptrdiff_t iterations);
    void rt_request_process ();

    bool is_running () const
    { return _is_running; }

    /** To be called by patches */
    void notify_add_node (node_ptr node)
    { _explore_node_add (node); }

    /** To be called by patches */
    void notify_remove_node (node_ptr node)
    { _explore_node_remove (node); }
    
private:
    void _explore_node_add (node_ptr node);
    void _explore_node_remove (node_ptr node);
    
    void _async_loop ();
    void _rt_process_once ();

    typedef std::list<sink_node_ptr> sink_node_list;
    typedef std::list<process_node_ptr> process_node_list;

    core::patch_ptr         _root;
    
    process_node_list       _procs; // Not readed from rt-threads.
    sink_node_list          _sinks; // Readed from rt-threads.
    
    full_process_context    _ctx;

    std::mutex              _rt_mutex;
    
    std::atomic<bool>       _is_running;
};

} /* namespace graph */
} /* namespace psynth */

#include <psynth/new_graph/processor.tpp>

#endif /* PSYNTH_GRAPH_PROCESSOR_HPP_ */
