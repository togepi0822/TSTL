#ifndef TSTL_INCLUDE_TGP_CONFIG_H
#define TSTL_INCLUDE_TGP_CONFIG_H

#include <cassert>
#include <type_traits>


/* begin of compiler's predefined macros */
// cpp standard
#if __cplusplus <= 201103L
#   define TGP_STD_VER 11
#elif __cplusplus <= 201402L
#   define TGP_STD_VER 14
#elif __cplusplus <= 201703L
#   define TGP_STD_VER 17
#elif __cplusplus <= 202002L
#   define TGP_STD_VER 20
#elif __cplusplus <= 202302L
#   define TGP_STD_VER 23
#else
#   define TGP_STD_VER 26
#endif

// exceptions
#ifdef __cpp_exceptions
#   define TGP_HAS_EXCEPTIONS
#endif

#ifdef TGP_HAS_EXCEPTIONS
#   define TGP_TRY      try
#   define TGP_CATCH(X) catch (X)
#   define TGP_THROW    throw
#else
#   define TGP_TRY      if (true)
#   define TGP_CATCH(X) else
#   define TGP_THROW
#endif

// rtti
#ifdef __cpp_rtti
#   define TGP_HAS_RTTI
#endif
/* end of predefined macros */


/* begin of compatibility */
// constexpr, return type, return value and noexcept
#if TGP_STD_VER >= 14
#   define TGP_CONSTEXPR_SINCE_CXX14 constexpr
#   define TGP_RETURN_TYPE_SINCE_CXX14(X, Y) X
#   define TGP_RETURN_VALUE_SINCE_CXX14(X) return X
#else
#   define TGP_CONSTEXPR_SINCE_CXX14
#   define TGP_RETURN_TYPE_SINCE_CXX14(X, Y) Y
#   define TGP_RETURN_VALUE_SINCE_CXX14(X) ((void)0)
#endif

#if TGP_STD_VER >= 17
#   define TGP_CONSTEXPR_SINCE_CXX17 constexpr
#   define TGP_RETURN_TYPE_SINCE_CXX17(X, Y) X
#   define TGP_RETURN_VALUE_SINCE_CXX17(X) return X
#   define TGP_NOEXCEPT_UNCONDITIONALLY_SINCE_CXX17 noexcept
#   define TGP_NOEXCEPT_CONDITIONALLY_SINCE_CXX17(X) noexcept(X)
#else
#   define TGP_CONSTEXPR_SINCE_CXX17
#   define TGP_RETURN_TYPE_SINCE_CXX17(X, Y) Y
#   define TGP_RETURN_VALUE_SINCE_CXX17(X) ((void)0)
#   define TGP_NOEXCEPT_UNCONDITIONALLY_SINCE_CXX17
#   define TGP_NOEXCEPT_CONDITIONALLY_SINCE_CXX17(X)
#endif

#if TGP_STD_VER >= 20
#   define TGP_CONSTEXPR_SINCE_CXX20 constexpr
#   define TGP_RETURN_TYPE_SINCE_CXX20(X, Y) X
#   define TGP_RETURN_VALUE_SINCE_CXX20(X) return X
#else
#   define TGP_CONSTEXPR_SINCE_CXX20
#   define TGP_RETURN_TYPE_SINCE_CXX20(X, Y) Y
#   define TGP_RETURN_VALUE_SINCE_CXX20(X) ((void)0)
#endif

#if TGP_STD_VER >= 23
#   define TGP_CONSTEXPR_SINCE_CXX23 constexpr
#   define TGP_RETURN_TYPE_SINCE_CXX23(X, Y) X
#   define TGP_RETURN_VALUE_SINCE_CXX23(X) return X
#else
#   define TGP_CONSTEXPR_SINCE_CXX23
#   define TGP_RETURN_TYPE_SINCE_CXX23(X, Y) Y
#   define TGP_RETURN_VALUE_SINCE_CXX23(X) ((void)0)
#endif

// nodiscard
#if TGP_STD_VER >= 17
#   define TGP_NODISCARD [[nodiscard]]
#else
#   define TGP_NODISCARD
#endif

// likely, unlikely
#if TGP_STD_VER >= 20
#   define TGP_LIKELY [[likely]]
#   define TGP_UNLIKELY [[unlikely]]
#else
#   define TGP_LIKELY
#   define TGP_UNLIKELY
#endif
/* end of compatibility */


/* begin of customized stuff */
// namespace tgp
#define NAMESPACE_TGP_BEGIN namespace tgp {
#define NAMESPACE_TGP_END   }

// invariant judgement
#define TGP_PRECONDITION(cond)  assert(cond)
#define TGP_POSTCONDITION(cond) assert(cond)

// helper alias for type_traits
NAMESPACE_TGP_BEGIN

template<class...>
using void_t = void;

template<bool B>
using bool_constant = std::integral_constant<bool, B>;

template<bool B, class T, class U>
using condition_t = typename std::conditional<B, T, U>::type;

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<class T, class U>
inline constexpr bool is_same_v = std::is_same<T, U>::value;

NAMESPACE_TGP_END
/* end of customized stuff */

#endif // end of TSTL_INCLUDE_TGP_CONFIG_H