/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef OSCMISC_H
#define OSCMISC_H

#include <lo/lo.h>

#define LO_HANDLER_ARGS const char* path, const char* types,\
	lo_arg** argv, int argc, lo_message msg

#define LO_HANDLER(clax,name)			\
int _##name##_cb (LO_HANDLER_ARGS);\
inline static int name##_cb(LO_HANDLER_ARGS, void* osc_listener)\
{ return ((clax*)osc_listener)->_##name##_cb(path, types, argv, argc, msg); }


inline bool lo_address_equals(lo_address a, lo_address b)
{
    return strcmp(lo_address_get_hostname(a), lo_address_get_hostname(b)) == 0
	&& strcmp(lo_address_get_port(a), lo_address_get_port(b)) == 0
	;// FIXME: && lo_address_get_protocol(a) == lo_address_get_protocol(b);
}

#endif /* OSCMISC_H */
