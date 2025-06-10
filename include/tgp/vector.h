#ifndef TSTL_INCLUDE_TGP_VECTOR_H
#define TSTL_INCLUDE_TGP_VECTOR_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <stdexcept>

#include <tgp/config.h>
#include <tgp/exception.h>
#include <tgp/split_buffer.h>
#include <tgp/compare.h>

NAMESPACE_TGP_BEGIN
template<class T, class Allocator = std::allocator<T>>
class vector {
    static_assert(is_same_v<T, typename Allocator::value_type>);

public:
    /* begin of public alias members */
    using value_type                = T;
    using allocator_type            = Allocator;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using pointer                   = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer             = typename std::allocator_traits<allocator_type>::const_pointer;
    using iterator                  = pointer;
    using const_iterator            = const_pointer;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    /* end of public alias members */


    /* begin of constructor and destructor */
    TGP_CONSTEXPR_SINCE_CXX20 vector() TGP_NOEXCEPT_CONDITIONALLY_SINCE_CXX17(noexcept(allocator_type()))
        : vector(allocator_type()) {}

    TGP_CONSTEXPR_SINCE_CXX20 explicit vector(const allocator_type& alloc) TGP_NOEXCEPT_UNCONDITIONALLY_SINCE_CXX17
        : alloc_(alloc) {}

    explicit vector(const size_type count, const allocator_type& alloc = allocator_type())
        : alloc_(alloc) {
        if (count > 0) {
            TGP_TRY {
                allocate_vector(count);
                construct_at_end(count);
            } TGP_CATCH (...) {
                destroy_vector();
                TGP_THROW;
            }
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 vector(size_type count, const value_type& value, const allocator_type& alloc = allocator_type())
        : alloc_(alloc) {
        if (count > 0) {
            TGP_TRY {
                allocate_vector(count);
                construct_at_end(count, value);
            } TGP_CATCH (...) {
                destroy_vector();
                TGP_THROW;
            }
        }
    }

    template<class InputIt, enable_if_t<std::__has_exactly_input_iterator_category<InputIt>::value, int> = 0>
    TGP_CONSTEXPR_SINCE_CXX20 vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
        : alloc_(alloc) {
        TGP_TRY {
            for (; first != last; ++first)
                emplace_back(*first);
        } TGP_CATCH (...) {
            destroy_vector();
            TGP_THROW;
        }
    }

    template<class InputIt, enable_if_t<std::__has_forward_iterator_category<InputIt>::value, int> = 0>
    TGP_CONSTEXPR_SINCE_CXX20 vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
        : alloc_(alloc) {
        auto count = static_cast<size_type>(std::distance(first, last));
        if (count > 0) {
            TGP_TRY {
                allocate_vector(count);
                construct_at_end(first, last);
            } TGP_CATCH (...) {
                destroy_vector();
                TGP_THROW;
            }
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 vector(const vector& other)
        : vector(other.begin(), other.end(),
                 alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    TGP_CONSTEXPR_SINCE_CXX20 vector(const vector& other, const allocator_type& alloc)
        : vector(other.begin(), other.end(), alloc) {}

    TGP_CONSTEXPR_SINCE_CXX20 vector(vector&& other) TGP_NOEXCEPT_UNCONDITIONALLY_SINCE_CXX17
        : begin_(other.begin_), end_(other.end_), cap_(other.cap_), alloc_(std::move(other.alloc_)) {
        other.begin_ = other.end_ = other.cap_ = nullptr;
    }

    TGP_CONSTEXPR_SINCE_CXX20 vector(vector&& other, const allocator_type& alloc)
        : vector(alloc) {
        if (alloc == other.get_allocator()) {
            begin_ = other.begin_;
            end_   = other.end_;
            cap_   = other.cap_;
            other.begin_ = other.end_ = other.cap_ = nullptr;
        } else {
            const size_type count = other.size();
            if (count > 0) {
                TGP_TRY {
                    allocate_vector(count);
                    construct_at_end(std::make_move_iterator(other.begin_), std::make_move_iterator(other.end_));
                } TGP_CATCH (...) {
                    destroy_vector();
                    TGP_THROW;
                }
            }
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20
    vector(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
        : vector(init.begin(), init.end(), alloc) {}

    TGP_CONSTEXPR_SINCE_CXX20 ~vector() {
        destroy_vector();
    }
    /* end of constructor and destructor */


    /* begin of element access */
    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 reference operator[] (const size_type pos) {
        return begin_[pos];
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reference operator[] (const size_type pos) const {
        return begin_[pos];
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 reference at(const size_type pos) {
        if (pos >= size())
            TGP_TRY_THROW(std::out_of_range("tgp::vector::at element access out of range"));
        return begin_[pos];
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reference at(const size_type pos) const {
        if (pos >= size())
            TGP_TRY_THROW(std::out_of_range("tgp::vector::at element access out of range"));
        return begin_[pos];
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 reference front() {
        return *begin_;
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reference front() const {
        return *begin_;
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 reference back() {
        return *(end_ - 1);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reference back() const {
        return *(end_ - 1);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 value_type* data() noexcept {
        return std::__to_address(begin_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const value_type* data() const noexcept {
        return std::__to_address(begin_);
    }
    /* end of element access */


    /* begin of iterators */
    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 iterator begin() noexcept {
        return iterator(begin_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_iterator begin() const noexcept {
        return const_iterator(begin_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_iterator cbegin() const noexcept {
        return begin();
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 iterator end() noexcept {
        return iterator(end_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_iterator end() const noexcept {
        return const_iterator(end_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_iterator cend() const noexcept {
        return end();
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 const_reverse_iterator crend() const noexcept {
        return rend();
    }
    /* end of iterators */


    /* begin of capacity */
    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 size_type capacity() const noexcept {
        return static_cast<size_type>(cap_ - begin_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 size_type max_size() const noexcept {
        return alloc_traits::max_size(alloc_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 bool empty() const noexcept {
        return begin_ == end_;
    }

    TGP_CONSTEXPR_SINCE_CXX20 void reserve(const size_type new_cap) {
        if (new_cap > capacity()) {
            if (new_cap > max_size())
                TGP_TRY_THROW(std::length_error("tgp::vector::reserve demanding size exceeds max size"));
            split_buffer<value_type, allocator_type&> sb(new_cap, size(), alloc_);
            swap_with_split_buffer(sb);
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void shrink_to_fit() {
        if (capacity() > size()) {
            TGP_TRY {
                split_buffer<value_type, allocator_type&> sb(size(), size(), alloc_);
                swap_with_split_buffer(sb);
            } TGP_CATCH (...) {

            }
        }
    }
    /* end of capacity */


    /* begin of modifiers */
    TGP_CONSTEXPR_SINCE_CXX20 void clear() noexcept {
        destruct_at_end(begin_);
    }

    TGP_CONSTEXPR_SINCE_CXX20 iterator erase(const_iterator pos) {
        pointer p = begin_ + (pos - begin());
        destruct_at_end(std::move(p + 1, end_, p));
        return p;
    }

    TGP_CONSTEXPR_SINCE_CXX20 iterator erase(const_iterator first, const_iterator last) {
        pointer p = begin_ + (first - begin());
        destruct_at_end(std::move(p + (last - first), end_, p));
        return p;
    }

    TGP_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator pos, const value_type& value) {
        pointer p = begin_ + (pos - begin());
        if (end_ == cap_) {
            split_buffer<value_type, allocator_type&> sb(recommend_cap(size() + 1), p - begin_, alloc_);
            sb.construct_at_end(value);
            p = swap_with_split_buffer(sb, p);
        } else {
            if (p == end_) {
                construct_one_at_end(value);
            } else {
                split_buffer<value_type, allocator_type&> sb(capacity(), p - begin_, alloc_);
                sb.construct_at_end(value);
                p = swap_with_split_buffer(sb, p);
            }
        }
        return p;
    }

    TGP_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator pos, value_type&& value) {
        pointer p = begin_ + (pos - begin());
        if (end_ == cap_) {
            split_buffer<value_type, allocator_type&> sb(recommend_cap(size() + 1), p - begin_, alloc_);
            sb.construct_at_end(std::move(value));
            p = swap_with_split_buffer(sb, p);
        } else {
            if (p == end_) {
                construct_one_at_end(std::move(value));
            } else {
                move_range(p, end_, p + 1);
                *p = std::move(value);
            }
        }
        return p;
    }

    TGP_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator pos, size_type count, const value_type& value) {
        pointer p = begin_ + (pos - begin());
        if (count > 0) {
            const size_type cur_size = size();
            if (count + cur_size > capacity()) {
                split_buffer<value_type, allocator_type&> sb(recommend_cap(count + cur_size), p - begin(), alloc_);
                sb.construct_at_end(count, value);
                p = swap_with_split_buffer(sb, p);
            } else {
                auto n = static_cast<size_type>(end_ - p);
                if (count > n) {
                    construct_at_end(count - n, value);
                    move_range(p, p + n, end_);
                    std::fill_n(p, n, *(p + n));
                } else {
                    split_buffer<value_type, allocator_type&> sb(capacity(), p - begin(), alloc_);
                    sb.construct_at_end(count, value);
                    p = swap_with_split_buffer(sb, p);
                }
            }
        }
        return p;
    }

    template<class InputIt, enable_if_t<std::__has_exactly_input_iterator_category<InputIt>::value, int> = 0>
    TGP_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator pos, InputIt first, InputIt last) {
        const difference_type n = pos - begin();
        const size_type cur_size = size();
        while (first != last) {
            emplace_back(*first);
            ++first;
        }
        std::rotate(begin() + n, begin() + cur_size, end());
        return begin() + n;
    }

    template<class InputIt, enable_if_t<std::__has_forward_iterator_category<InputIt>::value, int> = 0>
    TGP_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator pos, InputIt first, InputIt last) {
        auto count = static_cast<size_type>(std::distance(first, last));
        const size_type n = pos - begin();
        pointer p = begin_ + n;
        if (count + size() > capacity()) {
            split_buffer<value_type, allocator_type&> sb(recommend_cap(count + size()), n, alloc_);
            sb.construct_at_end(first, last);
            p = swap_with_split_buffer(sb, p);
        } else {
            auto m = static_cast<size_type>(end() - pos);
            InputIt mid = std::next(first, count);
            if (count > m) {
                mid = std::next(first, m);
                construct_at_end(mid, last);
                count = m;
            }
            if (count > 0) {
                move_range(p, p + m, end_);
                std::copy(first, mid, p);
            }
        }
        return p;
    }

    TGP_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        pointer p = begin_ + (pos - begin_);
        if (end_ == cap_) {
            split_buffer<value_type, allocator_type&> sb(recommend_cap(size() + 1), p - begin_, alloc_);
            sb.emplace_back(std::forward<Args>(args)...);
            p = swap_with_split_buffer(sb, p);
        } else {
            if (p == end_) {
                construct_one_at_end(std::forward<Args>(args)...);
            } else {
                move_range(p, end_, p + 1);
                *p = value_type{std::forward<Args>(args)...};
            }
        }

        return p;
    }

    template<class... Args>
    TGP_CONSTEXPR_SINCE_CXX20
    TGP_RETURN_TYPE_SINCE_CXX17(reference, void)
    emplace_back(Args&&... args) {
        if (end_ == cap_) {
            split_buffer<value_type, allocator_type&> sb(recommend_cap(size() + 1), size(), alloc_);
            sb.emplace_back(std::forward<Args>(args)...);
            swap_with_split_buffer(sb);
        } else {
             construct_one_at_end(std::forward<Args>(args)...);
        }
        TGP_RETURN_VALUE_SINCE_CXX17(*(end_ - 1));
    }

    TGP_CONSTEXPR_SINCE_CXX20 void push_back(const value_type& value) {
        emplace_back(value);
    }

    TGP_CONSTEXPR_SINCE_CXX20 void push_back(value_type&& value) {
        emplace_back(std::move(value));
    }

    TGP_CONSTEXPR_SINCE_CXX20 void pop_back() {
        destruct_at_end(end_ - 1);
    }

    TGP_CONSTEXPR_SINCE_CXX20 void resize(const size_type count) {
        const size_type cur_size = size();
        if (count <= cur_size) {
            destruct_at_end(begin_ + count);
        } else {
            if (count > capacity()) {
                split_buffer<value_type, allocator_type&> sb(recommend_cap(count), cur_size, alloc_);
                sb.construct_at_end(count - cur_size);
                swap_with_split_buffer(sb);
            } else {
                construct_at_end(count - cur_size);
            }
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void resize(const size_type count, const value_type& value) {
        const size_type cur_size = size();
        if (count <= cur_size) {
            destruct_at_end(begin_ + count);
        } else {
            if (count > capacity()) {
                split_buffer<value_type, allocator_type&> sb(recommend_cap(count), cur_size, alloc_);
                sb.construct_at_end(count - cur_size, value);
                swap_with_split_buffer(sb);
            } else {
                construct_at_end(count - cur_size, value);
            }
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void swap(vector& other)
    noexcept(alloc_traits::propagate_on_container_swap::value ||
             alloc_traits::is_always_equal::value) {
        using std::swap;
        swap(begin_, other.begin_);
        swap(end_, other.end_);
        swap(cap_, other.cap_);
        if (alloc_traits::propagate_on_container_swap::value)
            swap(alloc_, other.alloc_);
    }
    /* end of modifiers */


    /* begin of miscellaneous */
    TGP_CONSTEXPR_SINCE_CXX20 allocator_type get_allocator() noexcept {
        return alloc_;
    }

    TGP_CONSTEXPR_SINCE_CXX20 vector& operator=(const vector& other) {
        if (this != std::addressof(other)) {
            if (alloc_traits::propagate_on_container_copy_assignment::value) {
                if (alloc_ != other.alloc_) {
                    deallocate_vector();
                    alloc_ = other.alloc_;
                }
            }
            assign(other.begin_, other.end_);
        }
        return *this;
    }

    TGP_CONSTEXPR_SINCE_CXX20 vector& operator=(vector&& other)
    TGP_NOEXCEPT_CONDITIONALLY_SINCE_CXX17(alloc_traits::propagate_on_container_move_assignment::value ||
                             alloc_traits::is_always_equal::value) {
        if (alloc_ == other.alloc_ || alloc_traits::propagate_on_container_move_assignment::value) {
            alloc_ = std::move(other.alloc_);
            begin_ = other.begin_;
            end_   = other.end_;
            cap_   = other.cap_;
            other.begin_ = other.end_ = other.cap_ = nullptr;
        } else {
            assign(std::make_move_iterator(other.begin_), std::make_move_iterator(other.end_));
        }
        return *this;
    }

    TGP_CONSTEXPR_SINCE_CXX20 vector& operator=(std::initializer_list<value_type> ilist) {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    TGP_CONSTEXPR_SINCE_CXX20 void assign(size_type count, const value_type& value) {
        const size_type cur_size = size();
        std::fill_n(begin_, std::min(cur_size, count), value);
        if (count > capacity()) {
            split_buffer<value_type, allocator_type&> sb(recommend_cap(count), cur_size, alloc_);
            sb.construct_at_end(count - cur_size, value);
            swap_with_split_buffer(sb);
        } else {
            if (count > cur_size) {
                construct_at_end(count - cur_size, value);
            } else {
                destruct_at_end(begin_ + count);
            }
        }
    }

    template<class InputIt, enable_if_t<std::__has_exactly_input_iterator_category<InputIt>::value, int> = 0>
    TGP_CONSTEXPR_SINCE_CXX20 void assign(InputIt first, InputIt last) {
        pointer cur = begin_;
        for (; cur != end_ && first != last; ++first, (void)++cur)
            *cur = *first;
        if (cur == end_) {
            for (; first != last; ++first)
                emplace_back(*first);
        } else {
            destruct_at_end(cur);
        }
    }

    template<class InputIt, enable_if_t<std::__has_forward_iterator_category<InputIt>::value, int> = 0>
    TGP_CONSTEXPR_SINCE_CXX20 void assign(InputIt first, InputIt last) {
        auto count = static_cast<size_type>(std::distance(first, last));
        if (count > capacity()) {
            deallocate_vector();
            allocate_vector(recommend_cap(count));
            construct_at_end(first, last);
        } else {
            const size_type cur_size = size();
            if (count > cur_size) {
                InputIt mid = first + count;
                std::copy(first, mid, begin_);
                construct_at_end(mid, last);
            } else {
                std::copy(first, last, begin_);
                destruct_at_end(begin_ + count);
            }
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void assign(std::initializer_list<value_type> ilist) {
        assign(ilist.begin(), ilist.end());
    }
    /* end of miscellaneous */

private:
    /* begin of private data members and alias members */
    using alloc_traits = std::allocator_traits<allocator_type>;

    pointer begin_ = nullptr;
    pointer end_   = nullptr;
    _LIBCPP_COMPRESSED_PAIR(pointer, cap_ = nullptr, allocator_type, alloc_);
    /* end of private data members and alias members */


    /* begin of private function members */
    TGP_CONSTEXPR_SINCE_CXX20 void construct_at_end(const size_type n) {
        for (size_type i = 0; i < n; ++i) {
            alloc_traits::construct(alloc_, std::__to_address(end_));
            ++end_;
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void construct_at_end(const size_type n, const value_type& value) {
        for (size_type i = 0; i < n; ++i) {
            alloc_traits::construct(alloc_, std::__to_address(end_), value);
            ++end_;
        }
    }

    template<class Iter>
    TGP_CONSTEXPR_SINCE_CXX20 void construct_at_end(Iter first, Iter last) {
        for (; first != last; ++first) {
            ::new (static_cast<void*>(std::__to_address(end_))) value_type(*first);
            ++end_;
        }
    }

    template<class... Args>
    TGP_CONSTEXPR_SINCE_CXX20 void construct_one_at_end(Args&&... args) {
        alloc_traits::construct(alloc_, std::__to_address(end_), std::forward<Args>(args)...);
        ++end_;
    }

    TGP_CONSTEXPR_SINCE_CXX20 void destruct_at_end(pointer new_end) noexcept {
        pointer soon_to_be_end = end_;
        while (soon_to_be_end != new_end) {
            alloc_traits::destroy(alloc_, std::__to_address(--soon_to_be_end));
        }
        end_ = new_end;
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 size_type recommend_cap(const size_type new_size) const {
        const size_type ms = max_size();
        if (new_size > ms) {
            TGP_TRY_THROW(std::length_error("tgp::vector::recommend_cap demanding size exceeds max size"));
        }
        const size_type cur_cap = capacity();
        if (cur_cap > ms / 2)
            return ms;
        return std::max(cur_cap * 2, new_size);
    }

    TGP_CONSTEXPR_SINCE_CXX20 void swap_with_split_buffer(split_buffer<value_type, allocator_type&>& sb) {
        std::__uninitialized_allocator_relocate(
            alloc_, std::__to_address(begin_), std::__to_address(end_), std::__to_address(sb.first_));
        end_ = begin_;
        alloc_traits::deallocate(alloc_, begin_, capacity());
        begin_ = sb.first_;
        end_ = sb.end_;
        cap_ = sb.cap_;
    }

    TGP_CONSTEXPR_SINCE_CXX20 pointer swap_with_split_buffer(split_buffer<value_type, allocator_type&>& sb, pointer p) {
        pointer ret = sb.begin_;
        std::__uninitialized_allocator_relocate(
            alloc_, std::__to_address(p), std::__to_address(end_), std::__to_address(sb.end_));
        sb.end_ += (end_ - p);
        end_ = p;
        std::__uninitialized_allocator_relocate(
            alloc_, std::__to_address(begin_), std::__to_address(p), std::__to_address(sb.first_));
        sb.begin_ -= (p - begin_);
        end_ = begin_;
        alloc_traits::deallocate(alloc_, begin_, capacity());
        begin_ = sb.first_;
        end_ = sb.end_;
        cap_ = sb.cap_;
        return ret;
    }

    TGP_CONSTEXPR_SINCE_CXX20 void move_range(pointer from_s, pointer from_e, pointer to) {
        pointer old_last = end_;
        difference_type n = old_last - to;
        for (pointer p = from_s + n; p < from_e; ++p)
            construct_one_at_end(std::move(*p));
        std::move_backward(from_s, from_s + n, old_last);
    }

    TGP_NODISCARD TGP_CONSTEXPR_SINCE_CXX20 bool invariants() const {
        if (begin_ == nullptr) {
            if (end_ != nullptr || cap_ != nullptr)
                return false;
        } else {
            if (begin_ > end_)
                return false;
            if (end_ > cap_)
                return false;
        }
        return true;
    }

    TGP_CONSTEXPR_SINCE_CXX20 void destroy_vector() noexcept {
        if (begin_) {
            clear();
            alloc_traits::deallocate(alloc_, begin_, capacity());
        }
    }

    TGP_CONSTEXPR_SINCE_CXX20 void allocate_vector(const size_type n) {
        if (n > max_size())
            TGP_TRY_THROW(std::length_error("tgp::vector::allocator_vector demanding size exceeds max size"));
        begin_ = alloc_traits::allocate(alloc_, n);
        end_   = begin_;
        cap_   = begin_ + n;
    }

    TGP_CONSTEXPR_SINCE_CXX20 void deallocate_vector() {
        if (begin_) {
            clear();
            alloc_traits::deallocate(alloc_, begin_, capacity());
            begin_ = end_ = cap_ = nullptr;
        }
    }
    /* end of private function members */

}; // end of class vector

NAMESPACE_TGP_END

namespace std {

template<class T, class Alloc>
void swap(tgp::vector<T, Alloc>& lhs, tgp::vector<T, Alloc>& rhs)
TGP_NOEXCEPT_CONDITIONALLY_SINCE_CXX17(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // end of namespace std

#endif // end of TSTL_INCLUDE_TGP_VECTOR_H