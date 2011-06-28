/**
 *  Time-stamp:  <2011-06-28 18:54:58 raskolnikov>
 *
 *  @file        processor.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:17:02 2011
 *
 *  @brief Synthesis graph processor implementation.
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

#define PSYNTH_MODULE_NAME "psynth.graph.processor"

#include <iostream>

#include "base/throw.hpp"
#include "core/patch.hpp"
#include "sink_node.hpp"
#include "process_node.hpp"
#include "processor.hpp"

namespace psynth
{
namespace graph
{

PSYNTH_DEFINE_ERROR (processor_error);
PSYNTH_DEFINE_ERROR_WHAT (processor_not_running_error,
                          "Can not start running processor.");
PSYNTH_DEFINE_ERROR_WHAT (processor_not_idle_error,
                          "Can not stop idle processor.");

basic_process_context::basic_process_context (std::size_t block_size,
                                              std::size_t frame_rate,
                                              std::size_t queue_size)
    : _rt_buffers (rt_event_deque (queue_size),
                   rt_event_deque (queue_size),
                   rt_event_deque (queue_size))
    , _async_buffers (async_event_deque (queue_size),
                      async_event_deque (queue_size),
                      async_event_deque (queue_size))
    , _block_size (block_size)
    , _frame_rate (frame_rate)
    , _async_request_flip (false)
{
}

processor::processor (core::patch_ptr root,
                      std::size_t block_size,
                      std::size_t frame_rate,
                      std::size_t queue_size)
    : _root (root ? root : core::new_patch ())
    , _ctx (block_size, frame_rate, queue_size)
    , _is_running (false)
{
    _explore_node_add (_root);
}

processor::~processor ()
{
    if (_is_running)
        stop ();
}

void processor::start ()
{
    if (_is_running)
        throw processor_not_idle_error ();
    _is_running = true;
    _ctx._async_thread = std::thread (
        std::bind (&processor::_async_loop, this));
    for (auto& n : _procs)
        n->start ();
}

void processor::stop ()
{
    if (!_is_running)
        throw processor_not_running_error ();

    for (auto& n : _procs)
        n->stop ();
    
    {
        std::unique_lock<std::mutex> g (_ctx._async_mutex);
        _is_running = false;
        _ctx._async_cond.notify_all ();
    }
    
    if (_ctx._async_thread.joinable ())
        _ctx._async_thread.join ();
}

void processor::_async_loop ()
{
    while (_is_running || // HACK!!!!
           !_ctx._async_buffers.local ().empty ())
    {
        std::unique_lock<std::mutex> g (_ctx._async_mutex);

        for (auto& ev : _ctx._async_buffers.front ())
            ev (_ctx);
        _ctx._async_buffers.front ().clear ();
        _ctx._async_request_flip = true;
            
        _ctx._async_buffers.flip_local ();
        for (auto& ev : _ctx._async_buffers.front ())
            ev (_ctx);
        _ctx._async_buffers.front ().clear ();
            
        while (_ctx._async_request_flip &&
               _ctx._async_buffers.local ().empty () &&
               _is_running)
            _ctx._async_cond.wait (g);
    }
}

void processor::set_block_size (std::size_t new_size)
{
    assert (false);
}

void processor::set_frame_rate (std::size_t new_frame_rate)
{
    assert (false);
}

void processor::rt_request_process (std::ptrdiff_t iterations)
{
    while (iterations --> 0)
        rt_request_process ();
}

void processor::rt_request_process ()
{
    auto request_lock = base::make_unique_lock (
        _rt_mutex,
        std::try_to_lock);
    
    if (request_lock.owns_lock ())
        _rt_process_once ();
    else
        request_lock.lock ();
}

void processor::_rt_process_once ()
{
    _ctx._rt_buffers.flip_back ();
    for (auto& ev : _ctx._rt_buffers.front ())
        ev (_ctx);
    _ctx._rt_buffers.front ().clear ();
        
    for (auto& s : _sinks)
        s->rt_process (_ctx);
    _root->rt_advance ();
    
    _ctx._rt_buffers.flip_local ();
    for (auto& ev : _ctx._rt_buffers.front ())
        ev (_ctx);
    _ctx._rt_buffers.front ().clear ();
        
    std::unique_lock<std::mutex> g (_ctx._async_mutex, std::try_to_lock);
    if (g.owns_lock () && _ctx._async_request_flip &&
        !_ctx._async_buffers.back ().empty ())
    {
        _ctx._async_request_flip = false;
        _ctx._async_buffers.flip_back ();
        _ctx._async_cond.notify_all ();
    }
}

void processor::_explore_node_add (node_ptr n)
{
    // TODO: Maybe we shoudl, add patch visitor to avoid all this
    // dynamic_cast in case we find this pattern useful in other
    // parts.
    
    n->attach_to_process (*this);
    n->rt_context_update (_ctx);
    
    auto sink = std::dynamic_pointer_cast<sink_node> (n);
    if (sink)
    {
        if (!is_running ())
            _sinks.push_back (sink);
        else
            context ().push_rt_event (
                make_rt_event ([=] (rt_process_context&) {
                        this->_sinks.push_back (sink);
                    }));
    }
    
    auto proc = std::dynamic_pointer_cast<process_node> (n);
    if (proc)
    {
        _procs.push_back (proc);
        if (is_running ())
            proc->start ();
    }
    
    auto patch = std::dynamic_pointer_cast<core::patch> (n);
    if (patch)
    {
        for (auto& n : patch->childs ())
            _explore_node_add (n);
    }
}

void processor::_explore_node_remove (node_ptr n)
{
    n->check_attached_to_process ();
    if (&n->process () != this)
        PSYNTH_THROW (node_attachment_error)
            << "Removing node attached to other process?";
    n->detach_from_process ();
    
    auto sink = std::dynamic_pointer_cast<sink_node> (n);
    if (sink)
    {
        if (!is_running ())
            _sinks.remove (sink);
        else
            context ().push_rt_event (
                make_rt_event ([=] (rt_process_context&) {
                        this->_sinks.remove (sink);
                    }));
    }
    
    auto proc = std::dynamic_pointer_cast<process_node> (n);
    if (proc)
    {
        if (is_running ())
            proc->stop ();
        _procs.remove (proc);
    }
    
    auto patch = std::dynamic_pointer_cast<core::patch> (n);
    if (patch)
    {
        for (auto& n : patch->childs ())
            _explore_node_remove (n);
    }
}

} /* namespace graph */
} /* namespace psynth */
