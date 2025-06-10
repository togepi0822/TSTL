#ifndef TSTL_INCLUDE_TGP_COMPARE_H
#define TSTL_INCLUDE_TGP_COMPARE_H

#include <algorithm>

#include <tgp/config.h>
#include <tgp/has_range.h>

NAMESPACE_TGP_BEGIN

template<class T, class U, enable_if_t<has_range_and_size_v<T> && has_range_and_size_v<U>, int> = 0>
TGP_NODISCARD bool operator==(const T& lhs, const U& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, class U, enable_if_t<has_range_v<T> && has_range_v<U>, int> = 0>
TGP_NODISCARD bool operator<(const T& lhs, const U& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class T, class U>
TGP_NODISCARD bool operator!=(const T& lhs, const U& rhs) {
    return !(lhs == rhs);
}

template<class T, class U>
TGP_NODISCARD bool operator>(const T& lhs, const U& rhs) {
    return rhs < lhs;
}

template<class T, class U>
TGP_NODISCARD bool operator>=(const T& lhs, const U& rhs) {
    return !(lhs < rhs);
}

template<class T, class U>
TGP_NODISCARD bool operator<=(const T& lhs, const U& rhs) {
    return !(rhs < lhs);
}

NAMESPACE_TGP_END

#endif // end of TSTL_INCLUDE_TGP_COMPARE_H