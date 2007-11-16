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

#include <CEGUI/CEGUI.h>

#include "gui3d/ElementManager.h"
#include "gui3d/ElementTypes.h"
#include "gui3d/QueryFlags.h"
#include "common/Error.h"
#include "object/KnownObjects.h"

using namespace std;
using namespace Ogre;

ElementManager::ElementManager(Table* table, Ogre::SceneManager* scene,
			       Ogre::Camera* camera) :
    m_table(table),
    m_elems(),
    m_elemcount(0),
    m_camera(camera),
    m_scene(scene),
    m_rayquery(scene->createRayQuery(Ogre::Ray()))
{
}

Element* ElementManager::createElement(TableObject& obj)
{
    switch (obj.getType()) {
    case OBJ_OSCILLATOR:
	return new ElementOscillator(obj, m_scene);

    case OBJ_MIXER:
	return new ElementMixer(obj, m_scene);
	
    default:
	return NULL;
    }
}

void ElementManager::addElement(int e_type)
{
    TableObject obj;
    
    switch(e_type) {
    case ELEM_OSC_SINE:
	obj = m_table->addObject(OBJ_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		      ObjectOscillator::OSC_SINE);
	break;
	
    case ELEM_OSC_SQUARE:
	obj = m_table->addObject(OBJ_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		      ObjectOscillator::OSC_SQUARE);
	break;
	
    case ELEM_OSC_TRIANGLE:
	obj = m_table->addObject(OBJ_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		      ObjectOscillator::OSC_TRIANGLE);
	break;
	
    case ELEM_OSC_SAWTOOTH:
	obj = m_table->addObject(OBJ_OSCILLATOR);
	obj.setParam(ObjectOscillator::PARAM_WAVE,
		      ObjectOscillator::OSC_SAWTOOTH);
	break;

    case ELEM_MIXER:
	m_table->addObject(OBJ_MIXER);
	break;
	
    default:
	break;
    }
}

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
	for (ElemIter it = m_elems.begin(); it != m_elems.end();)
	    if ((*it++)->pointerMoved(pos))
		ret = true;
    }

    return ret;
}

bool ElementManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector2 pos;
    bool ret = false;
    
    if (getTablePointer(pos)) {
	for (ElemIter it = m_elems.begin(); it != m_elems.end();)
	    if ((*it++)->pointerClicked(pos, id))
		ret = true;
    }
	
    return ret;
}

bool ElementManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector2 pos;
    bool ret = false;
    
    if (getTablePointer(pos)) {
	for (ElemIter it = m_elems.begin(); it != m_elems.end();)
	    if((*it++)->pointerReleased(pos, id))
		ret = true;
    }
    
    return ret;
}

bool ElementManager::keyPressed(const OIS::KeyEvent &e)
{
    bool ret = false;
    
    for (ElemIter it = m_elems.begin(); it != m_elems.end();)
	if((*it++)->keyPressed(e))
		ret = true;    

    return ret;
}

bool ElementManager::keyReleased(const OIS::KeyEvent &e)
{
    bool ret = false;
    
    for (ElemIter it = m_elems.begin(); it != m_elems.end();)
	if((*it++)->keyReleased(e))
		ret = true;    
    
    return ret;
}

void ElementManager::handleAddObject(TableObject& obj)
{
    Element* elem = createElement(obj);

    if (elem != NULL) {
	m_elems.push_back(elem);
	obj.addListener(elem);
    }
    else
	WARNING("Could not create element.");
}

void ElementManager::handleDeleteObject(TableObject& obj)
{
    for (ElemIter it = m_elems.begin(); it != m_elems.end(); )
	if ((*it)->getObject() == obj) {
	    m_clear_elems.push_back(*it);
	    m_elems.erase(it++);
	} else
	    ++it;
}

void ElementManager::update()
{
    for (ElemIter it = m_clear_elems.begin(); it != m_clear_elems.end(); ++it)
	delete *it;
    m_clear_elems.clear();
}

void ElementManager::handleLinkAdded(const TablePatcherEvent& ev)
{
    cout << "awiniwaun\n";
}

void ElementManager::handleLinkDeleted(const TablePatcherEvent& ev)
{
    cout << "awinipeich\n";
}
