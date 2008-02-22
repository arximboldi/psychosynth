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

#include "common/Logger.h"
#include "common/Misc.h"
#include "common/ConfBackendXML.h"

#define DEFAULT_FLOAT_VALUE  0.0f
#define DEFAULT_INT_VALUE    0
#define DEFAULT_STRING_VALUE ""

#define DEFAULT_EMPTY_NAME "config"

#define CHAR_CAST reinterpret_cast<const char*>
#define XML_CAST  reinterpret_cast<const xmlChar*>

using namespace std;

ConfNode* ConfBackendXML::processNewElement(xmlTextReaderPtr reader, ConfNode* node)
{
    xmlChar* type;

    if (xmlTextReaderDepth(reader) == 0) {
	if ((!node->getName().empty()
	     && xmlStrcmp(xmlTextReaderConstName(reader), XML_CAST("root")))
	    || xmlStrcmp(xmlTextReaderConstName(reader),
			 XML_CAST(node->getName().c_str()))) {
	    return NULL;
	}
    } else if (!xmlTextReaderIsEmptyElement(reader)) {
	node = &node->getChild(CHAR_CAST(xmlTextReaderConstName(reader)));
    }

    type = xmlTextReaderGetAttribute(reader, XML_CAST ("type"));

    if (type != NULL) {
	if(xmlStrcmp(type, XML_CAST ("int")) == 0)
	    node->set(DEFAULT_INT_VALUE);
	else if (xmlStrcmp(type, XML_CAST ("float")) == 0)
	    node->set(DEFAULT_FLOAT_VALUE);
	else if (xmlStrcmp(type, XML_CAST ("string")) == 0)
	    node->set(DEFAULT_STRING_VALUE);
		
	xmlFree(type);
    }

    return node;
}

ConfNode* ConfBackendXML::processEndElement(xmlTextReaderPtr reader, ConfNode* node)
{
    return node->getParent();
}

ConfNode* ConfBackendXML::processText(xmlTextReaderPtr reader, ConfNode* node)
{
    const xmlChar* value;
    
    value = xmlTextReaderConstValue(reader);

    if (value) {
	istringstream value_str(CHAR_CAST(value));
	
	switch(node->type()) {
	case CONF_INT: {
	    int val;
	    value_str >> val;
	    node->set(val);
	}
	break;

	case CONF_FLOAT: {
	    float val;
	    value_str >> val;
	    node->set(val);
	}
	break;
	
	case CONF_STRING: {
	    string val;
	    value_str >> val;
	    node->set(val);
	}
	break;
	
	default:
	    break;
	}
    }

    return node;
}

ConfNode* ConfBackendXML::process(xmlTextReaderPtr reader, ConfNode* node)
{
    switch(xmlTextReaderNodeType(reader)) {
    case XML_READER_TYPE_ELEMENT:
	return processNewElement(reader, node);

    case XML_READER_TYPE_END_ELEMENT:
	return processEndElement(reader, node);

    case XML_READER_TYPE_TEXT:
	return processText(reader, node);
	
    default:
	break;
    }

    return node;
}

void ConfBackendXML::load(ConfNode& node)
{
    xmlTextReaderPtr reader;
    int ret;
    ConfNode* cur_node;
    
    reader = xmlReaderForFile(m_file.c_str(), NULL,
			      XML_PARSE_NOENT |
			      XML_PARSE_NOBLANKS);
    
    if (reader != NULL) {
	cur_node = &node;
	    
	ret = xmlTextReaderRead(reader);
        while (ret == 1 && cur_node != NULL) {
            cur_node = process(reader, cur_node);
            ret = xmlTextReaderRead(reader);
        }

	xmlFreeTextReader(reader);

	if (ret != 0) {
	    Logger::instance().log("xmlconf", Log::ERROR, "Failed to parse config file: " + m_file);
        }
	
    } else {
	Logger::instance().log("xmlconf", Log::ERROR, "Could not open config file for writing: " + m_file);
    }
}

void ConfBackendXML::expandValue(xmlTextWriterPtr writer, ConfNode& node)
{
    switch(node.type()) {
    case CONF_INT: {
	int val = 0;
	node.get(val);
	xmlTextWriterWriteAttribute(writer, XML_CAST ("type"), XML_CAST ("int"));
	xmlTextWriterWriteString(writer, XML_CAST (itoa(val, 10)));
    }
    break;
	
    case CONF_FLOAT: {
	float val = 0;
	node.get(val);
	xmlTextWriterWriteAttribute(writer, XML_CAST ("type"), XML_CAST ("float"));
	xmlTextWriterWriteString(writer, XML_CAST (ftoa(val, 8)));
    }
    break;
	
    case CONF_STRING: {
	string val;
	node.get(val);
	
	xmlTextWriterWriteAttribute(writer, XML_CAST ("type"), XML_CAST ("string"));
	xmlTextWriterWriteString(writer, XML_CAST (val.c_str()));
    }
    break;
	    
    default:
	cout << "no hay valor!\n";
	break;
    }

}

void ConfBackendXML::expandChilds(xmlTextWriterPtr writer, ConfNode& node)
{
    ConfNode::ChildIter i;

    for (i = node.begin(); i != node.end(); ++i)
	expand(writer, *i);
}

void ConfBackendXML::expand(xmlTextWriterPtr writer, ConfNode& node)
{
    if (node.getName().empty()) {
	xmlTextWriterStartElement(writer, XML_CAST ("root"));
    } else {
	xmlTextWriterStartElement(writer, XML_CAST (node.getName().c_str()));
    }

    cout << "expanding: " << node.getName() << endl;
    
    expandValue(writer, node);
    expandChilds(writer, node);
    
    xmlTextWriterEndElement(writer);
}

void ConfBackendXML::save(ConfNode& node)
{
    xmlTextWriterPtr writer;
    
    writer = xmlNewTextWriterFilename(m_file.c_str(), 0);
    if (writer == NULL) {
	Logger::instance().log("xmlconf", Log::ERROR, "Could not open config file for writing: " + m_file);
        return;
    }

    xmlTextWriterSetIndent(writer, 1);
    xmlTextWriterStartDocument(writer, NULL, NULL, NULL);

    expand(writer, node);
    
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
}
