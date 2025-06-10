#ifndef TSTL_INCLUDE_TGP_HAS_RANGE_H
#define TSTL_INCLUDE_TGP_HAS_RANGE_H

#include <type_traits>

#include <tgp/config.h>

NAMESPACE_TGP_BEGIN

/* begin of has_range */
template<class T, class = void>
struct has_range : std::false_type {};

template<class T>
struct has_range<T, void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
    : std::true_type {};

template<class T>
inline constexpr bool has_range_v = has_range<T>::value;
/* end of has_range */


/* begin of has_range_and_size */
template<class T, class = void>
struct has_range_and_size : std::false_type {};

template<class T>
struct has_range_and_size<T, void_t<decltype(std::declval<T>().begin()),
                                    decltype(std::declval<T>().end()),
                                    decltype(std::declval<T>().size())>>
    : std::true_type {};

template<class T>
inline constexpr bool has_range_and_size_v = has_range_and_size<T>::value;
/* end of has_range_and_size */

NAMESPACE_TGP_END

#endif // end of TSTL_INCLUDE_TGP_HAS_RANGE_H