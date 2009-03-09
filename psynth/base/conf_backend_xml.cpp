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

#include <sstream>

#include "base/logger.hpp"
#include "base/misc.hpp"
#include "base/conf_backend_xml.hpp"

#define DEFAULT_FLOAT_VALUE  0.0f
#define DEFAULT_INT_VALUE    0
#define DEFAULT_STRING_VALUE ""

#define DEFAULT_EMPTY_NAME   "config"

#define CHAR_CAST reinterpret_cast<const char*>
#define XML_CAST  reinterpret_cast<const xmlChar*>

using namespace std;

namespace psynth
{

conf_node* conf_backend_xml::process_new_element (xmlTextReaderPtr reader,
						  conf_node* node)
{
    xmlChar* type;
    
    if (xmlTextReaderDepth(reader) == 0) {
	if (node->get_name ().empty()) {
	    if (xmlStrcmp (xmlTextReaderConstName(reader),
			   XML_CAST ("root")))
		return NULL;
	} else if (xmlStrcmp (xmlTextReaderConstName(reader),
			      XML_CAST (node->get_name ().c_str()))) 
	    return NULL;
       
    } else if (!xmlTextReaderIsEmptyElement(reader)) {
	node = &node->get_child (CHAR_CAST (xmlTextReaderConstName(reader)));
    }

    type = xmlTextReaderGetAttribute(reader, XML_CAST ("type"));

    if (type != NULL) {
	conf_type ct = CONF_INT;
	
	if(xmlStrcmp(type, XML_CAST ("int")) == 0)
	    ct = CONF_INT;
	else if (xmlStrcmp(type, XML_CAST ("float")) == 0)
	    ct = CONF_FLOAT;
	else if (xmlStrcmp(type, XML_CAST ("string")) == 0)
	    ct = CONF_STRING;

	if (!m_defaulty) node->set_type(ct);
	else node->def_type(ct);
	
	xmlFree(type);
    }

    return node;
}

conf_node* conf_backend_xml::process_end_element (xmlTextReaderPtr reader,
						  conf_node* node)
{
    return node->get_parent();
}

conf_node* conf_backend_xml::process_text (xmlTextReaderPtr reader,
					   conf_node* node)
{
    const xmlChar* value;
    
    value = xmlTextReaderConstValue(reader);

    if (value) {
	istringstream value_str(CHAR_CAST(value));
	
	switch(node->type()) {
	case CONF_INT: {
	    int val;
	    value_str >> val;
	    if (!m_defaulty) node->set(val);
	    else node->def(val);
	}
	    break;

	case CONF_FLOAT: {
	    float val;
	    value_str >> val;
	    if (!m_defaulty) node->set(val);
	    else node->def(val);
	}
	    break;
	
	case CONF_STRING: {
	    string val;
	    value_str >> val;
	    if (!m_defaulty) node->set(val);
	    else node->def(val);
	}
	    break;
	
	default:
	    break;
	}
    }

    return node;
}

conf_node* conf_backend_xml::process (xmlTextReaderPtr reader,
				      conf_node* node)
{
    switch(xmlTextReaderNodeType(reader)) {
    case XML_READER_TYPE_ELEMENT:
	return process_new_element (reader, node);

    case XML_READER_TYPE_END_ELEMENT:
	return process_end_element (reader, node);

    case XML_READER_TYPE_TEXT:
	return process_text (reader, node);
	
    default:
	break;
    }

    return node;
}

void conf_backend_xml::do_load (conf_node& node)
{
    xmlTextReaderPtr reader;
    int ret;
    conf_node* cur_node;
    
    reader = xmlReaderForFile (m_file.c_str(), NULL,
			       XML_PARSE_NOENT |
			       XML_PARSE_NOBLANKS);
    
    if (reader != NULL) {
	cur_node = &node;
	    
	ret = xmlTextReaderRead(reader);
        while (ret == 1 && cur_node != NULL) {
            cur_node = process (reader, cur_node);
            ret = xmlTextReaderRead (reader);
        }

	xmlFreeTextReader(reader);

	if (ret != 0) {
	    logger::self () ("xmlconf", log::ERROR, "Failed to parse config file: " + m_file);
        }
	
    } else {
	logger::self () ("xmlconf", log::ERROR, "Could not open config file for reading: " + m_file);
    }
}

void conf_backend_xml::expand_value (xmlTextWriterPtr writer, conf_node& node)
{
    switch(node.type()) {
    case CONF_INT:
    {
	int val = 0;
	node.get(val);
	xmlTextWriterWriteAttribute(writer, XML_CAST ("type"), XML_CAST ("int"));
	xmlTextWriterWriteString(writer, XML_CAST (itoa(val, 10)));
    }
    break;
	
    case CONF_FLOAT:
    {
	float val = 0;
	node.get(val);
	xmlTextWriterWriteAttribute(writer, XML_CAST ("type"), XML_CAST ("float"));
	xmlTextWriterWriteString(writer, XML_CAST (ftoa(val, 8)));
    }
    break;
	
    case CONF_STRING:
    {
	string val;
	node.get(val);
	
	xmlTextWriterWriteAttribute(writer, XML_CAST ("type"), XML_CAST ("string"));
	xmlTextWriterWriteString(writer, XML_CAST (val.c_str()));
    }
    break;
	    
    default:
	break;
    }

}

void conf_backend_xml::expand_childs (xmlTextWriterPtr writer, conf_node& node)
{
    conf_node::iterator i;

    for (i = node.begin(); i != node.end(); ++i)
	expand (writer, *i);
}

void conf_backend_xml::expand (xmlTextWriterPtr writer, conf_node& node)
{
    if (node.get_name ().empty())
	xmlTextWriterStartElement (writer, XML_CAST ("root"));
    else
	xmlTextWriterStartElement (writer, XML_CAST (node.get_name ().c_str()));

    std::cout << "starting element: " << node.get_name () << std::endl;
    expand_value  (writer, node);
    expand_childs (writer, node);
    
    std::cout << "ending element: " << node.get_name () << std::endl;
    xmlTextWriterEndElement (writer);
}

void conf_backend_xml::save (conf_node& node)
{
    xmlTextWriterPtr writer;
    
    writer = xmlNewTextWriterFilename (m_file.c_str(), 0);

    if (writer == NULL) {
	logger::self () ("xmlconf", log::ERROR, "Could not open config file for writing: " + m_file);
        return;
    }

    xmlTextWriterSetIndent (writer, 1);
    xmlTextWriterStartDocument (writer, NULL, NULL, NULL);

    expand (writer, node);
    
    xmlTextWriterEndDocument (writer);
    xmlFreeTextWriter (writer);
}

} /* namespace psynth */

