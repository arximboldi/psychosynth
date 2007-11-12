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

#ifndef __OUTPUTOSS_H__
#define __OUTPUTOSS_H__

#include <pthread.h>

#include "psychosynth.h"
#include "output/Output.h"
#include "common/Thread.h"

class OutputOss : public Output, Runnable {
	int oss_fd;
	int oss_format;
	int oss_stereo;
	short int* buf;
	std::string oss_device;
	
	Thread oss_thread;
public:
	OutputOss(const AudioInfo& info, const std::string& device);
	~OutputOss();

	bool open();
	bool close();
	bool put(const Real* rbuf, int nframes);
	bool put(const AudioBuffer& buf);
	void run();
	void start();
	void stop();
};

#endif
