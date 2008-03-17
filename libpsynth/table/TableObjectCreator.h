/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef PSYNTH_TABLEOBJECTCREATOR_H
#define PSYNTH_TABLEOBJECTCREATOR_H

#include <list>
#include <string>
#include <iostream>
#include <libpsynth/table/Table.h>

namespace psynth
{

class ObjectFactoryManager;
class AudioInfo;
class Object;

class TableObjectCreator
{
    class ParamMakerBase
    {
    public:
	virtual ~ParamMakerBase() {}
	virtual void apply(TableObject& obj, const std::string& par) = 0;
	virtual ParamMakerBase* clone() const = 0;
    };

    template <class T>
    class ParamMaker : public ParamMakerBase
    {
	T m_val;
    public:
	ParamMaker(const T& val) :
	    m_val(val) {}
	    
	void apply(TableObject& obj, const std::string& par) {
	    obj.setParam(par, m_val);
	}

	void set(const T& val) {
	    m_val = val;
	}
	
	ParamMakerBase* clone() const {
	    return new ParamMaker<T> (m_val);
	}
    };

    typedef std::list<std::pair<std::string, ParamMakerBase*> > ParamMakerList;

    ParamMakerList m_param;
    std::string m_name;

    void destroy();
    void copy(const TableObjectCreator& obj);
    ParamMakerBase* find(const std::string& name);
    
public:
    TableObjectCreator() {}

    TableObjectCreator(const TableObjectCreator& obj) {
	copy(obj);
    }
    
    ~TableObjectCreator() {
	destroy();
    };

    TableObjectCreator& operator= (const TableObjectCreator& obj) {
	if (this != &obj) {
	    destroy();
	    copy(obj);
	}
	return *this;
    }
    
    TableObject create(Table& table);

    void setName(const std::string& name) {
	m_name = name;
    }
    
    template<class T>
    void addParam(const std::string& name, const T& value) {
	m_param.push_back(std::make_pair(std::string(name),
					 (ParamMakerBase*) new ParamMaker<T>(value)));
    }

    template<class T>
    void setParam(const std::string& name, const T& value) {
	ParamMaker<T>* param = (ParamMaker<T>*) find(name);
	if (param) param->set(value);
	else addParam(name, value);
    };

    void clear() {
	destroy();
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTCREATOR_H */
