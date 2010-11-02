/**
 *  Time-stamp:  <2010-11-03 00:25:43 raskolnikov>
 *
 *  @file        conf_backend_xml.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  XML psynthd configuration backend.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define PSYNTH_MODULE_NAME "psynth.base.xml"

#include <sstream>
#include <boost/lexical_cast.hpp>

#include "misc.hpp"
#include "conf_backend_xml.hpp"

using namespace boost;
using namespace std;

static const int         default_int    = 0;
static const int         default_float  = 0;
static const std::string default_string = "";

namespace
{

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

/*
 * Tool for disposal of wrapped C items.
 */
template<typename T>
class auto_free
{
public:
    typedef void (*dispose_func_t)(T*);

    auto_free (T* ptr, dispose_func_t f)
	: _ptr (ptr)
	, _f (f)
    {}

    ~auto_free ()
    {
	if (_ptr)
	    _f (_ptr);
    }

    operator T* ()
    {
	return _ptr;
    }

private:
    T* _ptr;
    dispose_func_t _f;
};

} /* namespace */

namespace psynth
{
namespace base
{

PSYNTH_DEFINE_ERROR (config_xml_error)
PSYNTH_DEFINE_ERROR_WHAT  (config_xml_type_error, "Wrong type.")
PSYNTH_DEFINE_ERROR_WHAT  (config_xml_io_error,   "Could not open file.")

conf_node* conf_backend_xml::process_new_element (xmlTextReaderPtr reader,
						  conf_node* node)
{
    xmlChar* type;
    
    if (xmlTextReaderDepth (reader) == 0)
    {
	if ((node->name ().empty() &&
	     xmlStrcmp (xmlTextReaderConstName(reader), xml_cast ("root"))) ||
	    (!node->name ().empty() &&
	     xmlStrcmp (xmlTextReaderConstName(reader), xml_cast (node->name ().c_str()))))
	    return 0;
    }
    else if (!xmlTextReaderIsEmptyElement(reader))
	node = &node->child (char_cast (xmlTextReaderConstName(reader)));

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
	    throw config_xml_type_error ();
	
	xmlFree(type);
    }

    return node;
}

conf_node* conf_backend_xml::process_end_element (xmlTextReaderPtr reader,
						  conf_node* node)

{
    return node->parent();
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
	    throw config_xml_type_error ();
    }

    return node;
}

conf_node* conf_backend_xml::process (xmlTextReaderPtr reader,
				      conf_node* node)
{
    switch (xmlTextReaderNodeType (reader))
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
    int ret;
    conf_node* cur_node;

    auto_free<xmlTextReader> reader
	(xmlReaderForFile (m_file.c_str(), NULL,
			   XML_PARSE_NOENT |
			   XML_PARSE_NOBLANKS),
	 &xmlFreeTextReader);
    
    if (reader == 0)
	throw config_xml_io_error ("Could not open config file for reading: " +
				   m_file +
				   ". The application might create it later.");
    
    cur_node = &node;
	
    ret = xmlTextReaderRead (reader);
    while (ret == 1 && cur_node != 0) {
	cur_node = process (reader, cur_node);
	ret = xmlTextReaderRead (reader);
    }

    if (ret != 0)
	throw config_xml_error ("Error while loading file.");
}

void conf_backend_xml::expand_value (xmlTextWriterPtr writer, conf_node& node)
{
    if (node.type () == typeid (int))
    {
	int val = default_int;
	string repr;
	node.get (val);
	repr = lexical_cast<string> (val);
	
	if (xmlTextWriterWriteAttribute (writer, xml_cast ("type"), xml_cast ("int")))
	    throw config_xml_error ("Error while writing node: " + node.path_name ());
	if (xmlTextWriterWriteString (writer, xml_cast (repr.c_str ())))
	    throw config_xml_error ("Error while writing node: " + node.path_name ());
    }
    else if (node.type () == typeid (float))
    {
	float val = default_int;
	string repr;
	node.get (val);
	repr = lexical_cast<string> (val);
	
	if (xmlTextWriterWriteAttribute (writer, xml_cast ("type"), xml_cast ("float")))
	    throw config_xml_error ("Error while writing node: " + node.path_name ());
	if (xmlTextWriterWriteString (writer, xml_cast (repr.c_str ())))
	    throw config_xml_error ("Error while writing node: " + node.path_name ());
    }
    else if (node.type () == typeid (std::string))
    {
	string repr;
	node.get (repr);
	
	if (xmlTextWriterWriteAttribute (writer, xml_cast ("type"), xml_cast ("string")))
	    throw config_xml_error ("Error while writing node: " + node.path_name ());
	if (xmlTextWriterWriteString (writer, xml_cast (repr.c_str ())))
	    throw config_xml_error ("Error while writing node: " + node.path_name ());
    }
    else if (!node.empty ())
	throw config_xml_type_error ();    
}

void conf_backend_xml::expand_childs (xmlTextWriterPtr writer, conf_node& node)
{
    conf_node::iterator i;

    for (i = node.begin(); i != node.end(); ++i)
	expand (writer, *i);
}

void conf_backend_xml::expand (xmlTextWriterPtr writer, conf_node& node)
{
    if (node.name ().empty()) {
	if (xmlTextWriterStartElement (writer, xml_cast ("root")) < 0)
	    throw config_xml_error ("Error while expanding node: " + node.path_name ());
    } else {
	if (xmlTextWriterStartElement (writer, xml_cast (node.name ().c_str())) < 0)
	    throw config_xml_error ("Error while expanding node: " + node.path_name ());
    }
    
    expand_value  (writer, node);
    expand_childs (writer, node);
    
    if (xmlTextWriterEndElement (writer) < 0)
	throw config_xml_error ("Error while ending node: " + node.path_name ());
}

void conf_backend_xml::save (conf_node& node)
{
    auto_free<xmlTextWriter> writer
	(xmlNewTextWriterFilename (m_file.c_str(), 0),
	 &xmlFreeTextWriter);

    if (writer == 0)
	throw config_xml_io_error ("Could not open config file for writing: " +
				   m_file);

    if (xmlTextWriterSetIndent (writer, 1) < 0)
	throw config_xml_error ("Error while setting indent size.");
    
    if (xmlTextWriterStartDocument (writer, 0, 0, 0) < 0)
	throw config_xml_error ("Error while starting document.");

    expand (writer, node);
    
    if (xmlTextWriterEndDocument (writer) < 0)
	throw config_xml_error ("Error while ending document.");
}

} /* namespace base */
} /* namespace psynth */

