/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007, 2016 Juan Pedro Bolivar Puente                    *
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

#include <psynth/graph/watch_viewer.hpp>

#include "gui3d/connection.hpp"

using namespace Ogre;
using namespace std;
using namespace psynth;
using namespace psynth::base;

const ColourValue WAVE_NORMAL_C_COLOUR = ColourValue(7.0, 0.7, 0.0, 0.6);
const ColourValue WAVE_NORMAL_A_COLOUR = ColourValue(0.0, 0.7, 0.0, 0.6);
const ColourValue WAVE_MUTE_COLOUR = ColourValue(0.0, 0.0, 0.0, 0.7);

const ColourValue LINE_NORMAL_COLOUR = ColourValue(0.7, 0.0, 0.0, 0.7);
const ColourValue LINE_MUTE_COLOUR = ColourValue(0.0, 0.0, 0.0, 0.7);

#define LINE_CLICK_WIDTH 0.3f
#define LINE_WIDTH       0.03f
#define LINE_Y           0.005f

#define WAVE_POINTS  1000
#define WAVE_LENGTH  20.0f
#define WAVE_A_SECS  0.05f
#define WAVE_C_SECS  1.0f
#define WAVE_WIDTH   1.0f
#define WAVE_Y       0.001f

/*
 * TODO: Optimizations!!
 */

void connection_line::build (const Ogre::Vector2& src,
			     const Ogre::Vector2& dst)
{
    Vector2 side =  (src - dst).perpendicular().normalisedCopy() * LINE_WIDTH;
    Vector2 top_left  = src + side;
    Vector2 top_right = src - side;
    Vector2 bot_left  = dst + side;
    Vector2 bot_right = dst - side;

    position(top_left.x, LINE_Y, top_left.y);
    position(top_right.x, LINE_Y, top_right.y);
    position(bot_left.x, LINE_Y, bot_left.y);
    position(bot_right.x, LINE_Y, bot_right.y);
}

connection_line::connection_line (const std::string& id,
				  const Ogre::ColourValue& colour,
				  const Vector2& src,
				  const Vector2& dest) :
    ManualObject (id)
{
    setDynamic (true);
    create_colour_material (id, colour);
    begin (getName(), RenderOperation::OT_TRIANGLE_STRIP);
    build (src, dest);
    end ();
}

void connection_line::update (const Ogre::Vector2& src,
			     const Ogre::Vector2& dest)
{
    beginUpdate(0);
    build(src, dest);
    end();
}

connection_line::~connection_line()
{
    Ogre::MaterialManager::getSingleton().remove(getName());
}


void connection_wave::build (const Ogre::Vector2& src,
			    const Ogre::Vector2& dst,
			    const sample* samples, int n_samples)
{
    if (samples) {
	int i;
	Real length = (src - dst).length();
	int real_samples =
	    length >= WAVE_LENGTH ?
	    n_samples : length / ((float) WAVE_LENGTH / n_samples);
	Real delta = length / (float) real_samples;
	Vector2 run = (dst - src).normalisedCopy() * delta;
	Vector2 base = src;
	Vector2 side = (dst - src).perpendicular().normalisedCopy() * WAVE_WIDTH;
	Vector2 tmp;

	i = n_samples - 1;
	while(i >= n_samples - real_samples) {
	    if (i !=  n_samples - 1 || samples[i] >= 0)
		position(base.x, WAVE_Y, base.y);
	    tmp = base + side * samples[i] * (sin((n_samples - i) * M_PI / real_samples) * .7 + 0.3);
	    position(tmp.x, WAVE_Y, tmp.y);

	    for (--i; i >= n_samples - real_samples &&
		     (samples[i+1] >= 0) == (samples[i] >= 0);
		 --i) {
		base += run;
		position(base.x, WAVE_Y, base.y);
		tmp = base + side * samples[i] * (sin((n_samples - i) * M_PI / real_samples) * 0.7 + 0.3);
		position(tmp.x, WAVE_Y, tmp.y);
	    }

	    position(base.x, WAVE_Y, base.y);
	    base += run;
	}
    } else {
	position(0,0,0);
    }
}

connection_wave::connection_wave (const std::string& id,
				  const Ogre::ColourValue& colour,
				  const Vector2& src,
				  const Vector2& dest) :
    ManualObject(id)
{
    setDynamic(true);
    create_colour_material (id, colour);
    begin (getName(), RenderOperation::OT_TRIANGLE_STRIP);
    build (src, dest, 0, 0);
    end ();
}

void connection_wave::update (const Ogre::Vector2& src,
			     const Ogre::Vector2& dest,
			     const sample* samples, int n_samples)
{
    estimateVertexCount(2 * n_samples);
    beginUpdate(0);
    build(src, dest, samples, n_samples);
    end();
}

connection_wave::~connection_wave ()
{
    Ogre::MaterialManager::getSingleton().remove(getName());
}

connection::connection (Ogre::SceneManager* scene,
			const world_patcher_event& ev) :
    m_scene(scene),
    m_link(ev)
{
    vector_2f v;

    ev.src.get_param (graph::node0::PARAM_MUTE, m_is_muted);

    ev.src.get_param (graph::node0::PARAM_POSITION, v);
    m_src.x = v.x;
    m_src.y = v.y;

    ev.dest.get_param (graph::node0::PARAM_POSITION, v);
    m_dest.x = v.x;
    m_dest.y = v.y;

    m_node = m_scene->getRootSceneNode()->createChildSceneNode();
    m_line = new connection_line (m_node->getName() + "line",
				  m_is_muted ?
				  LINE_MUTE_COLOUR :
				  LINE_NORMAL_COLOUR,
				  m_src, m_dest);

    m_wave = new connection_wave (m_node->getName() + "wave",
				  m_is_muted ?
				  WAVE_MUTE_COLOUR :
				  ev.socket_type == graph::node0::LINK_AUDIO ?
				  WAVE_NORMAL_A_COLOUR :
				  WAVE_NORMAL_C_COLOUR,
				  m_src, m_dest);

    m_node->attachObject(m_line);
    m_node->attachObject(m_wave);

    m_link.src.add_listener (this);
    m_link.dest.add_listener (this);

    if (ev.socket_type == graph::node0::LINK_AUDIO)
	m_watch = new graph::watch_view_audio (WAVE_POINTS,
                                               WAVE_A_SECS);
    else
	m_watch = new graph::watch_view_control (WAVE_POINTS,
                                                 WAVE_C_SECS);
    m_link.dest.attach_watch (ev.socket_type, ev.dest_socket, m_watch);
}

connection::~connection ()
{
    m_link.dest.detach_watch (m_link.socket_type, m_link.dest_socket, m_watch);
    delete m_watch;

    m_scene->destroySceneNode(m_node->getName());

    m_link.src.delete_listener (this);
    m_link.dest.delete_listener (this);

    delete m_line;
    delete m_wave;
}

void connection::handle_set_param_node (world_node& obj, int id)
{
    if (id == graph::node0::PARAM_POSITION) {
	vector_2f pos;
	obj.get_param (id, pos);

	if (obj.get_id() == m_link.src.get_id()) {
	    m_src.x = pos.x;
	    m_src.y = pos.y;
	} else if (obj.get_id() == m_link.dest.get_id()) {
	    m_dest.x = pos.x;
	    m_dest.y = pos.y;
	}

	m_line->update(m_src, m_dest);
    }

    if (id == graph::node0::PARAM_MUTE
	&& obj == m_link.src) {

	obj.get_param (id, m_is_muted);
	if (m_is_muted == true) {
	    m_line->set_colour (LINE_MUTE_COLOUR);
	    m_wave->set_colour (WAVE_MUTE_COLOUR);
	} else {
	    m_line->set_colour (LINE_NORMAL_COLOUR);
	    m_wave->set_colour (m_link.socket_type == graph::node0::LINK_AUDIO ?
				WAVE_NORMAL_A_COLOUR :
				WAVE_NORMAL_C_COLOUR);
	}

	m_line->update(m_src, m_dest);
    }
}

bool connection::pointer_clicked (const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    Vector2 side =  (m_src - m_dest).perpendicular().normalisedCopy() * LINE_CLICK_WIDTH;
    Vector2 top_left  = m_src + side;
    Vector2 top_right = m_src - side;
    Vector2 bot_left  = m_dest + side;
    Vector2 bot_right = m_dest - side;

    if (point_is_in_poly (pos, top_left, top_right, bot_right, bot_left)) {
	m_is_muted = !m_is_muted;
	m_link.src.set_param (graph::node0::PARAM_MUTE, m_is_muted);

	return true;
    }

    return false;
}

void connection::update ()
{
    const sample* buf;

    if (m_link.socket_type == graph::node0::LINK_AUDIO)
	buf = (const sample*)
            &const_range (dynamic_cast<graph::watch_view_audio*>(
                              m_watch)->get_buffer()) [0][0];
    else
	buf = (const sample*)
            &const_range (dynamic_cast<graph::watch_view_control*>(
                              m_watch)->get_buffer()) [0];

    //m_line->update(m_src, m_dest);
    m_wave->update(m_src, m_dest, buf, WAVE_POINTS);
}

/*
  void connection::updateSource(const Vector2& pos)
  {
  m_src = pos;
  m_line->update(m_src, m_dest);
  }

  void connection::updateDestiny(const Vector2& pos)
  {
  m_dest = pos;
  m_line->update(m_src, m_dest);
  }

  void connection::update(const Vector2& src, const Vector2& dest)
  {
  m_src = src;
  m_dest = dest;
  m_line->update(m_src, m_dest);
  }
*/
