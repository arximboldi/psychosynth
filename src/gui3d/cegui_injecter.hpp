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

#ifndef CEGUIINJECTER_H
#define CEGUIINJECTER_H

#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>
#include <psynth/base/misc.hpp>

inline CEGUI::MouseButton convert_button (OIS::MouseButtonID buttonID)
{
    switch (buttonID) {
    case OIS::MB_Left:   return CEGUI::LeftButton;
    case OIS::MB_Right:  return CEGUI::RightButton;
    case OIS::MB_Middle: return CEGUI::MiddleButton;
        //case OIS::MB_Button3:
        //case OIS::MB_Button4:
    default:             return CEGUI::LeftButton;
    }
}

class cegui_injecter : public OIS::MouseListener, public OIS::KeyListener
{
public:
    bool mouseMoved(const OIS::MouseEvent& e) {
	auto& ctx = CEGUI::System::getSingleton().getDefaultGUIContext();
	auto result =
            ctx.injectMousePosition(e.state.X.abs, e.state.Y.abs) ||
            ctx.injectMouseWheelChange(e.state.Z.rel * .01);
        return result;
    }

    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {
	auto result = CEGUI::System::getSingleton()
            .getDefaultGUIContext()
            .injectMouseButtonDown (convert_button(id));
        return result;
    }

    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {
	return CEGUI::System::getSingleton()
            .getDefaultGUIContext()
            .injectMouseButtonUp (convert_button (id));
    }

    bool keyPressed(const OIS::KeyEvent &e) {
	auto& ctx = CEGUI::System::getSingleton().getDefaultGUIContext();
	bool a = ctx.injectKeyDown((CEGUI::Key::Scan)e.key);
	bool b = ctx.injectChar(e.text);
	return a || b;
    }

    bool keyReleased(const OIS::KeyEvent &e) {
	return CEGUI::System::getSingleton()
            .getDefaultGUIContext()
            .injectKeyUp((CEGUI::Key::Scan)e.key);
    }
};

#endif /* CEGUIINJECTER_H */
