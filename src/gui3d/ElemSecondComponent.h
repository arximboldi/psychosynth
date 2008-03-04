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

#ifndef ELEMSECONDCOMPONENT_H
#define ELEMSECONDCOMPONENT_H

#include "gui3d/Element.h"

class ElemSecondComponent : public ElemComponent
{
    FlatRing* m_indicator;
    FlatRing* m_indicator_point;   
    Ogre::SceneNode* m_point_node;
    Ogre::SceneNode* m_point_yaw;
    Ogre::Degree m_angle;

    int m_param;

    float m_min_val;
    float m_max_val;
    float m_old_value;
    bool m_changing;
    
public:
    ElemSecondComponent(int param,
			float min_val, float max_val);
    ~ElemSecondComponent();
    
    void init();
    bool handlePointerMove(Ogre::Vector2 pos);
    bool handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id);
    bool handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id);
    void handleParamChange(psynth::TableObject& obj,
			   int param_id);   
};

#endif /* ELEMSECONDCOMPONENT_H */

