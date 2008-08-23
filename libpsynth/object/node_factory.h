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

#include <libpsynth/object/node.h>

#define PSYNTH_DECLARE_NODE_FACTORY( T, name )\
class T ## _factory : public node_factory\
{\
public:\
    node* create (const audio_info& info)\
    {\
        return new T (info);\
    }\
    \
    void destroy (node* obj)\
    {\
        delete obj;\
    }\
    const char* get_name () {\
        return name;\
    }\
};\
T ## _factory& get_ ## T ## _factory();

#define PSYNTH_DEFINE_NODE_FACTORY( T )\
T ## _factory& get_ ## T ## _factory ()\
{\
    static T ## _factory s_factory;\
    return s_factory;\
}

namespace psynth
{

class node_factory
{
public:

    virtual const char* get_name () = 0;
    virtual node* create (const audio_info& m_info) = 0;
    virtual void destroy (node* nod) = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTFACTORY_H */
