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

#ifndef PSYNTH_OUTPUT_DIRECTOR_JACK_H
#define PSYNTH_OUTPUT_DIRECTOR_JACK_H

#include <psynth/app/defaults_jack.hpp>
#include <psynth/app/output_director.hpp>
#include <psynth/io/jack_output.hpp>

namespace psynth
{

class output_director_jack : public output_director
{
    boost::signals2::connection m_on_server_change_slot;

    ~output_director_jack()
    {
	if (m_output)
	    stop();
    }
    
    void on_server_change (base::conf_node& conf)
    {
#if PSYNTH_ON_DEVICE_CHANGE_LISTENER 
        auto old_state = m_output->state ();
        build_output (*conf.parent ());
        if (old_state == io::async_state::running)
            m_output->start ();
#endif
    }
    
    virtual graph::audio_async_output_ptr
    do_start (base::conf_node& conf)
    {
     	m_on_server_change_slot =
	    conf.child ("server").on_change.connect
	    (boost::bind (&output_director_jack::on_server_change, this, _1));

        return build_output (conf);
    };

    virtual graph::audio_async_output_ptr
    build_output (base::conf_node& conf)
    {
        auto server = conf.child ("server").get<std::string> ();
	m_output = io::new_jack_output<graph::audio_const_range>(server, 44100);
	return m_output;
    }

    virtual void do_stop (base::conf_node& conf)
    {
	m_on_server_change_slot.disconnect ();
	
	if (m_output)
            m_output.reset ();
    }

public:
    void defaults (base::conf_node& conf)
    {
	conf.child ("server").def (
	    std::string (PSYNTH_DEFAULT_JACK_SERVER));
    }
};

class output_director_jack_factory : public output_director_factory
{
public:
    virtual const char* get_name () {
	return PSYNTH_DEFAULT_JACK_NAME;
    }
    
    virtual output_director* create_output_director () {
	return new output_director_jack;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_JACK_H */
