/**
 *  Time-stamp:  <2010-10-20 21:09:55 raskolnikov>
 *
 *  @file        concept.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Oct 20 19:58:39 2010
 *
 *  Concepts implementation.
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_CONCEPT_H_
#define PSYNTH_CONCEPT_H_

namespace psynth
{
namespace base
{

#ifdef BOOST_GIL_USE_CONCEPT_CHECK
#define PSYNTH_CLASS_REQUIRE(type_var, ns, concept)	\
    BOOST_CLASS_REQUIRE(type_var, ns, concept);
template <typename C> void psynth_function_requires ()
{
    boost::function_requires<C>();
}
#else
#define PSYNTH_CLASS_REQUIRE(T, NS, C) 
template <typename C> void psynth_function_requires() {}
#endif

/** \ingroup BasicConcepts */
/**
\code
auto concept DefaultConstructible<typename T> {
    T::T();    
};
\endcode
*/
template <typename T>
struct DefaultConstructible
{
    void constraints () {
	boost::function_requires <boost::DefaultConstructibleConcept<T> >();
    }
};

/** \ingroup BasicConcepts */
/**
\code
auto concept CopyConstructible<typename T> {
    T::T(T);
    T::~T();
};
\endcode
*/
template <typename T>
struct CopyConstructible
{
    void constraints () {
        function_requires<boost::CopyConstructibleConcept<T> >();
    }
};

/** \ingroup BasicConcepts */
/**
\code
auto concept Assignable<typename T, typename U = T> {
    typename result_type;
    result_type operator=(T&, U);    
};
\endcode
*/
template <typename T>
struct Assignable
{
    void constraints() {
        function_requires<boost::AssignableConcept<T> >();
    }
};

/** \ingroup BasicConcepts */
/**
\code
auto concept EqualityComparable<typename T, typename U = T> {
    bool operator==(T x, T y);    
    bool operator!=(T x, T y) { return !(x==y); }
};
\endcode
*/
template <typename T>
struct EqualityComparable
{
    void constraints() {
        function_requires<boost::EqualityComparableConcept<T> >();
    }
};

/** \ingroup BasicConcepts */

/**
\code
concept SameType<typename T, typename U>;// unspecified
\endcode
*/
template <typename T, typename U>
struct SameType
{
    void constraints() {
        BOOST_STATIC_ASSERT((boost::is_same<T,U>::value_core));
    }
};

/** \ingroup BasicConcepts */
/**
\code
auto concept Swappable<typename T> {
    void swap(T&,T&);
};
\endcode
*/
template <typename T>
struct Swappable
{
    void constraints() {
        using std::swap;
        swap (x, y);
    }
    T x, y;
};

/** \ingroup BasicConcepts */
/**
\code
auto concept Regular<typename T> :
     DefaultConstructible<T>, CopyConstructible<T>, EqualityComparable<T>, 
     Assignable<T>, Swappable<T> {};
\endcode
*/
template <typename T>
struct Regular
{
    void constraints() {
        gil_function_requires< boost::DefaultConstructibleConcept<T> >();
        gil_function_requires< boost::CopyConstructibleConcept<T> >();
	// ==, !=
        gil_function_requires< boost::EqualityComparableConcept<T> >(); 
        gil_function_requires< boost::AssignableConcept<T> >();
        gil_function_requires< Swappable<T> >();
    }
};

/** \ingroup BasicConcepts */
/**
\code
auto concept Metafunction<typename T> {
    typename type;
};
\endcode
*/
template <typename T>
struct Metafunction
{
    void constraints() {
        typedef typename T::type type;
    }
};

} /* namespace base */
} /* namespace psynth */

/**
 * @note To understand this better, please refer to:
 *    http://stlab.adobe.com/gil/html/gildesignguide.html
 * And use this terminology mapping terminology mapping:
 *
 *   Color       = Channel
 *   Color Space = Channel Space = Set of Colors
 *   Color Base  = Channel Base  = Set of Colors + Layout
 *   Channel     = Sample        = Where we store the color, i.e. int
 *   Pixel       = Frame
 */

#endif /* PSYNTH_CONCEPT_H_ */

