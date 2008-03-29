/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef ELEMTOGGLERCOMPONENT_H
#define ELEMTOGGLERCOMPONENT_H

#include <vector>
#include "gui3d/Element.h"
#include "gui3d/TaskManager.h"

class ElemTogglerComponent : public ElemComponent,
			     public Task
{
    typedef std::pair<FlatRing*, int> Toggle;
    std::vector<Toggle> m_toggles;

    int m_param_num;
    int m_param_first;
    int m_param_step;
    
    int m_step;
    
    void createToggles();
    void destroyToggles();
    void updateCurrentStepColour(int step);
    void updateStepColour(int step);
    
public:
    ElemTogglerComponent(int num_param,
			 int first_param,
			 int step_param);
    ~ElemTogglerComponent();
    
    void init();
    void update(int ms);
    bool handlePointerMove(Ogre::Vector2 pos);
    bool handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id);
    bool handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id);
    void handleParamChange(psynth::TableObject& obj,
			   int param_id);   
};

#endif /* ELEMTOGGLERCOMPONENT_H */

