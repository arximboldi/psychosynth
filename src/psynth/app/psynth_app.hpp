/**
 *  Time-stamp:  <2016-04-15 15:11:06 raskolnikov>
 *
 *  @file        psynth_app.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Argument parsing facility interface.
 *  This file is part of the Psychosynth project.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_PSYCHOSYNTHAPP_H
#define PSYNTH_PSYCHOSYNTHAPP_H

#include <psynth/warnings.hpp>
PSYNTH_DISABLE_WARNINGS
#include <boost/filesystem.hpp>
PSYNTH_RESTORE_WARNINGS
#include <psynth/app/director.hpp>

namespace psynth
{

namespace base { class arg_parser; }

class psynth_app : public boost::noncopyable
{
public:
    enum err_code {
	ERR_GENERIC = -1,
	SUCCESS = 0
    };
    
    director m_director;

    boost::filesystem::path m_cfg_dir;
    boost::filesystem::path m_data_dir;
    
    void generate_paths ();
    bool parse_args (int argc, const char* argv[]);
    
    boost::filesystem::path get_config_path ();
    boost::filesystem::path get_data_path ();
    
    virtual void prepare (base::arg_parser& args) {}
    virtual int execute () { return ERR_GENERIC; }

    virtual void print_help () {}
    virtual void print_version () {}

protected:
    void print_base_options (std::ostream& out);
    
public:
    world* get_world () {
	return m_director.get_world ();
    };
    
    graph::audio_async_output_ptr get_output () {
	return m_director.get_output ();
    };
    
    int run (int argc, const char* argv[]);

    void setup_synth ();
    void close_synth ();
};

} /* namespace psynth */

#endif /* PSYNTH_PSYCHOSYNTHAPP_H */
