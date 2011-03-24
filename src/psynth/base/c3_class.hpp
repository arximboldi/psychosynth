/**
 *  Time-stamp:  <2011-03-24 18:37:13 raskolnikov>
 *
 *  @file        c3_class.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Dec 25 20:32:07 2010
 *
 *  Linearization of class hierarchies.
 *  http://192.220.96.201/dylan/linearization-oopsla96.html
 *
 *  All ClsDef parameters have this shape:
 *
 *  struct mycls_def
 *  {
 *      template <class Super>
 *      class impl { ... };
 *      typedef mpl::vector<...> bases;
 *  };
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

#ifndef PSYNTH_BASE_C3_CLASS_H_
#define PSYNTH_BASE_C3_CLASS_H_

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/reverse.hpp>
#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/list.hpp>

#include <psynth/base/meta.hpp>

namespace psynth
{
namespace base
{

namespace detail
{

template<class ReversedPartialResult, class RemainingInputs>
struct merge_lists;

template<class ReversedPartialResult, class RemainingInputs>
struct do_merge_lists
{
    template<class C>
    struct candidate
    {
        typedef typename C::type cls;

        template<class L>
        struct tail_p : public mpl::contains<
            typename safe_pop_front<L>::type, cls> {};
        
        typedef typename mpl::if_<
            mpl::not_<any_p<RemainingInputs, mpl::quote1<tail_p> > >,
            cls, mpl::false_>::type type;
    };

    template <class L>
    struct candidate_at_head :
        public mpl::eval_if<mpl::not_<typename mpl::empty<L>::type >,
                            candidate<mpl::front<L> >,
                            mpl::false_> {};

    template <class L>
    struct candidate_at_head_p :
        public mpl::not_<boost::is_same<typename candidate_at_head<L>::type,
                                        mpl::false_> > {};
    
    typedef typename mpl::find_if<
        RemainingInputs,
        mpl::quote1<candidate_at_head_p> >::type next_iter;

    typedef mpl::not_<
        boost::is_same<
            next_iter,
            typename mpl::end<RemainingInputs>::type > > is_consistent;

    BOOST_MPL_ASSERT_MSG(is_consistent::value,
                         PSYNTH_C3_LINEARIZE_INCONSISTENT_HIERARCHY,
                         (ReversedPartialResult, RemainingInputs));

    typedef typename mpl::front<typename mpl::deref<next_iter>::type>::type next;
    
    //BOOST_MPL_ASSERT_MSG(false, PSYNTH_SHOW_NEXT, (next));
    
    template<class L>
    struct remove_next :
        public mpl::eval_if<
        typename boost::is_same<typename safe_front<L>::type, next>::type,
        mpl::pop_front<L>,
        mpl::identity<L> > {};
    
    typedef typename merge_lists<
        typename mpl::push_front<ReversedPartialResult, next>::type,
        typename mpl::transform<RemainingInputs,
                                mpl::quote1<remove_next> >::type >::type type;
};

template<class ReversedPartialResult, class RemainingInputs>
struct merge_lists : public mpl::eval_if<
    typename every_p<RemainingInputs, mpl::quote1<mpl::empty> >::type,
    mpl::reverse<ReversedPartialResult>,
    do_merge_lists<ReversedPartialResult, RemainingInputs> > {};

} /* namespace detail */

template<class ClsDef>
struct direct_superclasses
{
    typedef typename ClsDef::bases type;
};

template<class ClsDef>
struct all_superclasses
{
    typedef typename direct_superclasses<ClsDef>::type superclasses;
    
    typedef typename detail::merge_lists<
        mpl::list<ClsDef>,
        typename concat<
            typename mpl::transform<
                superclasses,
                mpl::quote1<all_superclasses> >::type,
            mpl::list<superclasses> >::type >::type type;
};

namespace detail
{

template <class ClsDefList, bool Empty>
struct c3_inheriter;

template<class ClsDefList>
struct c3_inheriter<ClsDefList, false> :
        public mpl::front<ClsDefList>::type::template impl<
    c3_inheriter<typename mpl::pop_front<ClsDefList>::type,
                 mpl::empty<
                     typename mpl::pop_front<ClsDefList>::type >::type::value
                 > >
{
    typedef typename mpl::front<ClsDefList>::type c3_type;    
};

template<class ClsDefList>
struct c3_inheriter<ClsDefList, true> :
    public mpl::empty_base
{
//    virtual void print (std::ostream& os) {}
};

} /* namespace detail */

template<class ClsDef>
struct generate_c3_hierarchy
{    
    typedef typename all_superclasses<ClsDef>::type classes;

    //BOOST_MPL_ASSERT_MSG(false, PSYNTH_SHOW_CLASSES, (classes));

    typedef typename detail::c3_inheriter<
        classes, mpl::empty<classes>::type::value> type;
};

template<class ClsImpl>
struct get_c3_type
{
    typedef typename ClsImpl::c3_type type;
};

#define PSYNTH_C3_CLASS_BEGIN(cls_name, ...)                            \
    struct cls_name ## _c3                                              \
    {                                                                   \
    typedef ::boost::mpl::transform< ::boost::mpl::list<__VA_ARGS__>, ::boost::mpl::quote1< ::psynth::base::get_c3_type > >::type bases; \
    template<class Super>                                               \
    class impl : public Super

#define PSYNTH_C3_CLASS_END(cls_name)                                   \
    };                                                                  \
    typedef ::psynth::base::generate_c3_hierarchy<cls_name ## _c3>::type cls_name;

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_C3_CLASS_H_ */
