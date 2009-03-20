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
#include <boost/lexical_cast.hpp>

#include "base/logger.hpp"
#include "base/misc.hpp"
#include "base/conf_backend_xml.hpp"

using namespace boost;
using namespace std;

static const int         default_int    = 0;
static const int         default_float  = 0;
static const std::string default_string = "";

static inline const char*
char_cast (const xmlChar* str)
{
    return reinterpret_cast<const char*> (str);
}

static inline const xmlChar*
xml_cast (const char* str)
{
    return reinterpret_cast<const xmlChar*> (str);
}

namespace psynth
{

conf_node* conf_backend_xml::process_new_element (xmlTextReaderPtr reader,
						  conf_node* node)
{
    xmlChar* type;
    
    if (xmlTextReaderDepth (reader) == 0) {
	if ((node->get_name ().empty()
	     && xmlStrcmp (xmlTextReaderConstName(reader),
			   xml_cast ("root")))
	    || xmlStrcmp (xmlTextReaderConstName(reader),
			  xml_cast (node->get_name ().c_str()))) 
	    return 0;
    }
    else if (!xmlTextReaderIsEmptyElement(reader))
	node = &node->get_child (char_cast (xmlTextReaderConstName(reader)));

    type = xmlTextReaderGetAttribute (reader, xml_cast ("type"));

    if (type != 0)
    {
	if (xmlStrcmp (type, xml_cast ("int")) == 0)
	    m_current_type = typeid (int);
	else if (xmlStrcmp (type, xml_cast ("float")) == 0)
	    m_current_type = typeid (float);
	else if (xmlStrcmp (type, xml_cast ("string")) == 0)
	    m_current_type = typeid (std::string);
	else
	    throw conf_xml_type_error ();
	
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

    if (value)
    {
	if (m_current_type == typeid (int))
	    node->set (lexical_cast<int> (char_cast (value)), m_overwrite);
	else if (m_current_type == typeid (float))
	    node->set (lexical_cast<float> (char_cast (value)), m_overwrite);
	else if (m_current_type == typeid (std::string))
	    node->set (lexical_cast<string> (char_cast (value)), m_overwrite);
	else
	    throw conf_xml_type_error ();
    }

    return node;
}

conf_node* conf_backend_xml::process (xmlTextReaderPtr reader,
				      conf_node* node)
{
    switch(xmlTextReaderNodeType(reader))
    {
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
    if (reader == 0)
	throw conf_xml_io_error ("Could not open config file for reading.");
    
    cur_node = &node;
	    
    ret = xmlTextReaderRead(reader);
    while (ret == 1 && cur_node != NULL) {
	cur_node = process (reader, cur_node);
	ret = xmlTextReaderRead (reader);
    }

    xmlFreeTextReader(reader);

    if (ret != 0)
	throw conf_xml_io_error ("Failed to parse config file");
}

void conf_backend_xml::expand_value (xmlTextWriterPtr writer, conf_node& node)
{
    if (node.type () == typeid (int))
    {
	int val = default_int;
	string repr;
	node.get (val);
	repr = lexical_cast<string> (val);
	
	xmlTextWriterWriteAttribute (writer, xml_cast ("type"), xml_cast ("int"));
	xmlTextWriterWriteString (writer, xml_cast (repr.c_str ()));
    }
    else if (node.type () == typeid (float))
    {
	float val = default_int;
	string repr;
	node.get (val);
	repr = lexical_cast<string> (val);
	
	xmlTextWriterWriteAttribute (writer, xml_cast ("type"), xml_cast ("float"));
	xmlTextWriterWriteString (writer, xml_cast (repr.c_str ()));
    }
    else if (node.type () == typeid (std::string))
    {
	string repr;
	node.get (repr);
	
	xmlTextWriterWriteAttribute (writer, xml_cast ("type"), xml_cast ("string"));
	xmlTextWriterWriteString (writer, xml_cast (repr.c_str ()));
    }
    else if (!node.empty ())
	throw conf_xml_type_error ();
    
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
	xmlTextWriterStartElement (writer, xml_cast ("root"));
    else
	xmlTextWriterStartElement (writer, xml_cast (node.get_name ().c_str()));

    expand_value  (writer, node);
    expand_childs (writer, node);
    
    xmlTextWriterEndElement (writer);
}

void conf_backend_xml::save (conf_node& node)
{
    xmlTextWriterPtr writer;
    
    writer = xmlNewTextWriterFilename (m_file.c_str(), 0);

    if (writer == 0)
	throw conf_xml_io_error ("Error while writing XML config file");

    xmlTextWriterSetIndent (writer, 1);
    xmlTextWriterStartDocument (writer, 0, 0, 0);

    expand (writer, node);
    
    xmlTextWriterEndDocument (writer);
    xmlFreeTextWriter (writer);
}

} /* namespace psynth */

