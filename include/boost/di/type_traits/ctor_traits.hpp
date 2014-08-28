//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_TYPE_TRAITS_CTOR_TRAITS_HPP
#define BOOST_DI_TYPE_TRAITS_CTOR_TRAITS_HPP

#include "boost/di/aux_/config.hpp"
#include "boost/di/aux_/mpl.hpp"
#include "boost/di/inject.hpp"
#include "boost/di/core/any_type.hpp"
#include "boost/di/type_traits/function_traits.hpp"

#include <string>

namespace boost {
namespace di {

BOOST_DI_HAS_MEMBER_FUNCTION(BOOST_DI_INJECTOR, BOOST_DI_INJECTOR);

template<typename T, std::size_t>
struct get_type {
    using type = T;
};

template<typename, typename, typename>
struct ctor_impl;

template<typename R, typename T, std::size_t... TArgs>
struct ctor_impl<R, T, index_sequence<TArgs...>>
    : pair<
          typename std::is_constructible<R, typename get_type<T, TArgs>::type...>::type
        , type_list<typename get_type<T, TArgs>::type...>
      >
{ };

template<typename, typename>
struct ctor_traits_impl;

template<typename T, std::size_t... Args>
struct ctor_traits_impl<T, index_sequence<Args...>>
    : at_key<
          type_list<>
        , std::true_type
        , typename ctor_impl<T, core::any_type<T>, typename make_index_sequence<Args>::type>::type...
      >
{ };

template<typename T>
struct ctor_traits
    : ctor_traits_impl<T, typename make_index_sequence<BOOST_DI_CFG_CTOR_LIMIT_SIZE + 1>::type>
{ };

template<typename T>
struct ctor_traits<std::basic_string<T>> {
    static void BOOST_DI_INJECTOR();
};

namespace type_traits {

template<typename T, typename = bool_<BOOST_DI_CAT(has_, BOOST_DI_INJECTOR)<T>::value>>
struct ctor_traits;

template<typename T, typename = bool_<BOOST_DI_CAT(has_, BOOST_DI_INJECTOR)<di::ctor_traits<T>>::value>>
struct ctor_traits_impl;

template<typename T>
struct ctor_traits<T, std::true_type>
    : function_traits<decltype(&T::BOOST_DI_INJECTOR)>::type
{ };

template<typename T>
struct ctor_traits<T, std::false_type>
    : ctor_traits_impl<T>::type
{ };

template<typename T>
struct ctor_traits_impl<T, std::true_type>
    : function_traits<decltype(&di::ctor_traits<T>::BOOST_DI_INJECTOR)>::type
{ };

template<typename T>
struct ctor_traits_impl<T, std::false_type>
    : di::ctor_traits<T>::type
{ };

} // namespace type_traits
} // namespace di
} // namespace boost

#endif

