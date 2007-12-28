/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef OUTPUTOSS_H
#define OUTPUTOSS_H

#include <pthread.h>

#include "psychosynth.h"
#include "output/Output.h"
#include "common/Thread.h"

class OutputOss : public Output, Runnable
{
    int m_fd;
    int m_format;
    int m_stereo;
    short int* m_buf;
    std::string m_device;
	
    Thread<> m_thread;
public:
    OutputOss(const AudioInfo& info, const std::string& device);
    ~OutputOss();

    bool open();
    bool close();
    bool put(const AudioBuffer& buf, size_t nframes);
    void run();
    void start();
    void stop();
};

#endif /* OUTPUTOSS_H */
