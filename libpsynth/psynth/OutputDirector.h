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

#ifndef PSYNTH_OUTPUT_DIRECTOR_H
#define PSYNTH_OUTPUT_DIRECTOR_H

#include <libpsynth/common/Config.h>

namespace psynth
{

class Output;

class OutputDirector
{
    ConfNode* m_conf;
    Output* m_output;
    
    virtual Output* doStart(ConfNode& conf) = 0;
    virtual void doStop(ConfNode& conf) = 0;

public:
    OutputDirector() :
	m_conf(NULL),
	m_output(NULL)
	{}
    
    virtual ~OutputDirector() {}

    void start(ConfNode& conf) {
	m_conf = &conf;
	m_output = doStart(conf);
    }

    void stop() {
	doStop(*m_conf);
	m_output = 0;
    }
    
    Output* getOutput() const {
	return m_output;
    }
};

class OutputDirectorFactory
{
public:
    virtual ~OutputDirectorFactory() {}
    virtual const char* getName() = 0;
    virtual OutputDirector* createOutputDirector() = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_DIRECTOR_H */
