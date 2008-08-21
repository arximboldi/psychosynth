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

#ifndef PSYNTH_DIRECTOR_H
#define PSYNTH_DIRECTOR_H

#include <libpsynth/common/config.h>
#include <libpsynth/common/misc.h>
#include <libpsynth/table/Table.h>
#include <libpsynth/psynth/output_director.h>
#include <libpsynth/psynth/file_manager_director.h>

namespace psynth
{

class director : public no_copy
{
    typedef std::map<std::string, output_director_factory*> odf_map;
    odf_map m_outdir;

    std::string m_old_output;
    file_manager_director m_filemgr;
    output_director* m_output;
    Table* m_table;

    conf_node* m_config;
    audio_info m_info;

    bool on_config_nudge(conf_node& node);
    
#if 0
    bool on_sample_rate_change(conf_node& node);
    bool on_block_size_change(conf_node& node);
    bool on_num_channels_change(conf_node& node);
    bool on_output_change(conf_node& node);
#endif
    
    void register_config();
    void unregister_config();
    
    void start_output();
    void stop_output();
    void update_info();

public:
    director()
	: m_output(NULL)
	, m_table(NULL) {}

    ~director();
    
    void attach_output_director_factory (output_director_factory* fact);
    void start (conf_node& conf, const std::string& home_path);
    void stop ();
    
    Table* get_table () {
	return m_table;
    }

    output* get_output() {
	if (m_output)
	    return m_output->get_output ();
	return NULL;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_DIRECTOR_H */

