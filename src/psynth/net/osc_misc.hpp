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

#ifndef PSYNTH_OSCMISC_H
#define PSYNTH_OSCMISC_H

#include <cstdio>
#include <lo/lo.h>

namespace psynth
{

#define LO_HANDLER_ARGS const char* path, const char* types,\
	lo_arg** argv, int argc, lo_message msg

#define LO_HANDLER(clax,name)			\
int _##name##_cb (LO_HANDLER_ARGS);\
inline static int name##_cb(LO_HANDLER_ARGS, void* osc_listener)\
{ \
    return ((clax*)osc_listener)->_##name##_cb(path, types, argv, argc, msg);\
}


inline bool lo_address_equals(lo_address a, lo_address b)
{
    return strcmp(lo_address_get_hostname(a), lo_address_get_hostname(b)) == 0
	&& strcmp(lo_address_get_port(a), lo_address_get_port(b)) == 0;
    // FIXME: && lo_address_get_protocol(a) == lo_address_get_protocol(b);
}

inline int lo_address_cmp(lo_address a, lo_address b)
{
    int res;

    res = strcmp(lo_address_get_hostname(a), lo_address_get_hostname(b));
    if (!res) {
	res = strcmp(lo_address_get_port(a), lo_address_get_port(b));
    }

    return res;
}

inline int lo_generic_handler(const char *path, const char *types, lo_arg **argv,
			      int argc, lo_message msg, void *user_data)
{
    int i;

    printf("--- OSC Message ---\n");
    printf("from host: %s\n", lo_address_get_hostname(lo_message_get_source(msg)));
    printf("from port: %s\n", lo_address_get_port(lo_message_get_source(msg)));
    printf("path: <%s>\n", path);

    for (i=0; i<argc; i++) {
	printf("arg %d '%c' ", i, types[i]);
	lo_arg_pp((lo_type)types[i], argv[i]);
	printf("\n");
    }
    
    printf("\n");
    fflush(stdout);

    return 1;
}

} /* namespace psynth */

#endif /* PSYNTH_OSCMISC_H */
