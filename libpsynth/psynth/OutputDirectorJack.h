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

#include <libpsynth/psynth/DefaultsJack.h>
#include <libpsynth/psynth/OutputDirector.h>
#include <libpsynth/output/OutputJack.h>

class OutputDirectorJack : public OutputDirector
{
    OutputJack* m_output;

    bool onServerChange(const ConfNode& conf) {
	std::string server;
	Output::State old_state;
	
	conf.get(server);
	
	old_state = m_output->getState();
	m_output->gotoState(Output::NOTINIT);
	m_output->setServer(server);
	m_output->gotoState(old_state);

	return false;
    }
  
    virtual Output* doStart(ConfNode& conf) {
	std::string server;

	conf.getChild("server").def(DEFAULT_JACK_SERVER);
	conf.getChild("server").get(server);
	conf.getChild("server").addChangeEvent(MakeEvent(this, &OutputDirectorJack::onServerChange));
	
	m_output = new OutputJack;

	m_output->setServer(server);

	return m_output;
    };

    virtual void doStop(ConfNode& conf) {
	conf.getChild("server").deleteChangeEvent(MakeEvent(this, &OutputDirectorJack::onServerChange));
	
	delete m_output;
	m_output = NULL;
    }

public:
    OutputDirectorJack() :
	m_output(NULL) {}
};

class OutputDirectorJackFactory : public OutputDirectorFactory
{
public:
    virtual const char* getName() {
	return DEFAULT_JACK_NAME;
    }
    
    virtual OutputDirector* createOutputDirector() {
	return new OutputDirectorJack;
    }
};

#endif /* PSYNTH_OUTPUT_DIRECTOR_JACK_H */
