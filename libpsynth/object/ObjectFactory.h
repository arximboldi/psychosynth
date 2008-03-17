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

#ifndef PSYNTH_OBJECTFACTORY_H
#define PSYNTH_OBJECTFACTORY_H

#include <libpsynth/object/Object.h>

#define PSYNTH_DECLARE_OBJECT_FACTORY( T, name )\
class T ## Factory : public ObjectFactory\
{\
public:\
    Object* create(const AudioInfo& info)\
    {\
        return new T (info);\
    }\
    \
    void destroy(Object* obj)\
    {\
        delete obj;\
    }\
    const char* getName() {\
        return name;\
    }\
};\
T ## Factory& get ## T ## Factory();

#define PSYNTH_DEFINE_OBJECT_FACTORY( T )\
T ## Factory& get ## T ## Factory()\
{\
    static T ## Factory s_factory;\
    return s_factory;\
}

namespace psynth
{

class ObjectFactory
{
public:

    virtual const char* getName() = 0;
    virtual Object* create(const AudioInfo& m_info) = 0;
    virtual void destroy(Object*) = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTFACTORY_H */
