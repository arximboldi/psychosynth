/**
 *  Time-stamp:  <2011-03-16 18:32:49 raskolnikov>
 *
 *  @file        meta.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Sun Dec 26 18:42:36 2010
 *
 *  Meta-programming utilities.
 *
 *  @todo These utilites probably should not be in the psynth::base
 *  namespace to avoid conflicts.
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

#ifndef PSYNTH_BASE_META_H_
#define PSYNTH_BASE_META_H_

#include <boost/mpl/find_if.hpp>
#include <boost/mpl/quote.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/reverse.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/front_inserter.hpp>

namespace psynth
{
namespace base
{

namespace mpl = boost::mpl;

template <class Seq, class Default = mpl::list<> >
struct safe_front :
        public mpl::eval_if<typename mpl::empty<Seq>::type,
                            Default,
                            mpl::front<Seq> > {};

template <class Seq, class Default = mpl::list<> >
struct safe_pop_front :
        public mpl::eval_if<typename mpl::empty<Seq>::type,
                            Default,
                            mpl::pop_front<Seq> > {};

template <class Seq1, class Seq2>
struct concat
{
    typedef typename mpl::copy<
        typename mpl::reverse<Seq1>::type,
        typename mpl::front_inserter<Seq2> >::type type;
};

template <class Sequence, class Pred>
struct every_p
{
    template <class X>
    struct not_pred : public mpl::not_<typename mpl::apply<Pred, X>::type> {};

    typedef typename boost::is_same<
        typename mpl::find_if<Sequence, mpl::quote1<not_pred> >::type,
        typename mpl::end<Sequence>::type>::type type;
};

template<class Sequence, class Pred>
struct any_p
{
    typedef typename mpl::not_<
        boost::is_same<
            typename mpl::find_if<Sequence, Pred>::type,
            typename mpl::end<Sequence>::type> >::type type;
};

struct join
{
    template <class Seq1, class Seq2>
    struct apply
    {
        typedef typename mpl::reverse<
            typename boost::mpl::copy<
                Seq2,
                boost::mpl::front_inserter<Seq1>
                >::type >::type type;
    };
};

/**
 *
 * @todo Use join_view instead?
 */
template<class Sequence>
struct flatten
{
    typedef typename boost::mpl::fold<
        Sequence,
        typename boost::mpl::clear<Sequence>::type,
        join
        >::type type;
};

template <class Element, class Sequence>
struct pair_seq : public mpl::transform<
    Sequence,
    mpl::lambda<typename mpl::pair<Element, mpl::_1> >
    >::type {};


} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_META_H_ */
