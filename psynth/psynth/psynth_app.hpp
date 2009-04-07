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

#ifndef PSYNTH_PSYCHOSYNTHAPP_H
#define PSYNTH_PSYCHOSYNTHAPP_H

#include <boost/filesystem.hpp>

#include <psynth/psynth/director.hpp>

namespace psynth
{

class arg_parser;

class psynth_app : public non_copyable
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
    
    virtual void prepare (arg_parser& args) {}
    virtual int execute () { return ERR_GENERIC; }

    virtual void print_help () {}
    virtual void print_version () {}

protected:
    void print_base_options (std::ostream& out);
    
public:
    world* get_world () {
	return m_director.get_world ();
    };
    
    output* get_output () {
	return m_director.get_output ();
    };
    
    int run (int argc, const char* argv[]);

    void setup_synth ();
    void close_synth ();
};

} /* namespace psynth */

#endif /* PSYNTH_PSYCHOSYNTHAPP_H */
