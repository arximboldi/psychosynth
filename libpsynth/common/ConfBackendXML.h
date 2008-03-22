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

#ifndef PSYNTH_CONFBACKENDXML_H
#define PSYNTH_CONFBACKENDXML_H

#include <string>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <libpsynth/common/Config.h>

namespace psynth
{

/**
 * XML configuration backend that stores the config to an XML file. This
 * is not a live backend so it needs explicit calls on @c save and @c load
 * on the @c ConfNode.
 */
class ConfBackendXML : public ConfBackend
{
    std::string m_file;
    bool m_defaulty;
    
    ConfNode* processNewElement(xmlTextReaderPtr reader, ConfNode* node);
    ConfNode* processText(xmlTextReaderPtr reader, ConfNode* node);
    ConfNode* processEndElement(xmlTextReaderPtr reader, ConfNode* node);
    ConfNode* process(xmlTextReaderPtr reader, ConfNode* node);

    void doLoad(ConfNode& node);
    
    void expand(xmlTextWriterPtr writer, ConfNode& node);
    void expandChilds(xmlTextWriterPtr writer, ConfNode& node);
    void expandValue(xmlTextWriterPtr writer, ConfNode& node);

public:
    /**
     * Constructor.
     * @param file The name of the file where to store and load
     * the configuration.
     */
    ConfBackendXML(const std::string& file) :
	m_file(file)
	{}

    /** Constructor */
    ~ConfBackendXML() {}

    /**
     * Changes the name of the file to use.
     * @param file The new file name to use.
     */
    void setFile(const std::string& file) {
	m_file = file;
    }

    /**
     * Returns the name of the current filename.
     */
    const std::string& getFile() {
	return m_file;
    }

    /** Called when attached to a node. Does nothing. */
    void attach(ConfNode& node) {};
    /** Called when dattached from a node. Does nothing. */
    void datach(ConfNode& node) {};

    /**
     * Permanently stores the configuration in the currently set file.
     * @param node The configuration to store.
     */
    void save(ConfNode& node);

    /**
     * Loads the conentent of the XML file into a node.
     * @param node The node where to store the config.
     */
    void load(ConfNode& node) {
	m_defaulty = false;
	doLoad(node);
    }

    /**
     * Loads the content of the XML file into a node but preserving the node
     * values that are already set.
     */
    void defLoad(ConfNode& node) {
	m_defaulty = true;
	doLoad(node);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_CONFBACKENDXML_H */
