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

#include "psynth/Director.h"
#include "psynth/Defaults.h"

using namespace std;

Director::~Director()
{
    for (ODFMap::iterator i = m_outdir.begin(); i != m_outdir.end(); ++i)
	delete i->second;
}

void Director::attachOutputDirectorFactory(OutputDirectorFactory* fact)
{
    m_outdir[fact->getName()] = fact;
}

void Director::stopOutput()
{
    if (m_output) {
	m_output->getOutput()->close();
	m_table->detachOutput(m_output->getOutput());
	
	m_output->stop();   
	
	delete m_output;
	m_output = NULL;
    }
}

void Director::startOutput()
{
    std::string out_name;

    m_config->getChild("output").get(out_name);
    
    ODFMap::iterator i = m_outdir.find(out_name);
    if (i != m_outdir.end()) {
	m_output = i->second->createOutputDirector();
	m_output->start(m_config->getChild(i->second->getName()));

	m_table->attachOutput(m_output->getOutput());

	m_output->getOutput()->setInfo(m_info);
	m_output->getOutput()->open();
	m_output->getOutput()->start();
    }
}

void Director::registerConfig()
{
    m_config->getChild("sample_rate") .def(DEFAULT_SAMPLE_RATE);
    m_config->getChild("block_size")  .def(DEFAULT_BLOCK_SIZE);
    m_config->getChild("num_channels").def(DEFAULT_NUM_CHANNELS);
    m_config->getChild("output")      .def(DEFAULT_OUTPUT);

    m_config->getChild("sample_rate") .addChangeEvent(MakeEvent(this, &Director::onSampleRateChange));
    m_config->getChild("block_size")  .addChangeEvent(MakeEvent(this, &Director::onBlockSizeChange));
    m_config->getChild("num_channels").addChangeEvent(MakeEvent(this, &Director::onNumChannelsChange));
    m_config->getChild("output")      .addChangeEvent(MakeEvent(this, &Director::onOutputChange));
}

void Director::unregisterConfig()
{
    m_config->getChild("sample_rate") .deleteChangeEvent(MakeEvent(this, &Director::onSampleRateChange));
    m_config->getChild("block_size")  .deleteChangeEvent(MakeEvent(this, &Director::onBlockSizeChange));
    m_config->getChild("num_channels").deleteChangeEvent(MakeEvent(this, &Director::onNumChannelsChange));
    m_config->getChild("output")      .deleteChangeEvent(MakeEvent(this, &Director::onOutputChange));    
}

void Director::start(ConfNode& conf)
{
    m_config = &conf;

    registerConfig();

    conf.getChild("sample_rate").get(m_info.sample_rate);
    conf.getChild("num_channels").get(m_info.num_channels);
    conf.getChild("block_size").get(m_info.block_size);
    
    m_table = new Table(m_info);
    
    startOutput();
}

void Director::stop()
{
    unregisterConfig();
    
    stopOutput();
    delete m_table;
    
    m_table = NULL;
    m_output = NULL;
    m_config = NULL;
}

bool Director::onSampleRateChange(const ConfNode& node)
{
    cout << "SAMPLE_RATE_CHANGE!\n";
    node.get(m_info.sample_rate);
    m_table->setInfo(m_info);
    return false;
}

bool Director::onBlockSizeChange(const ConfNode& node)
{
    node.get(m_info.block_size);
    m_table->setInfo(m_info);    
    return false;
}

bool Director::onNumChannelsChange(const ConfNode& node)
{
    node.get(m_info.num_channels);
    m_table->setInfo(m_info);
    return false;
}

bool Director::onOutputChange(const ConfNode& node)
{
    stopOutput();
    startOutput();
    return false;
}
