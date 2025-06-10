#ifndef TSTL_INCLUDE_TGP_SPLIT_BUFFER_H
#define TSTL_INCLUDE_TGP_SPLIT_BUFFER_H

#include <memory>
#include <type_traits>

#include <tgp/config.h>

NAMESPACE_TGP_BEGIN

/*
 * a split_buffer is kind of a vector with some space reserved at the front.
 * it's used temporarily when:
 *  1. realloc when running out of capacity of vector or shrinking capacity of vector
 *  2. insert elements into vector
 *  ...
 */
template<class T, class Allocator>
struct split_buffer {
    static_assert(std::is_lvalue_reference_v<Allocator>);


    /* begin of alias members */
    using value_type                = T;
    using allocator_type            = std::remove_reference_t<Allocator>;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using pointer                   = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer             = typename std::allocator_traits<allocator_type>::const_pointer;

    using alloc_traits              = std::allocator_traits<allocator_type>;
    /* end of alias members */


    /* begin of data members */
    pointer first_ = nullptr;
    pointer begin_ = nullptr;
    pointer end_   = nullptr;
    _LIBCPP_COMPRESSED_PAIR(pointer, cap_ = nullptr, allocator_type, alloc_);
    /* end of data members */


    /* begin of function members */
    split_buffer(const split_buffer&)            = delete;
    split_buffer& operator=(const split_buffer&) = delete;

    TGP_CONSTEXPR_SINCE_CXX20 split_buffer(size_type cap, size_type pre_reserve, Allocator alloc)
        : alloc_(alloc) {

        first_  = alloc_traits::allocate(alloc_, cap);
        begin_  = first_ + pre_reserve;
        end_    = begin_;
        cap_    = first_ + cap;
    }

    TGP_CONSTEXPR_SINCE_CXX20 ~split_buffer() {
        if (first_) {
            clear();
            alloc_traits::deallocate(alloc_, first_, capacity());
        }
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 size_type capacity() const noexcept {
        return static_cast<size_type>(cap_ - first_);
    }

    TGP_CONSTEXPR_SINCE_CXX20 void clear() noexcept {
        for (; begin_ != end_; ++begin_)
            alloc_traits::destroy(alloc_, std::__to_address(begin_));
    }

    TGP_CONSTEXPR_SINCE_CXX20 void construct_at_end(const size_type n) {
        for (size_type i = 0; i < n; ++i) {
            alloc_traits::construct(alloc_, std::__to_address(end_));
            ++end_;
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void construct_at_end(const size_type n, const T& value) {
        for (size_type i = 0; i < n; ++i) {
            alloc_traits::construct(alloc_, std::__to_address(end_), value);
            ++end_;
        }
    }

    template<class InputIt>
    TGP_CONSTEXPR_SINCE_CXX20 void construct_at_end(InputIt first, InputIt last) {
        for (; first != last; ++first) {
            ::new (static_cast<void*>(std::__to_address(end_))) value_type(*first);
            ++end_;
        }
    }

    template<class... Args>
    TGP_CONSTEXPR_SINCE_CXX20 void emplace_back(Args&&... args) {
        alloc_traits::construct(alloc_, std::__to_address(end_), std::forward<Args>(args)...);
        ++end_;
    }

    template<class... Args>
    TGP_CONSTEXPR_SINCE_CXX20 void emplace_front(Args&&... args) {
        alloc_traits::construct(alloc_, std::__to_address(begin_ - 1), std::forward<Args>(args)...);
        --begin_;
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 bool invariants() const {
        if (first_ == nullptr) {
            if (begin_ != nullptr || end_ != nullptr || cap_ != nullptr)
                return false;
        } else {
            if (first_ > begin_)
                return false;
            if (begin_ > end_)
                return false;
            if (end_ > cap_)
                return false;
        }
        return true;
    }
    /* end of function members */

}; // end of struct split_buffer

NAMESPACE_TGP_END
#endif // end of TSTL_INCLUDE_TGP_SPLIT_BUFFER_H