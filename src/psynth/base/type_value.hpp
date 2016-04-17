/**
 *  Time-stamp:  <2010-10-18 18:56:06 raskolnikov>
 *
 *  @file        type_value.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2009
 *
 *  A type_info wrapper with value semantics.
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_TYPE_INFO_H_
#define PSYNTH_TYPE_INFO_H_

#include <typeinfo>

namespace psynth
{
namespace base
{

/**
 * A @a typeid wrapper that have value semantics. This class is analogous to
 * the TypeInfo class described in Andrei Alexandrescu's "Modern C++ Design".
 */
class type_value
{
public:
    type_value ()
	: _info (&typeid (void))
    {};

    type_value (const std::type_info& t)
	: _info (&t)
    {}

    bool before (const type_value& t) const
    {
	return _info->before (*t._info);
    }

    const char* name () const
    {
	return _info->name ();
    }

    const std::type_info& get () const
    {
	return *_info;
    }

private:
    const std::type_info* _info;
};

inline bool operator== (const type_value& lhs, const type_value& rhs)
{
    return lhs.get () == rhs.get ();
}

inline bool operator< (const type_value& lhs, const type_value& rhs)
{
    return lhs.before (rhs);
}

inline bool operator!= (const type_value& lhs, const type_value& rhs)
{
    return ! (lhs == rhs);
}

inline bool operator> (const type_value& lhs, const type_value& rhs)
{
    return rhs < lhs;
}

inline bool operator<= (const type_value& lhs, const type_value& rhs)
{
    return !(lhs > rhs);
}

inline bool operator>= (const type_value& lhs, const type_value& rhs)
{
    return !(lhs < rhs);
}

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_TYPE_VALUE_H_ */
