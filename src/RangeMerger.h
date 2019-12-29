// Copyright 2019 Dmitry Valter
// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef INTEGRALRANGE_MERGERANGER_H
#define INTEGRALRANGE_MERGERANGER_H

#include "IntegralRangeVector.h"

namespace ranges {

    template<typename Test>
    struct is_pair : std::false_type {};

    template<class FirstT, class SecondT>
    struct is_pair<std::pair<FirstT, SecondT>>: std::true_type {};

    template <class Test>
    constexpr bool is_pair_v = is_pair<Test>::value;

    /*!
     * Get the beginning of the range on a current iterator position (overload for unsigned value iterators)
     * @tparam It Iterator type
     * @param it iterator
     * @return Beginning of the range on a current iterator position
     */
    template<typename It,
            std::enable_if_t<std::is_unsigned_v<typename std::iterator_traits<It>::value_type>, bool> = true>
    auto get_first(const It &it) -> typename std::iterator_traits<It>::value_type {
        return *it;
    }

    /*!
     * Get the ending of the range on a current iterator position (overload for unsigned value iterators)
     * @tparam It Iterator type
     * @param it iterator
     * @return Ending of the range on a current iterator position
     */
    template<typename It,
            std::enable_if_t<std::is_unsigned_v<typename std::iterator_traits<It>::value_type>, bool> = true>
    auto get_last(const It &it) -> typename std::iterator_traits<It>::value_type {
        return typename std::iterator_traits<It>::value_type(*it + 1u);
    }

    /*!
     * Get the beginning of the range on a current iterator position (overload for range iterators)
     * @tparam It Iterator type
     * @param it iterator
     * @return Beginning of the range on a current iterator position
     */
    template<typename It, std::enable_if_t<is_pair_v<typename It::value_type> &&
                                           std::is_unsigned_v<typename It::value_type::first_type>, bool> = true>
    auto get_first(const It &it) -> typename std::iterator_traits<It>::value_type::first_type {
        return it->first;
    }

    /*!
     * Get the ending of the range on a current iterator position (overload for range iterators)
     * @tparam It Iterator type
     * @param it iterator
     * @return Ending of the range on a current iterator position
     */
    template<typename It, std::enable_if_t<is_pair_v<typename It::value_type> &&
                                           std::is_unsigned_v<typename It::value_type::first_type>, bool> = true>
    auto get_last(const It &it) -> typename std::iterator_traits<It>::value_type::second_type {
        return it->second;
    }

    /*!
     * Inserts a range to the end of the container (overload for range iterators)
     * @tparam Cont Container type
     * @param output Container to insert value to
     * @param range Range of values to insert
     */
    template<typename Cont, std::enable_if_t<std::is_arithmetic_v<typename Cont::value_type>, bool> = true>
    void insert_back(Cont &output, std::pair<typename Cont::value_type, typename Cont::value_type> range) {
        for (typename Cont::value_type i = range.first; i < range.second; i++) {
            output.push_back(i);
        }
    }

    /*!
     * Inserts a range to the end of the container (overload for unsigned value iterators)
     * @tparam Cont Container type
     * @param output Container to insert value to
     * @param range Range of values to insert
     */
    template<typename Cont, std::enable_if_t<is_pair_v<typename Cont::value_type>, bool> = true>
    void insert_back(
            Cont &output,
            std::pair<typename Cont::value_type::first_type, typename Cont::value_type::second_type> range) {
        output.push_back(range);
    }

    /*!
     * Calculates an intersection of multiple ranges
     * @tparam Cont Ranges container type
     * @param ranges Ranges to calculate intersection of
     * @return Intersection of multiple ranges
     */
    template<typename Cont>
    auto intersect_ranges(const std::vector<Cont> &ranges) -> Cont {
        if (ranges.empty()) {
            return Cont{};
        }

        if (ranges.size() == 1) {
            return ranges[0];
        }

        typedef decltype(get_first(ranges[0].begin())) value_type;
        Cont result;

        value_type curRangeBegin = 0;
        value_type curRangeEnd = std::numeric_limits<value_type>::max();

        std::optional<std::pair<value_type, value_type>> pendingRange;
        size_t containerToForward = 0;
        std::vector<decltype(ranges[0].begin())> iters;
        iters.reserve(ranges.size());

        for (auto &range : ranges) {
            if (range.begin() == range.end()) {
                return Cont{};
            }
            iters.push_back(range.begin());
        }

        int iter = 0;

        for (;;) {

            for (size_t i = 0; i < ranges.size(); i++) {
                assert(iters[i] != ranges[i].end());

                auto begin = get_first(iters[i]);
                auto end = get_last(iters[i]);

                if (begin > curRangeBegin) {
                    curRangeBegin = begin;
                }

                if (end < curRangeEnd) {
                    curRangeEnd = end;
                    containerToForward = i;
                }
            }

            if (curRangeBegin < curRangeEnd) {
                if (pendingRange) {
                    if (pendingRange->second == curRangeBegin) {
                        pendingRange->second = curRangeEnd;
                    }
                    else {
                        insert_back(result, pendingRange.value());
                        pendingRange = {curRangeBegin, curRangeEnd};
                    }
                }
                else {
                    pendingRange = {curRangeBegin, curRangeEnd};
                }
            }

            curRangeEnd = std::numeric_limits<value_type>::max();

            if (++iters[containerToForward] == ranges[containerToForward].end()) {
                if (pendingRange) {
                    insert_back(result, pendingRange.value());
                }
                break;
            }
            iter++;
        }

        return result;
    }

    /*!
     * Calculates a union of multiple ranges
     * @tparam Cont Ranges container type
     * @param ranges Ranges to calculate union of
     * @return Union of multiple ranges
     */
    template<typename Cont>
    auto unite_ranges(std::vector<Cont> ranges) -> Cont {
        if (ranges.empty()) {
            return Cont{};
        }

        if (ranges.size() == 1) {
            return ranges[0];
        }

        typedef decltype(get_first(ranges[0].begin())) value_type;
        Cont result;

        constexpr value_type LAST = std::numeric_limits<value_type>::max();

        value_type curRangeBegin = 0;
        value_type curRangeEnd = LAST;

        std::optional<std::pair<value_type, value_type>> pendingRange;
        size_t containerToForward = 0;
        std::vector<decltype(ranges[0].begin())> iters;
        iters.reserve(ranges.size());

        for (auto &range : ranges) {
            iters.push_back(range.begin());
        }

        int iter = 0;

        for (;;) {
            containerToForward = 0;
            curRangeBegin = LAST;
            curRangeEnd = LAST;

            for (size_t i = 0; i < ranges.size(); i++) {
                if (iters[i] >= ranges[i].end()) {
                    continue;
                }
                auto begin = get_first(iters[i]);
                auto end = get_last(iters[i]);
                if (begin < curRangeBegin) {
                    curRangeBegin = begin;
                    curRangeEnd = end;
                    containerToForward = i;
                }
            }

            if (curRangeEnd == LAST) {
                if (pendingRange) {
                    insert_back(result, pendingRange.value());
                }
                break;
            }

            if (curRangeBegin < curRangeEnd) {
                if (pendingRange) {
                    if (pendingRange->second >= curRangeBegin) {
                        if (pendingRange->second < curRangeEnd) {
                            pendingRange->second = curRangeEnd;
                        }
                    }
                    else {
                        insert_back(result, pendingRange.value());
                        pendingRange = {curRangeBegin, curRangeEnd};
                    }
                }
                else {
                    pendingRange = {curRangeBegin, curRangeEnd};
                }
            }

            curRangeEnd = std::numeric_limits<value_type>::max();

            ++iters[containerToForward];
            iter++;
        }

        return result;
    }

}

#endif // INTEGRALRANGE_MERGERANGER_H
