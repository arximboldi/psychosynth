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
 
/***************************************************************************
 *                                                                         *
 *   Most of the code in this file was taken from libassa.                 *
 *                                                                         *
 *   LIBASSA                                                               *
 *   =======                                                               *
 *                                                                         *
 *   Copyright (C) 1997-2002,2005  Vladislav Grinchenko                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 ***************************************************************************/

#ifndef CMD_LINE_OPTS_H
#define CMD_LINE_OPTS_H

#include <string> 
#include <vector>

class CmdLineOpts; 

/**
 * @file CmdLineOpts.h
 *
 *	Class to handle processing command-line options.
 */

/** 
 * @class Option.
 *
 * This class is a helper class of CmdLineOpts class. 
 * It is not used by any other class and cannot be instantiated.
 */
class Option {
public:
	friend class CmdLineOpts; 

	/**
	 * @enum type_t
	 * Option type. Each option, except for flags has a value
	 * following it on the command line. Following types are supported:
	 */
	enum type_t { 
		string_t=0,	/**< Convert argument to STL string  */
		int_t,		/**< Convert argument to int  */
		uint_t,		/**< Convert argument to unsigned int  */
		long_t,		/**< Convert argument to long  */
		ulong_t,	/**< Convert argument to unsinged long  */
		double_t,	/**< Convert argument to double  */
		float_t,	/**< Convert argument to float  */
		flag_t,		/**< No argument; bool value is flipped.  */
		func_t,		/**< Convert argument to function  */
		func_one_t,	/**< Convert argument to function with one argument  */
		none_t					
	};

private:
	/**
	 * Private default constructor
	 */
	Option ();

	/**
	 * Private constructor
	 */
	Option (char shopt_, const std::string& lopt_, type_t type_, void* val_);

	/**
	 * Write object state to the log file
	 */
	void dump () const;

	/**
	 * Return the type of the Option object
	 */
	const char* type_c_str ();

private:
	char    m_short_name;
	std::string  m_long_name;
	type_t  m_type;
	void*   m_val;
};

inline 
Option::Option () :
	m_short_name (' '), m_long_name (""), m_type (none_t), m_val (NULL) 
{
}

inline
Option::Option (char shopt_, const std::string& lopt_, type_t type_, void* val_) : 
	m_short_name (shopt_), 	m_long_name (lopt_),
	m_type (type_), m_val (val_) 
{
}

/* -------------------------------------------------------------------------- */

/** 
 * @class CmdLineOpts.
 *
 * CmdLineOpts class parsers the command line arguments.
 */
class CmdLineOpts
{
public:
	typedef void (* OPTS_FUNC) (void);
	typedef void (* OPTS_FUNC_ONE) (const std::string&);

	typedef std::vector<Option> OptionSet;

	/**
	 * Default constructor
	 */
	CmdLineOpts ();

	/**
	 * Do-nothing destructor
	 */
	virtual ~CmdLineOpts () { 
	}

	/** 
	 * Add binary flag option. 
	 * @param c  short name
	 * @param s  long name
	 * @param f  pointer to bool flag variable
	 * @return true on success, false on error
	 */
	bool addFlagOpt (const char c, const std::string& s, bool* f);

	/**
	 * Add an option with STL string argument.
	 * @param c  short name
	 * @param s  long name
	 * @param str  pointer to string variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, std::string* str);

	/** 
	 * Add an option with integer argument.
	 * @param c  short name
	 * @param s  long name
	 * @param i  pointer to int variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, int* i);

	/** 
	 * Add an option with unsigned integer argument.
	 * @param c  short name
	 * @param s  long name
	 * @param ui  pointer to u_int variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, unsigned int* ui);

	/**
	 * Add an option with long argument.
	 * @param c  short name
	 * @param s  long name
	 * @param l  pointer to long variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, long* l);

	/**
	 * Add an option with unsigned long argument.
	 * @param c  short name
	 * @param s  long name
	 * @param ul  pointer to unsigned long variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, unsigned long* ul);

	/**
	 * Add an option with double argument.
	 * @param c  short name
	 * @param s  long name
	 * @param d  pointer to double variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, double* d);

	/** 
	 * Add an option with float argument.
	 * @param c  short name
	 * @param s  long name
	 * @param f  pointer to float variable
	 * @return true on success, false on error
	 */
	bool addOpt (const char c, const std::string& s, float* f);

	/**
	 * Add an option with static function argument.
	 * This void function with no arguments will be called 
	 * when command line option is processed. An option installed
	 * is treated as binary flag option.
	 * @param c_  short name
	 * @param s_  long name
	 * @param f_  pointer to the static function
	 * @return true on success, false on error
	 */
	bool addOpt (const char c_, const std::string& s_, OPTS_FUNC f_);

	/** 
	 * Add an option with static function argument.
	 * This void function with STL string arguments will be called 
	 * when command line option is processed. The option value is
	 * delivered via function's argument.
	 * @param c_  short name
	 * @param s_  long name
	 * @param f_  pointer to the static function
	 * @return true on success, false on error
	 */
	bool addOpt (const char c_, const std::string& s_, OPTS_FUNC_ONE f_);

	/**
	 * Remove option for the option list.
	 * @param c_ short name
	 * @param s_ long name
	 * @return true if found, false if not
	 */
	bool delOpt (const char c_, const std::string& s_);

	/**
	 * Parse command line arguments based on installed options set.
	 * @return true on success, false on error.
	 */
	bool parseArgs (const char* argv[]);

	/* TODO: XML config file parsing */ 
#if 0
	/**
	 * Parse configuration parameters found in [options] section
	 * of the INI file. File should be already loaded with load().
	 * @param inifile_ The INI file to parse.
	 * @return The number of options parsed or -1 if [options] section
	 *         is missing
	 */
	int parseConfigFile (IniFile& inifile_);
#endif

	/** 
	 * If previous call to one of member functions returned false,
	 * retrieve detailed error message.
	 */
	const char* getOptError () const;

	/**
	 * Static function. Convert string list of command line options
	 * into dynamically allocated argv-like array. The array is 
	 * terminated with NULL. This memory must be freed after it has 
	 * been used. Remember that the first parameter is process name.
	 * @param src_  command line option string
	 * @param argc_ number of options found in the source string
	 * @param argv_ returns a pointer to the heap-allocated memory
	 */
	static void str2argv (const std::string& src_, int&  argc_, char**& argv_);

	/** 
	 * Free up memory allocated by <TT>str_to_argv()</TT> function
	 */
	static void freeArgv (char**& argv_);

	/**
	 * Write options set to the log file.
	 */
	void dump () const;

protected:
	/**
	 * Detect if supplied option is valid
	 */
	bool is_valid (const char sopt_, const std::string& lopt_);
	
	/**
	 * Reset error message to an empty string
	 */
	void set_error_none ();

	/**
	 * Perform value assignment to the node. Data conversion  happens here.
	 */
	bool assign (Option* node_, const char* op_);

	/**
	 * Locate option in the options set.
	 */
	Option* find_option (const char* str_);

	/**
	 * Locate option in the options set.
	 */
	Option* find_option (const char letter_);

	/**
	 * Process positional argument arg_. This method must be
	 * overloaded by the derived class to take advantage of it.
	 * @param arg_ positional argument value
	 */
	virtual void pos_arg (const char* arg_);

private:

	OptionSet m_opts_set;
	std::string    m_error;
};


inline void 
CmdLineOpts::pos_arg (const char* /*arg_*/) { /* no-opt*/ }
	
inline
CmdLineOpts::CmdLineOpts () : m_opts_set (), m_error ("")
{
	set_error_none ();
}

inline void
CmdLineOpts::set_error_none ()
{
	m_error = "";
}

inline const char*
CmdLineOpts::getOptError () const
{
	return (m_error.c_str ());
}

#endif /* CMD_LINE_OPTS_H */  
