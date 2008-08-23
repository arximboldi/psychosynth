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

#include "gui3d/ElementManager.h"
#include "gui3d/ElementTypes.h"
#include "gui3d/TheElements.h"
#include "gui3d/QueryFlags.h"

#include <libpsynth/common/logger.h>

#include <libpsynth/node/node_types.h>

using namespace std;
using namespace Ogre;
using namespace psynth;

ElementManager::ElementManager(Table* table, Ogre::SceneManager* scene,
			       Ogre::Camera* camera) :
    m_table(table),
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

ElementManager::~ElementManager()
{
    for (ElemMapIter it = m_elems.begin(); it != m_elems.end(); ++it)
	delete it->second;

    for (list<Connection*>::iterator it = m_cons.begin(); it != m_cons.end(); ++it)
	delete *it;

    delete m_rayquery;
}

Element* ElementManager::createElement(TableObject& obj)
{
    switch (obj.getType()) {
    case NODE_OSCILLATOR:
	return new ElementOscillator(obj, m_scene);
    case NODE_LFO:
	return new ElementLFO(obj, m_scene);
    case NODE_FILTER:
	return new ElementFilter(obj, m_scene);
    case NODE_MIXER:
	return new ElementMixer(obj, m_scene);
    case NODE_CONTROLMIXER:
	return new ElementControlMixer(obj, m_scene);
    case NODE_SAMPLER:
	return new ElementSampler(obj, m_scene);
    case NODE_STEPSEQ:
	return new ElementStepSeq(obj, m_scene);
    case NODE_AUDIONOISE:
	return new ElementAudioNoise(obj, m_scene);
    case NODE_CONTROLNOISE:
	return new ElementControlNoise(obj, m_scene);
    case NODE_ECHO:
	return new ElementEcho(obj, m_scene);
    case NODE_DELAY:
	return new ElementDelay(obj, m_scene);
    default:
	return NULL;
    }
}

void ElementManager::addElement(psynth::TableObjectCreator& creator)
{
    m_must_own++;
    if (!m_adding.isNull())
	m_adding.deleteMe();
    m_adding = creator.create(*m_table);
    m_must_own--;
}

/*
void ElementManager::addElement(int e_type)
{
    TableObject obj;

    m_must_own++;
    
    switch(e_type) {
    case ELEM_OSC_SINE:
	obj = m_table->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SINE);
	break;
	
    case ELEM_OSC_SQUARE:
	obj = m_table->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SQUARE);
	break;
	
    case ELEM_OSC_TRIANGLE:
	obj = m_table->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_TRIANGLE);
	break;
	
    case ELEM_OSC_SAWTOOTH:
	obj = m_table->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SAWTOOTH);
	break;

    case ELEM_OSC_MOOGSAW:
	obj = m_table->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_MOOGSAW);
	break;

    case ELEM_OSC_EXP:
	obj = m_table->addObject(NODE_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_EXP);
	break;
	
    case ELEM_LFO_SINE:
	obj = m_table->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SINE);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_SQUARE:
	obj = m_table->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SQUARE);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_TRIANGLE:
	obj = m_table->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_TRIANGLE);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_SAWTOOTH:
	obj = m_table->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_SAWTOOTH);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;

    case ELEM_LFO_MOOGSAW:
	obj = m_table->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_MOOGSAW);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_LFO_EXP:
	obj = m_table->addObject(NODE_LFO);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		     ObjectOscillator::OSC_EXP);
	obj.setParam(ObjectOscillator::PARAM_FREQUENCY,
		     2.0f);
	break;
	
    case ELEM_FILTER_LOWPASS:
	obj = m_table->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_LOWPASS);
	break;

    case ELEM_FILTER_HIGHPASS:
	obj = m_table->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_HIGHPASS);
	break;

    case ELEM_FILTER_BANDPASS_CSG:
	obj = m_table->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_BANDPASS_CSG);
	break;

    case ELEM_FILTER_BANDPASS_CZPG:
	obj = m_table->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_BANDPASS_CZPG);
	break;

    case ELEM_FILTER_NOTCH:
	obj = m_table->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_NOTCH);
	break;

    case ELEM_FILTER_MOOG:
	obj = m_table->addObject(NODE_FILTER);
	obj.setParam(ObjectFilter::PARAM_TYPE,
		     ObjectFilter::FILTER_MOOG);
	break;
	
    case ELEM_MIXER:
	obj = m_table->addObject(NODE_MIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_SUM);
	break;

    case ELEM_RINGMOD:
	obj = m_table->addObject(NODE_MIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_PRODUCT);
	break;

    case ELEM_CTRLMIXER:
	obj = m_table->addObject(NODE_CONTROLMIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_SUM);
	break;

    case ELEM_CTRLRINGMOD:
	obj = m_table->addObject(NODE_CONTROLMIXER);
	obj.setParam(ObjectMixer::PARAM_MIXOP,
		     ObjectMixer::MIX_PRODUCT);
	break;
	
    default:
	break;
    }

    m_must_own--;
}
*/

bool ElementManager::getTablePointer(Vector2& res)
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

bool ElementManager::mouseMoved(const OIS::MouseEvent& e)
{
    Vector2 pos;
    bool ret = false;
    
    if (getTablePointer(pos)) {
	for (ElemMapIter it = m_elems.begin(); it != m_elems.end();)
	    if ((*it++).second->pointerMoved(pos))
		ret = true;
    }

    return ret;
}

bool ElementManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector2 pos;
    bool ret = false;

    if (!m_adding.isNull())
	m_adding = TableObject();
    
    if (getTablePointer(pos)) {
	for (ElemMapIter it = m_elems.begin(); it != m_elems.end(); ++it)
	    if ((*it).second->pointerClicked(pos, id))
		ret = true;

	if (ret)
	    return ret;
	
	for (list<Connection*>::iterator it = m_cons.begin(); it != m_cons.end(); ++it)
	    if ((*it)->pointerClicked(pos, id))
		return true;//ret = true;
    }
	
    return ret;
}

bool ElementManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector2 pos;
    bool ret = false;
    
    if (getTablePointer(pos)) {
	for (ElemMapIter it = m_elems.begin(); it != m_elems.end();)
	    if((*it++).second->pointerReleased(pos, id))
		ret = true;
    }
    
    return ret;
}

bool ElementManager::keyPressed(const OIS::KeyEvent &e)
{
    bool ret = false;
    
    for (ElemMapIter it = m_elems.begin(); it != m_elems.end();)
	if((*it++).second->keyPressed(e))
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

bool ElementManager::keyReleased(const OIS::KeyEvent &e)
{
    bool ret = false;
    
    for (ElemMapIter it = m_elems.begin(); it != m_elems.end();)
	if((*it++).second->keyReleased(e))
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

void ElementManager::handleAddObject(TableObject& obj)
{
    Element* elem = createElement(obj);

    if (elem != NULL) {
	elem->setFirstModifier(m_modifier_1);
	elem->setSecondModifier(m_modifier_2);
	
	if (m_must_own)
	    elem->setOwned(true);
	
	m_elems.insert(pair<int,Element*>(obj.getID(), elem));
	obj.addListener(elem);
    }
    else
	WARNING("Could not create element.");
}

void ElementManager::handleDeleteObject(TableObject& obj)
{
    for (ElemMapIter it = m_elems.begin(); it != m_elems.end(); )
	if ((*it).second->getObject() == obj) {
	    m_clear_elems.push_back((*it).second);
	    m_elems.erase(it++);
	} else
	    ++it;
}

void ElementManager::update()
{
    for (ElemIter it = m_clear_elems.begin(); it != m_clear_elems.end(); ++it)
	delete *it;
    
    for (list<Connection*>::iterator it = m_cons.begin(); it != m_cons.end(); ++it)
	(*it)->update();
    
    m_clear_elems.clear();
}

void ElementManager::handleLinkAdded(const TablePatcherEvent& ev)
{
    m_cons.push_back(new Connection(m_scene, ev));

    ElemMapIter it = m_elems.find(ev.src.getID());
    if (it != m_elems.end())
	it->second->setTarget(ev.dest);
}

void ElementManager::handleLinkDeleted(const TablePatcherEvent& ev)
{
    /* TODO */
    for (list<Connection*>::iterator it = m_cons.begin(); it != m_cons.end();)
	if ((*it)->getSource() == ev.src && (*it)->getDestiny() == ev.dest) {
	    delete *it;
	    m_cons.erase(it++);
	} else
	    ++it;

    ElemMapIter it = m_elems.find(ev.src.getID());
    if (it != m_elems.end())
	it->second->clearTarget(ev.dest);
}
