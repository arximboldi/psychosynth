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

#include <libpsynth/common/misc.h>
#include <libpsynth/psynth/Director.h>

namespace psynth
{

  class arg_parser;
  
class PsychosynthApp : public no_copy
{
public:
    enum ErrCode {
	ERR_GENERIC = -1,
	SUCCESS = 0
    };
    
    Director m_director;
    std::string m_cfg_dir;

    void generatePaths();
    bool parseArgs(int argc, const char* argv[]);
    
    std::string getConfigPath();
    std::string getDataPath();
    
    virtual void prepare(arg_parser& args) {}
    virtual int execute() { return ERR_GENERIC; }

    virtual void printHelp() {}
    virtual void printVersion() {}

protected:
    void printBaseOptions(std::ostream& out);
    
public:
    Table* getTable() {
	return m_director.getTable();
    };
    
    Output* getOutput() {
	return m_director.getOutput();
    };
    
    int run(int argc, const char* argv[]);

    void setupSynth();
    void closeSynth();
};

} /* namespace psynth */

#endif /* PSYNTH_PSYCHOSYNTHAPP_H */
