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

#ifndef PSYNTH_GRAPH_WATCH_H
#define PSYNTH_GRAPH_WATCH_H

#include <psynth/synth/audio_info.hpp>
#include <psynth/graph/buffers.hpp>

namespace psynth
{
namespace graph
{

class watch
{
public:
    virtual void set_info (const audio_info& info) {}
    virtual void update (const sample_const_range& buf) {}
    virtual void update (const audio_const_range& buf) {}
};

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_WATCH_H */
