/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef PSYNTH_PATCHERDYNAMIC_H
#define PSYNTH_PATCHERDYNAMIC_H

#include <map>
#include <set>

#include <psynth/world/patcher.hpp>

namespace psynth
{

class patcher_dynamic : public patcher
{
    struct link {
	graph::node0* src;
	graph::node0* dest;
	float dist;
	float dist_to_center;
	int sock_type;
	int out_sock;
	int in_sock;
	int actual_in_sock;

	link (graph::node0* s, graph::node0* d, float ds, float dc, int t, int os, int is) :
	    src(s), dest(d), dist(ds), dist_to_center(dc),
	    sock_type(t), out_sock(os), in_sock(is), actual_in_sock(-1)
	    {}

	bool operator< (const link& l) const {
	    return dist == l.dist ? dist_to_center < l.dist_to_center : dist < l.dist;
	}
    };

    struct pnode {
	graph::node0* obj;
	graph::node0* dest;
	bool out_used; /* We output to one object only */
	int actual_sock_type;
	int actual_in_sock;

	pnode (graph::node0* o = NULL) :
	    obj(o),
	    dest(NULL),
	    out_used(false),
	    actual_sock_type(-1),
	    actual_in_sock(-1) {}
    };

    class link_ptr_cmp {
    public:
	bool operator () (const link* a, const link* b) {
	  return *a < *b;
	}
    };

    bool m_changed;

    std::map<int, pnode> m_nodes;
    std::multiset<link*, link_ptr_cmp> m_links;

    inline void undo_link (link& l);
    inline void make_link (link& l);
    inline void find_in_sock (link& l);
    inline bool is_linked (link& l);

public:
    patcher_dynamic ();
    ~patcher_dynamic ();

    bool add_node (graph::node0* obj);
    bool delete_node (graph::node0* obj);
    void set_param_node (graph::node0* obj, int id);
    void update ();
    void clear ();
};

} /* namespace psynth */

#endif /* PSYNTH_PATCHERDYNAMIC_H */
