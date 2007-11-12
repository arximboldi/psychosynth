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

#ifndef _ERROR_H_
#define _ERROR_H_

#include <cstdlib>
#include <iostream>

#ifndef NDEBUG
 #define DEBUG(err) std::cerr << "DEBUG: (" << __FILE__ << ':' << __LINE__ << ')' \
							  << err << std::endl; 
 #define WARNING(err) std::cerr << "WARNING: (" << __FILE__ << ':' << __LINE__ << ')' << err << std::endl;
 #define FATAL(err) if(1) { std::cerr << "FATAL: (" << __FILE__ << ':' << __LINE__ << ')' \
							  << err << std::endl; exit(-1)} else;
#else
 #define DEBUG(err) 
 #define WARNING(err) std::cerr << "WARNING: " << err << std::endl;
 #define FATAL(err) if(1) { std::cerr << "FATAL: " << err << std::endl; exit(-1) } else;
#endif

#endif /* _ERROR_H_ */
