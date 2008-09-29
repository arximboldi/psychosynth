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

#include <psynth/psynth/defaults_jack.hpp>
#include <psynth/psynth/output_director.hpp>
#include <psynth/output/output_jack.hpp>

namespace psynth
{

class output_director_jack : public output_director
{
    output_jack* m_output;
    sigc::connection m_on_server_change_slot;
    
    ~output_director_jack() {
	if (m_output)
	    stop();
    }
    
    void on_server_change (conf_node& conf) {
	std::string server;
	output::state old_state;
	
	conf.get(server);
	
	old_state = m_output->get_state();
	m_output->goto_state(output::NOTINIT);
	m_output->set_server(server);
	m_output->goto_state(old_state);
    }
  
    virtual output* do_start (conf_node& conf) {
	std::string server;

     	conf.get_child ("server").get(server);
	m_on_server_change_slot =
	    conf.get_child ("server").on_change.connect
	    (sigc::mem_fun (*this, &output_director_jack::on_server_change));
	
	m_output = new output_jack;

	m_output->set_server(server);

	return m_output;
    };

    virtual void do_stop (conf_node& conf) {
	m_on_server_change_slot.disconnect ();
	
	delete m_output;
	m_output = NULL;
    }

public:
    void defaults (conf_node& conf) {
	conf.get_child ("server").def(DEFAULT_JACK_SERVER);
    }

    output_director_jack () :
	m_output(NULL) {}
};

class output_director_jack_factory : public output_director_factory
{
public:
    virtual const char* get_name () {
	return DEFAULT_JACK_NAME;
    }
    
    virtual output_director* create_output_director () {
	return new output_director_jack;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_JACK_H */
