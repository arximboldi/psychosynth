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

#include "gui3d/psychosynth_3d.hpp"
#include <sigc++/sigc++.h>

#if 0
const char* test_func ()
{
  //return psynth::itoa (1, 2);
  return 0;
}

class test_base
{
public:
    virtual void func () {
	std::cout << "Base invoked\n";
    }
};

class test_derived : public test_base
{
public:
    void func () {
	std::cout << "Derived invoked\n";
    }
};
#endif

int main (int argc, const char *argv[])
{
#if 1
    psychosynth_3d main_app;
    return main_app.run (argc, argv);
#endif

#if 0
    std::auto_ptr<test_base> ptr (new test_derived);
    sigc::signal <void> sig;
    sig.connect (sigc::mem_fun (*ptr, &test_base::func));
    sig ();
#endif
    
#if 0
    test_class obj;

    boost::signal <void ()> sig;
    boost::signal <void ()> vod;
    const char* (*fp)();
    boost::function <void ()> func_obj;
    boost::function <void ()> func;
    psynth::timer timer;
    sigc::signal <const char*> sigc;
    sigc::signal <const char*> vosigc;
    
    fp = &test_func;
    sig.connect (&test_func);
    sigc.connect (sigc::ptr_fun (test_func));
    func = &test_func;
    func_obj = boost::bind (&test_class::func, obj);

#define LOOP 10000000

    timer.update ();
    for (int i = 0; i < LOOP; ++i)
	sig ();
    timer.update ();
    std::cout << "signal: " << timer.delta_ticks () << std::endl;

    timer.update ();
    for (int i = 0; i < LOOP; ++i)
	sigc ();
    timer.update ();
    std::cout << "sigc signal: " << timer.delta_ticks () << std::endl;

    timer.update ();
    for (int i = 0; i < LOOP; ++i)
	vosigc ();
    timer.update ();
    std::cout << "sigc empty signal: " << timer.delta_ticks () << std::endl;

    timer.update ();
    for (int i = 0; i < LOOP; ++i)
	vod ();
    timer.update ();
    std::cout << "empty signal: " << timer.delta_ticks () << std::endl;

    timer.update ();
    for (int i = 0; i < LOOP; ++i)
	func ();
    timer.update ();
    std::cout << "function: " << timer.delta_ticks () << std::endl;

    timer.update ();
    for (int i = 0; i < LOOP; ++i)
	fp ();
    timer.update ();
    std::cout << "pointer: " << timer.delta_ticks () << std::endl;
    
#endif
}
