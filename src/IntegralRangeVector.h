// Copyright 2019 Dmitry Valter
// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef INTEGRALRANGE_INTEGRALRANGEVECTOR_H
#define INTEGRALRANGE_INTEGRALRANGEVECTOR_H

#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace ranges {

    /**
     * Class to store a set of unsigned integral values in a range format
     */
    template<typename T, typename Allocator = std::allocator<T>>
    class IntegralRangeVector {
    public:
        static_assert(std::is_unsigned_v<T>);

        //! Mask that is applied to the beginning and ending of the range
        static constexpr T mask = std::numeric_limits<T>::max() ^(std::numeric_limits<T>::max() >> 1);

        //! Type of value returned when iterating over the container
        typedef std::pair<T, T> value_type;

        //! Type of value used to calculate range size
        typedef std::size_t size_type;

        //! Type that represents difference between two positions in the container
        typedef std::ptrdiff_t difference_type;

    private:
        std::vector<T, Allocator> _rangeVect;
        mutable std::optional<size_type> _length;

    public:

        //! Class used to iterate over range container
        class const_iterator {
        public:

            //! Default constructor - creates an end iterator
            const_iterator() = default;

            //! Type of values stored in the iterated container
            typedef IntegralRangeVector::value_type value_type;

            //! Reference to the stored value
            typedef const value_type &reference;

            //! Constant reference to the stored value
            typedef const value_type &const_reference;

            //! Pointer to the stored value
            typedef const value_type *pointer;

            //! Constant pointer to the stored value
            typedef const value_type *const_pointer;

            //! Difference between two iterators
            typedef std::ptrdiff_t difference_type;

            //! Iterator category
            typedef std::input_iterator_tag iterator_category;

        private:
            typename decltype(_rangeVect)::const_iterator _base_iter;
            typename decltype(_rangeVect)::const_iterator _end_iter;

            value_type _current_value = {T(0u), T(0u)};

            void calculate_value() {
                if (_base_iter >= _end_iter) {
                    _current_value = {T(0u), T(0u)};
                }
                else if (mask & *_base_iter) {
                    _current_value = {*_base_iter & (~mask), *(_base_iter + 1) & (~mask)};
                }
                else {
                    _current_value = {*_base_iter, *_base_iter + T(1u)};
                }
            }

            const_iterator(decltype(_base_iter) base_iter, decltype(_base_iter) end_iter)
                    : _base_iter(base_iter), _end_iter(end_iter) {
                calculate_value();
            }

            friend class IntegralRangeVector<T, Allocator>;

        public:

            //! Equals operator between two iterators
            bool operator==(const const_iterator &other) const { return _base_iter == other._base_iter; }

            //! Not equals operator between two iterators
            bool operator!=(const const_iterator &other) const { return _base_iter != other._base_iter; }

            //! Lesser than operator between two iterators
            bool operator<(const const_iterator &other) const { return _base_iter < other._base_iter; }

            //! Greater than operator between two iterators
            bool operator>(const const_iterator &other) const { return _base_iter > other._base_iter; }

            //! Lesser or equal operator between two iterators
            bool operator<=(const const_iterator &other) const { return _base_iter <= other._base_iter; }

            //! Greater or equal operator between two iterators
            bool operator>=(const const_iterator &other) const { return _base_iter >= other._base_iter; }

            //! Dereference operator
            const_reference operator*() const {
                assert(_base_iter < _end_iter);

                return _current_value;
            }

            //! Member access operator
            const_pointer operator->() const {
                assert(_base_iter < _end_iter);

                return &_current_value;
            }

            //! Postfix increment operator
            const_iterator operator++(int) {
                const_iterator result = *this;
                if (_base_iter < _end_iter && mask & *_base_iter) {
                    ++_base_iter;
                }
                ++_base_iter;

                calculate_value();
                return result;
            }

            //! Prefix increment operator
            const_iterator &operator++() {
                if (_base_iter < _end_iter && mask & *_base_iter) {
                    ++_base_iter;
                }
                ++_base_iter;

                calculate_value();
                return *this;
            }
        };

        /*!
         * Initializes container by copying a vector into the container
         * @param vect Vector to initialize container with
         */
        IntegralRangeVector(const std::vector<T, Allocator> &vect, const Allocator& allocator = Allocator())
                : _rangeVect(vect, allocator) {}

        /*!
         * Initializes container by moving a vector into the container
         * @param vect Vector to initialize container with
         */
        IntegralRangeVector(std::vector<T, Allocator> &&vect, const Allocator& allocator = Allocator())
                : _rangeVect(std::move(vect), allocator) {}

        /*!
         * Initializes container by copying value range
         * @param first First element of the range
         * @param first Last element of the range
         */
        template <typename InputIt>
        IntegralRangeVector(InputIt first, InputIt last, const Allocator& allocator = Allocator())
                : _rangeVect(first, last, allocator) {}

        //! Default constructor - creates an empty container
        IntegralRangeVector(const Allocator& allocator = Allocator())
                : _rangeVect(allocator), _length(0u) {}

        //! Copy constructor
        IntegralRangeVector(const IntegralRangeVector &other) = default;

        //! Move constructor
        IntegralRangeVector(IntegralRangeVector &&other) = default;

        //! Assignment operator
        IntegralRangeVector &operator=(const IntegralRangeVector &other) = default;

        //! Move assignment operator
        IntegralRangeVector &operator=(IntegralRangeVector &&other) = default;

        /*!
         * Reserves a space in the container
         * @param size Amount of range pairs to store in the container
         */
        void reserve(size_type size) {
            assert (size < (std::numeric_limits<size_type>::max() >> 1));
            _rangeVect.reserve(2 * size);
        }

        /*!
         * Appends a value range to the end of the container
         * @param val A value range to append
         */
        void push_back(value_type val) {
            assert((val.first & mask) == 0);
            assert((val.second & mask) == 0);

            if (_length != std::nullopt) {
                *_length += (val.second - val.first);
            }

            if (!_rangeVect.empty() && val.second - val.first > 0) {
                if ((_rangeVect.back() & mask) > 0 && (_rangeVect.back() & ~mask) == val.first) {
                    _rangeVect.back() = (val.second | mask);
                    return;
                }
                else if ((_rangeVect.back() & mask) == 0 && (_rangeVect.back() & ~mask) == val.first - 1) {
                    _rangeVect.back() |= mask;
                    _rangeVect.push_back(val.second | mask);
                    return;
                }
            }
            switch (val.second - val.first) {
                case 0:
                    break;
                case 1:
                    _rangeVect.push_back(val.first);
                    break;
                default:
                    _rangeVect.push_back(val.first | mask);
                    _rangeVect.push_back(val.second | mask);
            }
        }

        /*!
         * Appends a single value to the end of the container
         * @param val A value to append
         */
        void push_back(typename value_type::first_type val) {
            assert((val & mask) == 0);

            if (_length != std::nullopt) {
                ++(*_length);
            }

            if (!_rangeVect.empty()) {
                if ((_rangeVect.back() & mask) > 0 && (_rangeVect.back() & ~mask) == val) {
                    _rangeVect.back() = ((val + 1) | mask);
                    return;
                }
                else if ((_rangeVect.back() & mask) == 0 && (_rangeVect.back() & ~mask) == val - 1) {
                    _rangeVect.back() |= mask;
                    _rangeVect.push_back((val + 1) | mask);
                    return;
                }
            }
            _rangeVect.push_back(val);
        }

        //! Equals operator for two integral range containers
        bool operator==(const IntegralRangeVector &other) const { return _rangeVect == other._rangeVect; }

        //! Not equals operator for two integral range containers
        bool operator!=(const IntegralRangeVector &other) const { return _rangeVect != other._rangeVect; }

        //! Returns a constant iterator pointing to the beginning of the container
        const_iterator cbegin() const { return {_rangeVect.cbegin(), _rangeVect.cend()}; }

        //! Returns a constant iterator pointing to the end of the container
        const_iterator cend() const { return {_rangeVect.cend(), _rangeVect.cend()}; }

        //! Returns an iterator pointing to the beginning of the container
        const_iterator begin() const { return {_rangeVect.cbegin(), _rangeVect.cend()}; }

        //! Returns an iterator pointing to the end of the container
        const_iterator end() const { return {_rangeVect.cend(), _rangeVect.cend()}; }

        //! Returns the internal container that is used to store ranges
        const std::vector<T, Allocator> &getBase() const { return _rangeVect; }

        //! Converts the stored ranges to a vector of contiguous values
        template<typename Allocator1 = Allocator>
        std::vector<T, Allocator1> toVector(const Allocator1 allocator = Allocator1()) const {
            std::vector<T, Allocator1> result(allocator);
            result.reserve(_rangeVect.size());
            for (const auto &range : *this) {
                result.reserve(result.size() + (range.second - range.first));
                for (auto i = range.first; i < range.second; i++) {
                    result.push_back(i);
                }
            }
            return result;
        }

        //! Checks if the container is empty
        bool empty() const {
            return _rangeVect.empty();
        }

        //! Returns an amount of individual values stored in a range container
        size_type length() const {
            if (_length == std::nullopt) {
                _length = 0u;
                for (const auto &range : *this) {
                    *_length += (range.second - range.first);
                }
            }

            return *_length;
        }
    };

}

#endif // INTEGRALRANGE_INTEGRALRANGEVECTOR_H
