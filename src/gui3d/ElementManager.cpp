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
#include "gui3d/QueryFlags.h"
#include "common/Error.h"

using namespace std;
using namespace Ogre;

void ElementManager::stopAdding()
{
	if (isAdding()) {
		delete (*m_addelem).second;

		m_elems.erase(m_addelem);
		m_addelem = m_elems.end();
	}
}

void ElementManager::add()
{
	(*m_addelem).second->setGhost(false);
	m_addelem = m_elems.end();
}

void ElementManager::select(const std::string& name)
{
	unselect();
	m_selected = m_elems.find(name);
	if (m_selected != m_elems.end())
		(*m_selected).second->setSelected(true);
	else
		WARNING("Selected element not found in elements map.");
}

void ElementManager::unselect()
{
	if (m_selected != m_elems.end()) {
		(*m_selected).second->setSelected(false);
		m_selected = m_elems.end();
	}
}

void ElementManager::move(Element* elem)
{
	CEGUI::Point mousepos = CEGUI::MouseCursor::getSingleton().getPosition();	
	Ray ray =  Ray( m_camera->getCameraToViewportRay(
			mousepos.d_x/m_camera->getViewport()->getActualWidth(),
			mousepos.d_y/m_camera->getViewport()->getActualHeight() ) );
	pair<bool, Ogre::Real>  inter = ray.intersects(Plane(Vector3(0.0,1.0,0.0), Ogre::Real(0.0)));
	
	if (inter.first) {
		Vector3 interpt = ray.getPoint(inter.second);
	
		elem->setPosition(Vector2(interpt.x, interpt.z));
	}
}

ElementManager::ElementManager(Table* table, Ogre::SceneManager* scene, Ogre::Camera* camera) :
	m_table(table),
	m_elems(),
	m_addelem(m_elems.end()),
	m_selected(m_elems.end()),
	m_elemcount(0),
	m_camera(camera),
	m_scene(scene),
	m_rayquery(scene->createRayQuery(Ogre::Ray()))
{
	int i;
	for (i = 0; i < N_MB; i++)
		m_pressed[i] = false;
}

void ElementManager::addElement(int e_type)
{
	unselect();
	stopAdding();
	
	stringstream stm;
	stm << "e" << m_elemcount++;
	
	if (!m_elems[stm.str()]) {
		Element* newelem  = new Element(m_scene, stm.str(), "oscsine.mesh",
			Vector2(0.0, 0.0), Degree(0.0), true);
		m_elems[stm.str()] = newelem;
		move(newelem);
		m_addelem = m_elems.find(stm.str());
	} else {
		WARNING("Repeated id!\n");
	}
}

bool ElementManager::mouseMoved(const OIS::MouseEvent& e)
{
	if (isAdding()) {
		if (m_pressed[OIS::MB_Right])
			(*m_addelem).second->rotate(Degree(e.state.Y.rel * 0.2));
		else
			move((*m_addelem).second);
		return true;
	}
	
	if (isSelected()) {
		if (m_pressed[OIS::MB_Left]) {
			move((*m_selected).second);
			return true;
		}
		else if (m_pressed[OIS::MB_Right]) {
			(*m_selected).second->rotate(Degree(e.state.Y.rel * 0.5));
			return true;
		}
	}
	
	return false;
}

bool ElementManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	m_pressed[id] = true;
	
	if (id == OIS::MB_Left) {
		if (isAdding()) {
			add();
			return true;
		}
		
		CEGUI::Point mousepos = CEGUI::MouseCursor::getSingleton().getPosition();	
		Ray mouseray =  Ray( m_camera->getCameraToViewportRay(
				mousepos.d_x/m_camera->getViewport()->getActualWidth(),
				mousepos.d_y/m_camera->getViewport()->getActualHeight() ) );
		
		m_rayquery->setRay(mouseray);
		m_rayquery->setSortByDistance(true);
		m_rayquery->setQueryMask(QFLAG_ELEMENT);
		
		RaySceneQueryResult &result = m_rayquery->execute();
		
		if (result.begin() != result.end()) {
			if (result.begin()->movable) {
				stopAdding();
				select(result.begin()->movable->getParentSceneNode()->getName());
				return true;
			}
		}
		
		unselect();
	}
	
	return false;
}

bool ElementManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	m_pressed[id] = false;
	
	return false;
}

bool ElementManager::keyPressed(const OIS::KeyEvent &e)
{
	return false;
}

bool ElementManager::keyReleased(const OIS::KeyEvent &e)
{
	return false;
}

