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

#include <libpsynth/common/Config.h>
#include <libpsynth/common/Misc.h>
#include <libpsynth/table/Table.h>
#include <libpsynth/psynth/OutputDirector.h>
#include <libpsynth/psynth/FileManagerDirector.h>

namespace psynth
{

class Director : public NoCopy
{
    typedef std::map<std::string, OutputDirectorFactory*> ODFMap;
    ODFMap m_outdir;

    std::string m_old_output;
    FileManagerDirector m_filemgr;
    OutputDirector* m_output;
    Table* m_table;

    ConfNode* m_config;
    AudioInfo m_info;

    bool onConfigNudge(ConfNode& node);
#if 0
    bool onSampleRateChange(ConfNode& node);
    bool onBlockSizeChange(ConfNode& node);
    bool onNumChannelsChange(ConfNode& node);
    bool onOutputChange(ConfNode& node);
#endif
    
    void registerConfig();
    void unregisterConfig();
    
    void startOutput();
    void stopOutput();
    void updateInfo();

public:
    Director() :
	m_output(NULL), m_table(NULL) {}

    ~Director();
    
    void attachOutputDirectorFactory(OutputDirectorFactory* fact);
    void start(ConfNode& conf, const std::string& home_path);
    void stop();
    
    Table* getTable() {
	return m_table;
    }

    Output* getOutput() {
	if (m_output)
	    return m_output->getOutput();
	return NULL;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_DIRECTOR_H */

