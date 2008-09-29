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

#include <algorithm>
#include <CEGUI/CEGUI.h>

#include "gui3d/element_manager.hpp"
#include "gui3d/element_types.hpp"
#include "gui3d/the_elements.hpp"
#include "gui3d/query_flags.hpp"

#include <psynth/common/logger.hpp>

#include <psynth/node/node_types.hpp>

using namespace std;
using namespace Ogre;
using namespace psynth;

element_manager::element_manager(world* world, Ogre::SceneManager* scene,
			       Ogre::Camera* camera) :
    m_world(world),
    m_elems(),
    m_elemcount(0),
    m_must_own(0),
    m_modifier_1(0),
    m_modifier_2(0),
    m_camera(camera),
    m_scene(scene),
    m_rayquery(scene->createRayQuery(Ogre::Ray()))
{
}

element_manager::~element_manager()
{
    for (elem_map_iter it = m_elems.begin(); it != m_elems.end(); ++it)
	delete it->second;

    for (list<connection*>::iterator it = m_cons.begin(); it != m_cons.end(); ++it)
	delete *it;

    delete m_rayquery;
}

element* element_manager::create_element (world_node& obj)
{
    switch (obj.get_type ()) {
    case NODE_OSCILLATOR:
	return new element_oscillator (obj, m_scene);
    case NODE_LFO:
	return new element_lfo (obj, m_scene);
    case NODE_FILTER:
	return new element_filter (obj, m_scene);
    case NODE_MIXER:
	return new element_mixer (obj, m_scene);
    case NODE_CONTROLMIXER:
	return new element_control_mixer (obj, m_scene);
    case NODE_SAMPLER:
	return new element_sampler (obj, m_scene);
    case NODE_STEPSEQ:
	return new element_step_seq (obj, m_scene);
    case NODE_AUDIONOISE:
	return new element_audio_noise (obj, m_scene);
    case NODE_CONTROLNOISE:
	return new element_control_noise (obj, m_scene);
    case NODE_ECHO:
	return new element_echo (obj, m_scene);
    case NODE_DELAY:
	return new element_delay (obj, m_scene);
    default:
	return NULL;
    }
}

void element_manager::add_element (psynth::world_node_creator& creator)
{
    m_must_own++;
    if (!m_adding.is_null ())
	m_adding.delete_me ();
    m_adding = creator.create(*m_world);
    m_must_own--;
}

/*
void element_manager::addElement(int e_type)
{
    world_node obj;

    m_must_own++;
    
    switch(e_type) {
    case ELEM_OSC_SINE:
	obj = m_world->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SINE);
	break;
	
    case ELEM_OSC_SQUARE:
	obj = m_world->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SQUARE);
	break;
	
    case ELEM_OSC_TRIANGLE:
	obj = m_world->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_TRIANGLE);
	break;
	
    case ELEM_OSC_SAWTOOTH:
	obj = m_world->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SAWTOOTH);
	break;

    case ELEM_OSC_MOOGSAW:
	obj = m_world->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_MOOGSAW);
	break;

    case ELEM_OSC_EXP:
	obj = m_world->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_EXP);
	break;
	
    case ELEM_LFO_SINE:
	obj = m_world->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SINE);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_SQUARE:
	obj = m_world->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SQUARE);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_TRIANGLE:
	obj = m_world->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_TRIANGLE);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_SAWTOOTH:
	obj = m_world->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SAWTOOTH);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;

    case ELEM_LFO_MOOGSAW:
	obj = m_world->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_MOOGSAW);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_EXP:
	obj = m_world->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_EXP);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_FILTER_LOWPASS:
	obj = m_world->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_LOWPASS);
	break;

    case ELEM_FILTER_HIGHPASS:
	obj = m_world->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_HIGHPASS);
	break;

    case ELEM_FILTER_BANDPASS_CSG:
	obj = m_world->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_BANDPASS_CSG);
	break;

    case ELEM_FILTER_BANDPASS_CZPG:
	obj = m_world->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_BANDPASS_CZPG);
	break;

    case ELEM_FILTER_NOTCH:
	obj = m_world->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_NOTCH);
	break;

    case ELEM_FILTER_MOOG:
	obj = m_world->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_MOOG);
	break;
	
    case ELEM_MIXER:
	obj = m_world->addObject(NODE_MIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_SUM);
	break;

    case ELEM_RINGMOD:
	obj = m_world->addObject(NODE_MIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_PRODUCT);
	break;

    case ELEM_CTRLMIXER:
	obj = m_world->addObject(NODE_CONTROLMIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_SUM);
	break;

    case ELEM_CTRLRINGMOD:
	obj = m_world->addObject(NODE_CONTROLMIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_PRODUCT);
	break;
	
    default:
	break;
    }

    m_must_own--;
}
*/

bool element_manager::get_world_pointer (Vector2& res)
{
    CEGUI::Point mousepos = CEGUI::MouseCursor::getSingleton().getPosition();	
    Ray ray =  Ray(m_camera->getCameraToViewportRay(
		       mousepos.d_x/m_camera->getViewport()->getActualWidth(),
		       mousepos.d_y/m_camera->getViewport()->getActualHeight()));
    pair<bool, Ogre::Real>  inter = ray.intersects(Plane(Vector3(0.0,1.0,0.0),
							 Ogre::Real(0.0)));

    if (inter.first) {
	Vector3 interpt = ray.getPoint(inter.second);

	res.x = interpt.x;
	res.y = interpt.z;

	return true;
    }

    return false; 
}

bool element_manager::mouseMoved(const OIS::MouseEvent& e)
{
    Vector2 pos;
    bool ret = false;
    
    if (get_world_pointer (pos)) {
	for (elem_map_iter it = m_elems.begin(); it != m_elems.end();)
	    if ((*it++).second->pointer_moved (pos))
		ret = true;
    }

    return ret;
}

bool element_manager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector2 pos;
    bool ret = false;

    if (!m_adding.is_null())
	m_adding = world_node();
    
    if (get_world_pointer (pos)) {
	for (elem_map_iter it = m_elems.begin(); it != m_elems.end(); ++it)
	    if ((*it).second->pointer_clicked (pos, id))
		ret = true;

	if (ret)
	    return ret;
	
	for (list<connection*>::iterator it = m_cons.begin(); it != m_cons.end(); ++it)
	    if ((*it)->pointer_clicked (pos, id))
		return true;//ret = true;
    }
	
    return ret;
}

bool element_manager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector2 pos;
    bool ret = false;
    
    if (get_world_pointer (pos)) {
	for (elem_map_iter it = m_elems.begin(); it != m_elems.end();)
	    if((*it++).second->pointer_released (pos, id))
		ret = true;
    }
    
    return ret;
}

bool element_manager::keyPressed(const OIS::KeyEvent &e)
{
    bool ret = false;
    
    for (elem_map_iter it = m_elems.begin(); it != m_elems.end();)
	if((*it++).second->key_pressed (e))
	    ret = true;    

    switch(e.key) {
    case OIS::KC_LCONTROL:
    case OIS::KC_RCONTROL:
	//m_modifier_1++;
	m_modifier_1 = 1;
	break;

    case OIS::KC_LSHIFT:
    case OIS::KC_RSHIFT:
	//m_modifier_2++;
	m_modifier_2 = 1;
	break;
	
    default:
	break;
    }
           
    return ret;
}

bool element_manager::keyReleased(const OIS::KeyEvent &e)
{
    bool ret = false;
    
    for (elem_map_iter it = m_elems.begin(); it != m_elems.end();)
	if((*it++).second->key_released (e))
	    ret = true;    

    switch(e.key) {
    case OIS::KC_LCONTROL:
    case OIS::KC_RCONTROL:
	//m_modifier_1--;
	m_modifier_1 = 0;
	break;

    case OIS::KC_LSHIFT:
    case OIS::KC_RSHIFT:
	//m_modifier_2--;
	m_modifier_2 = 0;
	break;
	
    default:
	break;
    }
    
    return ret;
}

void element_manager::handle_add_node (world_node& obj)
{
    element* elem = create_element (obj);

    if (elem != NULL) {
	elem->set_first_modifier (m_modifier_1);
	elem->set_second_modifier (m_modifier_2);
	
	if (m_must_own)
	    elem->set_owned (true);
	
	m_elems.insert(pair<int, element*>(obj.get_id(), elem));
	obj.add_listener(elem);
    }
    else
	logger::instance () ("gui3d", log::WARNING, "Could not create element.");
}

void element_manager::handle_delete_node (world_node& obj)
{
    for (elem_map_iter it = m_elems.begin(); it != m_elems.end(); )
	if (it->second->get_node() == obj) {
	    m_clear_elems.push_back((*it).second);
	    m_elems.erase(it++);
	} else
	    ++it;
}

void element_manager::update()
{
    for (elem_iter it = m_clear_elems.begin(); it != m_clear_elems.end(); ++it)
	delete *it;
    
    for (list<connection*>::iterator it = m_cons.begin(); it != m_cons.end(); ++it)
	(*it)->update();
    
    m_clear_elems.clear();
}

void element_manager::handle_link_added (const world_patcher_event& ev)
{
    m_cons.push_back(new connection(m_scene, ev));

    elem_map_iter it = m_elems.find(ev.src.get_id());
    if (it != m_elems.end())
	it->second->set_target (ev.dest);
}

void element_manager::handle_link_deleted (const world_patcher_event& ev)
{
    /* TODO */
    for (list<connection*>::iterator it = m_cons.begin(); it != m_cons.end();)
	if ((*it)->get_source () == ev.src &&
	    (*it)->get_destiny () == ev.dest) {
	    delete *it;
	    m_cons.erase(it++);
	} else
	    ++it;

    elem_map_iter it = m_elems.find(ev.src.get_id());
    if (it != m_elems.end())
	it->second->clear_target (ev.dest);
}
