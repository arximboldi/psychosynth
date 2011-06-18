/**
 *  Time-stamp:  <2011-06-17 10:42:41 raskolnikov>
 *
 *  @file        declare.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Oct 18 15:11:23 2010
 *
 *  @brief Aiding in forward declaring types.
 *  @todo The new template declarations is cool but we have to check
 *  the performance.
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

#ifndef PSYNTH_BASE_DECLARE_H_
#define PSYNTH_BASE_DECLARE_H_

#include <utility>
#include <memory>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>

#include <psynth/base/preprocessor.hpp>

namespace psynth
{
namespace base
{

#define PSYNTH_DECLARE_SHARED_POINTER(type_name)                        \
    typedef std::shared_ptr<type_name> type_name ## _ptr;               \
    typedef std::weak_ptr<type_name> type_name ## _weak_ptr;            \
    template <typename... Args>                                         \
    type_name ## _ptr new_ ## type_name (Args && ... args)              \
    {                                                                   \
        return std::make_shared <type_name> (                           \
            std::forward<Args>(args) ...);                              \
    }

#define PSYNTH_DECLARE_TYPE(type_name)		\
    struct type_name;

#define PSYNTH_DECLARE_SHARED_TYPE(type_name)	\
    PSYNTH_DECLARE_TYPE (type_name)		\
    PSYNTH_DECLARE_SHARED_POINTER (type_name)


/**
 * @todo Use template typedefs when available.
 */
#define PSYNTH_DECLARE_SHARED_POINTER_TEMPLATE(type_name, ...)          \
    PSYNTH_DECLARE_SHARED_POINTER_TEMPLATE_IMPL(                        \
        type_name, PSYNTH_VARG_TO_SEQ(__VA_ARGS__))

#define PSYNTH_TEMPLATE_PARAM_ITER(r, data, i, elem)    \
    BOOST_PP_COMMA_IF(i) elem BOOST_PP_CAT(Par__, i)

#define PSYNTH_TEMPLATE_PARAMS_FULL(param_seq)                          \
    BOOST_PP_SEQ_FOR_EACH_I(PSYNTH_TEMPLATE_PARAM_ITER, _, param_seq)

#define PSYNTH_TEMPLATE_PARAMS(param_seq) \
    BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(param_seq), Par__)

// TODO: Make this generate a weak pointer alias too?
#define PSYNTH_DECLARE_SHARED_POINTER_TEMPLATE_IMPL(type_name, param_seq) \
    template <PSYNTH_TEMPLATE_PARAMS_FULL(param_seq)>                   \
    struct type_name ## _ptr :                                          \
        public std::shared_ptr<type_name <PSYNTH_TEMPLATE_PARAMS(param_seq)> > \
    {                                                                   \
        typedef std::shared_ptr<type_name <PSYNTH_TEMPLATE_PARAMS(param_seq)> > base; \
        template <typename... Args>                                     \
            type_name ## _ptr (Args&&... args)                          \
            : base (std::forward<Args> (args) ...) {}                   \
        template <typename... Args>                                     \
            type_name ## _ptr& operator= (Args&&... args)               \
            { this->base::operator= (std::forward<Args> (args) ...); return *this; } \
    };                                                                  \
                                                                        \
    template <PSYNTH_TEMPLATE_PARAMS_FULL(param_seq), typename... Args> \
    type_name ## _ptr<PSYNTH_TEMPLATE_PARAMS(param_seq)>                \
    inline new_ ## type_name (Args && ... args)                                \
    {                                                                   \
        return                                                          \
            std::make_shared <type_name <PSYNTH_TEMPLATE_PARAMS(param_seq)> > \
            (std::forward<Args>(args)...);                              \
    }

#define PSYNTH_DECLARE_TEMPLATE(type_name, ...)                     \
    template <__VA_ARGS__>                                          \
    struct type_name;

#define PSYNTH_DECLARE_SHARED_TEMPLATE(type_name, ...)                  \
    PSYNTH_DECLARE_TEMPLATE (type_name, __VA_ARGS__)                    \
    PSYNTH_DECLARE_SHARED_POINTER_TEMPLATE (type_name, __VA_ARGS__)

} /* namespace base */
} /* namespace psynth */


#endif /* PSYNTH_BASE_DECLARE_H_ */
