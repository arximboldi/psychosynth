/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007, 2008                    *
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

#ifndef PSYNTH_OUTPUT_DIRECTOR_H
#define PSYNTH_OUTPUT_DIRECTOR_H

#include <psynth/base/config.hpp>
#include <psynth/graph/node_output.hpp>

namespace psynth
{

class output_director
{
protected:
    base::conf_node* m_conf;
    graph::audio_async_output_ptr m_output;
    
    virtual graph::audio_async_output_ptr
    do_start (base::conf_node& conf) = 0;

    virtual void
    do_stop (base::conf_node& conf) = 0;

public:
    virtual void defaults (base::conf_node& conf) = 0;
    
    output_director ()
	: m_conf (0)
	, m_output ()
	{}
    
    virtual ~output_director () {}

    void start (base::conf_node& conf)
    {
	m_conf = &conf;
	m_output = do_start (conf);
    }

    void stop ()
    {
	do_stop (*m_conf);
	m_output.reset ();
    }
    
    graph::audio_async_output_ptr get_output() const
    { return m_output; }
};

class output_director_factory
{
public:
    virtual ~output_director_factory () {}
    virtual const char* get_name () = 0;
    virtual output_director* create_output_director () = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_H */
