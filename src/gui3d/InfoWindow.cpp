/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#include <iostream>
#include <libpsynth/version.h>
#include "config.h"
#include "gui3d/InfoWindow.h"

using namespace CEGUI;
using namespace std;

#define IW_WIDTH   400
#define IW_HEIGHT  400

static const char* ABOUT_TEXT =
    "Psychosynth is an interactive free software sound synthesis system.\n\n"
    "For further information and reporting bugs or suggestions please visit our webpage.\n\n"
    "Webpage: http://www.suicidesoft.com/psychosynth\n\n"
    "Compilation options:\n";

static const char* COMPILE_OPTIONS_TEXT =
    "  * OSC network support: "
#ifdef PSYNTH_HAVE_OSC
    "Yes.\n"
#else
    "No.\n"
#endif
    "  * PCM file support: "
#ifdef PSYNTH_HAVE_PCM
    "Yes.\n"
#else
    "No.\n"
#endif
    "  * OGG file support: "
#ifdef PSYNTH_HAVE_OGG
    "Yes.\n"
#else
    "No.\n"
#endif
    "  * ALSA audio support: "
#ifdef PSYNTH_HAVE_ALSA
    "Yes.\n"
#else
    "No.\n"
#endif
    "  * Jack audio support: "
#ifdef PSYNTH_HAVE_JACK
    "Yes.\n"
#else
    "No.\n"
#endif
    "  * OSS audio support: "
#ifdef PSYNTH_HAVE_OSS
    "Yes.\n"
#else
    "No.\n"
#endif
    "  * XML config support: "
#ifdef PSYNTH_HAVE_XML
    "Yes.\n"
#else
    "No.\n"
#endif	
    "";

static const char* CREDITS_TEXT =
    "This program was done with care and effort by all these people. We hope you liked it :-)";

static const char* CREDITS_LIST_TEXT =
    "[ Project leader ]\n"
    "Juan Pedro Bolivar Puente\n"
    "<jpboli@correo.ugr.es>\n";

static const char* LICENSE_TEXT =
    "This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 3 of the License, or \
(at your option) any later version.\
\n\n\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \
GNU General Public License for more details. \
\n\n\
You should have received a copy of the GNU General Public License \
along with this program. If not, see <http://www.gnu.org/licenses/>.";


CEGUI::FrameWindow* InfoWindow::createWindow()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow"));
	
    window->setPosition(UVector2(UDim(0.5, -IW_WIDTH/2), UDim(0.5, -IW_HEIGHT/2)));
    window->setSize    (UVector2(UDim(0, IW_WIDTH),UDim(0, IW_HEIGHT)));
    window->setText("Info");

    Imageset* logoimg =
	ImagesetManager::getSingleton().createImageset("Logo.imageset");
    Window* logo = wmgr.createWindow("TaharezLook/StaticImage");
    logo->setProperty("Image", "set:Logo image:the_logo");
    logo->setPosition(UVector2(UDim(0.5, -logoimg->getImageWidth("the_logo")/2),
			       UDim(0, 30)));
    logo->setSize(UVector2(UDim(0, logoimg->getImageWidth("the_logo")),
			   UDim(0, logoimg->getImageHeight("the_logo"))));
    logo->setProperty("FrameEnabled", "False");
    logo->setProperty("BackgroundEnabled", "false");

    Window* slogan = wmgr.createWindow("TaharezLook/StaticText");
    slogan->setPosition(UVector2(UDim(0.5, -logoimg->getImageWidth("the_logo")/2),
				 UDim(0, logoimg->getImageHeight("the_logo") + 26)));
    slogan->setSize(UVector2(UDim(0, logoimg->getImageWidth("the_logo")), UDim(0, 20)));
    slogan->setProperty("FrameEnabled", "False");
    slogan->setProperty("BackgroundEnabled", "false");
    slogan->setText("Psychosynth "  VERSION);


    Window* container = wmgr.createWindow("TaharezLook/TabControl");
    container->setPosition(UVector2(UDim(0, 20),
				    UDim(0, logoimg->getImageHeight("the_logo")+50)));
    container->setSize(UVector2(UDim(1, -40), UDim(1, -logoimg->getImageHeight("the_logo")-70)));

    /*
     * About section.
     */
    Window* about = wmgr.createWindow("DefaultGUISheet");
    about->setText("About");
    about->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    about->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    Window* about_st = wmgr.createWindow("TaharezLook/StaticText");
    about_st->setPosition(UVector2(UDim(0.0, 10), UDim(0.0, 10)));
    about_st->setSize(UVector2(UDim(1, -20), UDim(0, 140)));
    about_st->setProperty("FrameEnabled", "False");
    about_st->setProperty("BackgroundEnabled", "false");
    about_st->setProperty("HorzFormatting", "WordWrapLeftAligned");
    about_st->setText(ABOUT_TEXT);

    MultiLineEditbox* about_mle = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    about_mle->setPosition(UVector2(UDim(0, 10), UDim(0.0, 150)));
    about_mle->setSize(UVector2(UDim(1, -20), UDim(1, -160)));
    about_mle->setReadOnly(true);
    about_mle->setText(COMPILE_OPTIONS_TEXT);
    
    /*
     * Credits section.
     */
    Window* credits = wmgr.createWindow("DefaultGUISheet");
    credits->setText("Credits");
    credits->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    credits->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    Window* credits_st = wmgr.createWindow("TaharezLook/StaticText");
    credits_st->setPosition(UVector2(UDim(0.0, 10), UDim(0.0, 10)));
    credits_st->setSize(UVector2(UDim(1, -20), UDim(0, 40)));
    credits_st->setProperty("FrameEnabled", "False");
    credits_st->setProperty("BackgroundEnabled", "false");
    credits_st->setProperty("HorzFormatting", "WordWrapLeftAligned");
    credits_st->setText(CREDITS_TEXT);

    Window* credits_mle  = wmgr.createWindow("TaharezLook/StaticText");
    credits_mle->setPosition(UVector2(UDim(0, 10), UDim(0.0, 50)));
    credits_mle->setSize(UVector2(UDim(1, -20), UDim(1, -60)));
    credits_mle->setText(CREDITS_LIST_TEXT);
    credits_mle->setProperty("VertScrollbar", "True");
    credits_mle->setProperty("HorzFormatting", "WordWrapCentred");

    /*
     * License section.
     */
    Window* license = wmgr.createWindow("DefaultGUISheet");
    license->setText("License");
    license->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    license->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    MultiLineEditbox* license_mle = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    license_mle->setPosition(UVector2(UDim(0, 10), UDim(0.0, 10)));
    license_mle->setSize(UVector2(UDim(1, -20), UDim(1, -20)));
    license_mle->setReadOnly(true);
    license_mle->setText(LICENSE_TEXT);
    
    about->addChildWindow(about_st);
    about->addChildWindow(about_mle);

    credits->addChildWindow(credits_st);
    credits->addChildWindow(credits_mle);

    license->addChildWindow(license_mle);
    
    container->addChildWindow(about);
    container->addChildWindow(credits);
    container->addChildWindow(license);
    
    window->addChildWindow(container);
    window->addChildWindow(logo);
    window->addChildWindow(slogan);
    
    return window;
}

