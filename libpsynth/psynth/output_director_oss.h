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

#ifndef PSYNTH_OUTPUT_DIRECTOR_OSS_H
#define PSYNTH_OUTPUT_DIRECTOR_OSS_H

#include <libpsynth/psynth/defaults_oss.h>
#include <libpsynth/psynth/output_director.h>
#include <libpsynth/output/output_oss.h>

namespace psynth
{

class output_director_oss : public output_director
{
    output_oss* m_output;

    ~output_director_oss() {
	if (m_output)
	    stop();
    }
    
    bool on_device_change(conf_node& conf) {
	std::string device;
	output::state old_state;
	
	conf.get(device);
	
	old_state = m_output->get_state();
	m_output->goto_state(output::NOTINIT);
	m_output->set_device(device);
	m_output->goto_state(old_state);

	return false;
    }
    
    virtual output* do_start(conf_node& conf) {
	std::string device;
	
	conf.get_child ("out_device").get(device);
	conf.get_child ("out_device").add_change_event(
	    MakeDelegate(this, &output_director_oss::on_device_change));

	m_output = new output_oss;
	m_output->set_device(device);

	return m_output;
    };

    virtual void do_stop(conf_node& conf) {
	conf.get_child ("out_device").delete_change_event(
	    MakeDelegate(this, &output_director_oss::on_device_change));
	delete m_output;
	m_output = NULL;
    }

public:
    void defaults(conf_node& conf) {
	conf.get_child ("out_device").def(DEFAULT_OSS_OUT_DEVICE);
    }

    output_director_oss() :
	m_output(NULL) {}
};

class output_director_oss_factory : public output_director_factory
{
public:
    virtual const char* get_name() {
	return DEFAULT_OSS_NAME;
    }
    
    virtual output_director* create_output_director() {
	return new output_director_oss;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_OSS_H */
