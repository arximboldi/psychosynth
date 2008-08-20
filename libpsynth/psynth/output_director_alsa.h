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

#include <libpsynth/psynth/defaults_alsa.h>
#include <libpsynth/psynth/output_director.h>
#include <libpsynth/output/OutputAlsa.h>

namespace psynth
{

class output_director_alsa : public output_director
{
    OutputAlsa* m_output;

    ~output_director_alsa() {
	if (m_output)
	    stop();
    }
    
    bool on_device_change (conf_node& conf) {
	std::string device;
	Output::State old_state;
	
	conf.get(device);
	
	old_state = m_output->getState();
	m_output->gotoState(Output::NOTINIT);
	m_output->setDevice(device);
	m_output->gotoState(old_state);

	return false;
    }
    
    virtual Output* do_start (conf_node& conf) {
	std::string device;
	
	conf.get_child ("out_device").get(device);
	conf.get_child ("out_device").add_change_event(
	    MakeDelegate(this, &output_director_alsa::on_device_change)
	    );

	m_output = new OutputAlsa;
	m_output->setDevice(device);

	return m_output;
    };

    virtual void do_stop (conf_node& conf) {
	conf.get_child ("out_device").delete_change_event(
	    MakeDelegate(this, &output_director_alsa::on_device_change)
	    );
	
	if (m_output) {
	    delete m_output;
	    m_output = NULL;
	}
    }

public:
    void defaults (conf_node& conf) {
	conf.get_child ("out_device").def(DEFAULT_ALSA_OUT_DEVICE);
    }
    
    output_director_alsa () :
	m_output(NULL) {}
};

class output_director_alsa_factory : public output_director_factory
{
public:
    virtual const char* get_name() {
	return DEFAULT_ALSA_NAME;
    }
    
    virtual output_director* create_output_director() {
	return new output_director_alsa;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_ALSA_H */
