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

#ifndef PSYNTH_OUTPUT_DIRECTOR_ALSA_H
#define PSYNTH_OUTPUT_DIRECTOR_ALSA_H

#include <psynth/app/defaults_alsa.hpp>
#include <psynth/app/output_director.hpp>
#include <psynth/io/output_alsa.hpp>

namespace psynth
{

class output_director_alsa : public output_director
{
    output_alsa* m_output;
    sigc::connection m_on_device_change_slot;
    
    ~output_director_alsa()
    {
	if (m_output)
	    stop();
    }
    
    void on_device_change (base::conf_node& conf)
    {
	std::string device;
	output::state old_state;
	
	conf.get(device);
	
	old_state = m_output->get_state();
	m_output->goto_state(output::NOTINIT);
	m_output->set_device(device);
	m_output->goto_state(old_state);
    }
    
    virtual output* do_start (base::conf_node& conf)
    {
	std::string device;
	
	conf.get_child ("out_device").get (device);
	m_on_device_change_slot =
	    conf.get_child ("out_device").on_change.connect
	    (sigc::mem_fun (*this, &output_director_alsa::on_device_change));

	m_output = new output_alsa;
	m_output->set_device(device);

	return m_output;
    };

    virtual void do_stop (base::conf_node& conf)
    {
	m_on_device_change_slot.disconnect ();
	
	if (m_output) {
	    delete m_output;
	    m_output = NULL;
	}
    }

public:
    void defaults (base::conf_node& conf)
    {
	conf.get_child ("out_device").def (std::string (PSYNTH_DEFAULT_ALSA_OUT_DEVICE));
    }
    
    output_director_alsa () :
	m_output(NULL) {}
};

class output_director_alsa_factory : public output_director_factory
{
public:
    virtual const char* get_name()
    {
	return PSYNTH_DEFAULT_ALSA_NAME;
    }
    
    virtual output_director* create_output_director()
    {
	return new output_director_alsa;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_ALSA_H */
