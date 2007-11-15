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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

#include "gui3d/FlatRing.h"
#include "table/Table.h"

class Element;

class ElemComponent
{
    Element* m_parent;
    Ogre::SceneNode* m_node;
    
public:    
    virtual ~ElemComponent() {};
    virtual void init() = 0;
    virtual void handleParamChange(TableObject& obj, int param_id) = 0;
    virtual bool handlePointerMove(Ogre::Vector2 pos) = 0;
    virtual bool handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id) = 0;
    virtual bool handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id) = 0;
    
    void setSceneNode(Ogre::SceneNode* node) {
	m_node = node;
    }
    
    Ogre::SceneNode* getSceneNode() {
	return m_node;
    }

    void setParent(Element* parent) {
	m_parent = parent;
    }
    
    Element* getParent() {
	return m_parent;
    };
};

class ElemMainComponent : public ElemComponent
{
    std::string m_mesh;
    Ogre::Entity* m_mesh_ent;
    Ogre::Vector2 m_last_mouse_pos;
    int m_param;
    float m_min_val;
    float m_max_val;
    float m_old_value;
    bool m_rotating;
    
public:
    ElemMainComponent(const std::string& mesh,
		      int param, float min_val, float max_val);
    void setMesh(const std::string& mesh);

    bool handlePointerMove(Ogre::Vector2 pos);
    bool handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id);
    bool handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id);
    void init();
    virtual void handleParamChange(TableObject& obj, int param_id);
};

class ElemMultiMainComponent : public ElemMainComponent
{
    int m_param;
    const char** m_names;
public:
    ElemMultiMainComponent(int param_1, float min_val, float max_val,
			   int param_2, const char** names);
    
    void handleParamChange(TableObject& obj, int param_id);
};

class Element : public TableObjectListener
{
    typedef std::list<ElemComponent*>::iterator ElemComponentIter;
    std::list<ElemComponent*> m_comp;
        
    TableObject m_obj;

    Ogre::ColourValue   m_col_ghost;
    Ogre::ColourValue   m_col_selected;
    Ogre::ColourValue   m_col_normal;
    Ogre::SceneManager* m_scene;
    FlatRing*           m_base;
    Ogre::SceneNode*    m_node;

    Ogre::Vector2 m_click_diff;
    Ogre::Vector2 m_pos;
    Ogre::Degree m_angle;
	
    bool m_ghost;
    bool m_selected;
    bool m_moving;
    
public:
    static const Real RADIOUS = 1.0f;

    Element(const TableObject& obj, Ogre::SceneManager* scene);
    
    virtual ~Element();

    void addComponent(ElemComponent* comp);
    
    void setGhost(bool ghost);
    void setSelected(bool selected);
    void setPosition(const Ogre::Vector2& pos);
    
    bool pointerClicked(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointerReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointerMoved(const Ogre::Vector2& pos);
    bool keyPressed(const OIS::KeyEvent& e);
    bool keyReleased(const OIS::KeyEvent& e);
    
    void handleMoveObject(TableObject& obj);
    void handleActivateObject(TableObject& obj);
    void handleDeactivateObject(TableObject& obj);
    void handleSetParamObject(TableObject& ob, int param_id);
    
    bool isGhost() const {
	return m_ghost;
    };
	
    bool isSelected() const {
	return m_selected;
    };
	
    Ogre::Vector2 getPosition() {
	return m_pos;
    }

    Ogre::SceneManager* getScene() {
	return m_scene;
    }

    TableObject& getObject() {
	return m_obj;
    }
};

class ElementOscillator : public Element
{
public:
    ElementOscillator(const TableObject& obj, Ogre::SceneManager* m_scene);
};

class ElementMixer : public Element
{
public:
    ElementMixer(const TableObject& obj, Ogre::SceneManager* m_scene);
};

#endif /* ELEMENT_H */
