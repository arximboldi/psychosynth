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

#include <libpsynth/common/config.h>

namespace psynth
{

/**
 * XML configuration backend that stores the config to an XML file. This
 * is not a live backend so it needs explicit calls on @c save and @c load
 * on the @c conf_node.
 */
class conf_backend_xml : public conf_backend
{
    std::string m_file;
    bool m_defaulty;
    
    conf_node* process_new_element (xmlTextReaderPtr reader, conf_node* node);
    conf_node* process_text (xmlTextReaderPtr reader, conf_node* node);
    conf_node* process_end_element (xmlTextReaderPtr reader, conf_node* node);
    conf_node* process (xmlTextReaderPtr reader, conf_node* node);

    void do_load (conf_node& node);
    
    void expand (xmlTextWriterPtr writer, conf_node& node);
    void expand_childs (xmlTextWriterPtr writer, conf_node& node);
    void expand_value (xmlTextWriterPtr writer, conf_node& node);

public:
    /**
     * Constructor.
     * @param file The name of the file where to store and load
     * the configuration.
     */
    conf_backend_xml (const std::string& file)
	: m_file(file)
	{}

    /** Constructor */
    ~conf_backend_xml () {}

    /**
     * Changes the name of the file to use.
     * @param file The new file name to use.
     */
    void set_file (const std::string& file) {
	m_file = file;
    }

    /**
     * Returns the name of the current filename.
     */
    const std::string& get_file () {
	return m_file;
    }

    /** Called when attached to a node. Does nothing. */
    void attach (conf_node& node) {};

    /** Called when dattached from a node. Does nothing. */
    void datach (conf_node& node) {};

    /**
     * Permanently stores the configuration in the currently set file.
     * @param node The configuration to store.
     */
    void save (conf_node& node);

    /**
     * Loads the conentent of the XML file into a node.
     * @param node The node where to store the config.
     */
    void load (conf_node& node) {
	m_defaulty = false;
	do_load(node);
    }

    /**
     * Loads the content of the XML file into a node but preserving the node
     * values that are already set.
     */
    void def_load (conf_node& node) {
	m_defaulty = true;
	do_load(node);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_CONFBACKENDXML_H */
