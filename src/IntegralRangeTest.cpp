// Copyright 2019 Dmitry Valter
// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "RangeMerger.h"
#include "IntegralRangeVector.h"

using namespace ranges;

SCENARIO("Integral range test", "[integral range test]") {
    REQUIRE(std::is_same_v<typename std::iterator_traits<IntegralRangeVector<uint64_t>::const_iterator>::iterator_category,
        std::input_iterator_tag>);

    WHEN("Base vector is filled with plain values") {
        size_t COUNT = 32;
        typedef uint8_t utype;

        REQUIRE(IntegralRangeVector<utype>::mask == 0x80);

        std::vector<utype> vect;
        for (utype i = 0; i < COUNT; i++) {
            vect.push_back(i);
        }

        THEN("Iteration loop will work fine") {
            auto arr = IntegralRangeVector<utype>(vect);
            auto it = arr.cbegin();
            for (utype i = 0; i < COUNT; i++) {
                REQUIRE(it->first == i);
                REQUIRE(it->second == i + 1);
                it++;
            }

            REQUIRE(it == arr.cend());
        }

        THEN("Iteration loop with postfix increment will work fine") {
            auto arr = IntegralRangeVector<utype>(vect);
            auto it = arr.cbegin();
            for (utype i = 0; i < COUNT; i++) {
                REQUIRE(it->first == i);
                REQUIRE((it++)->second == i + 1);
            }

            REQUIRE(it == arr.cend());
        }

        THEN("Range-based iteration loop will work fine") {
            assert(0 == COUNT % 2);

            auto arr = IntegralRangeVector<utype>(vect);
            utype ctr = 0;
            for (const auto& pair : arr) {
                REQUIRE(pair.first == ctr);
                REQUIRE(pair.second == ctr + 1);
                ctr++;
            }
        }
    }

    WHEN("Base vector is filled with only ranges") {
        size_t COUNT = 64;
        typedef uint16_t utype;

        REQUIRE(IntegralRangeVector<utype>::mask == 0x8000);

        std::vector<utype> vect;
        for (utype i = 0; i < COUNT; i++) {
            vect.push_back(i | 0x8000u);
            vect.push_back(utype((2 * i + 7u) | 0x8000u));
        }

        THEN("Iteration loop will work fine") {
            auto arr = IntegralRangeVector<utype>(vect);
            auto it = arr.cbegin();
            for (utype i = 0; i < COUNT; i++) {
                REQUIRE(it->first == i);
                REQUIRE(it->second == 2 * i + 7);
                it++;
            }

            REQUIRE(it == arr.cend());
        }

        THEN("Iteration loop with pstfix increment will work fine") {
            auto arr = IntegralRangeVector<utype>(vect);
            auto it = arr.cbegin();
            for (utype i = 0; i < COUNT; i++) {
                REQUIRE(it->first == i);
                REQUIRE((it++)->second == 2 * i + 7);
            }

            REQUIRE(it == arr.cend());
        }

        THEN("Range-based iteration loop will work fine") {
            assert(0 == COUNT % 2);

            auto arr = IntegralRangeVector<utype>(vect);
            utype ctr = 0;
            for (const auto& pair : arr) {
                REQUIRE(pair.first == ctr);
                REQUIRE(pair.second == 2 * ctr + 7);
                ctr++;
            }
        }
    }

    WHEN("Base vector is filled with values and ranges mixed") {
        size_t COUNT = 64;
        typedef uint64_t utype;

        REQUIRE(IntegralRangeVector<utype>::mask == 0x8000000000000000lu);

        std::vector<utype> vect;
        for (utype i = 0; i < COUNT;) {
            vect.push_back(i++);
            vect.push_back(i++);

            vect.push_back(i | IntegralRangeVector<utype>::mask);
            vect.push_back(utype((i++ * 2 + 7u) | IntegralRangeVector<utype>::mask));

            vect.push_back(i++);

            vect.push_back(i | IntegralRangeVector<utype>::mask);
            vect.push_back(utype((i++ * 2 + 8u) | IntegralRangeVector<utype>::mask));
            vect.push_back(i | IntegralRangeVector<utype>::mask);
            vect.push_back(utype((i++ * 2 + 9u) | IntegralRangeVector<utype>::mask));
        }

        THEN("Iteration loop will work fine") {
            auto arr = IntegralRangeVector<utype>(vect);
            auto it = arr.cbegin();
            for (utype i = 0; i < COUNT;) {
                REQUIRE(it->first == i);
                REQUIRE(it->second == i + 1);
                it++;
                i++;

                REQUIRE(it->first == i);
                REQUIRE(it->second == i + 1);
                it++;
                i++;

                REQUIRE(it->first == i);
                REQUIRE(it->second == 2 * i + 7u);
                it++;
                i++;

                REQUIRE(it->first == i);
                REQUIRE(it->second == i + 1);
                it++;
                i++;

                REQUIRE(it->first == i);
                REQUIRE(it->second == 2 * i + 8u);
                it++;
                i++;

                REQUIRE(it->first == i);
                REQUIRE(it->second == 2 * i + 9u);
                it++;
                i++;
            }

            REQUIRE(it == arr.cend());
        }

        THEN("Iteration loop with pstfix increment will work fine") {
            auto arr = IntegralRangeVector<utype>(vect);
            auto it = arr.cbegin();
            for (utype i = 0; i < COUNT;) {
                REQUIRE(it->first == i);
                REQUIRE((it++)->second == i + 1);
                i++;

                REQUIRE(it->first == i);
                REQUIRE((it++)->second == i + 1);
                i++;

                REQUIRE(it->first == i);
                REQUIRE((it++)->second == 2 * i + 7u);
                i++;

                REQUIRE(it->first == i);
                REQUIRE((it++)->second == i + 1);
                i++;

                REQUIRE(get_first(it) == i);
                REQUIRE(get_last(it) == 2 * i + 8u);
                REQUIRE(it->first == i);
                REQUIRE((it++)->second == 2 * i + 8u);
                i++;

                REQUIRE(it->first == i);
                REQUIRE((it++)->second == 2 * i + 9u);
                i++;
            }

            REQUIRE(it == arr.cend());
        }

        THEN("Range-based iteration loop will work fine") {
            assert(0 == COUNT % 2);

            auto arr = IntegralRangeVector<utype>(vect);
            utype ctr = 0;
            for (const auto& pair : arr) {
                switch (ctr % 6) {
                case 0:
                case 1:
                case 3:
                    REQUIRE(pair.first == ctr);
                    REQUIRE(pair.second == ctr + 1);
                    break;
                case 2:
                    REQUIRE(pair.first == ctr);
                    REQUIRE(pair.second == 2 * ctr + 7u);
                    break;
                case 4:
                case 5:
                    REQUIRE(pair.first == ctr);
                    REQUIRE(pair.second == 2 * ctr + (ctr % 6) + 4);
                default:
                    break;
                }
                ctr++;
            }
        }
    }

    SECTION("Inserters") {
        typedef uint32_t utype;
        constexpr utype COUNT = 1024;
        auto arr = IntegralRangeVector<utype>();

        for (utype i = 0; i < COUNT; i++) {
            arr.push_back({ i * i, (i+1)*(i+1) - 1 });
        }

        auto it = arr.begin();
        for (utype i = 1; i < COUNT; i++, it++) {
            REQUIRE(it->first == i * i);
            REQUIRE(it->second == (i+1)*(i+1) - 1);
        }
        REQUIRE(it == arr.end());
    }

    SECTION("Insert concatenate") {
        typedef uint32_t utype;
        constexpr utype COUNT = 1024;
        auto arr = IntegralRangeVector<utype>();

        for (utype i = 0; i < COUNT; i++) {
            arr.push_back({ i * i, (i+1)*(i+1) });
        }

        REQUIRE(arr.length() == COUNT*COUNT);
        REQUIRE(std::distance(arr.begin(), arr.end()) == 1);
        REQUIRE(arr.begin()->first == 0);
        REQUIRE(arr.begin()->second == COUNT * COUNT);
    }

    SECTION("Insert single value") {
        typedef uint32_t utype;
        constexpr utype COUNT = 1024;
        auto arr = IntegralRangeVector<utype>();

        for (utype i = 0; i < COUNT; i++) {
            arr.push_back(i * 2);
        }

        auto it = arr.begin();
        for (utype i = 0; i < COUNT; i++, it++) {
            REQUIRE(it->first == i * 2);
            REQUIRE(it->second == i * 2 + 1);
        }
        REQUIRE(it == arr.end());
    }

    SECTION("Insert concatenate single value") {
        typedef uint32_t utype;
        constexpr utype COUNT = 1024;
        auto arr = IntegralRangeVector<utype>();

        for (utype i = 0; i < COUNT; i++) {
            arr.push_back(i);
        }

        REQUIRE(arr.length() == COUNT);
        REQUIRE(std::distance(arr.begin(), arr.end()) == 1);
        REQUIRE(arr.begin()->first == 0);
        REQUIRE(arr.begin()->second == COUNT);
    }

    SECTION("Comparators") {
        size_t COUNT = 32;
        typedef uint8_t utype;

        std::vector<utype> vect;
        auto arr = IntegralRangeVector(vect);

        REQUIRE(arr.cbegin() == arr.cend());

        REQUIRE(arr.cbegin()++ == arr.cend());
        REQUIRE(++arr.cbegin() > arr.cend());

        vect.resize(COUNT, 1);
        arr = IntegralRangeVector(vect);

        REQUIRE(arr.cbegin() < arr.cend());
        auto it = arr.cbegin();

        REQUIRE(it == arr.cbegin());
        REQUIRE(arr.cbegin() <= it);
        REQUIRE(arr.cbegin() >= it);
        REQUIRE(it <= arr.cbegin());
        REQUIRE(it == arr.cbegin());
        it++;

        REQUIRE(it > arr.cbegin());
        REQUIRE(it != arr.cbegin());
        REQUIRE(arr.cbegin() <= it);
        REQUIRE(arr.cbegin() < it);

        REQUIRE(arr.getBase() == vect);
    }

    GIVEN("Dummy manually created range vectors") {
        std::vector<IntegralRangeVector<uint16_t>> ranges(4);
        insert_back(ranges[0], { 0, 7 });
        insert_back(ranges[0], { 9, 15 });
        insert_back(ranges[0], { 15, 17 });
        insert_back(ranges[0], { 100, 200 });
        insert_back(ranges[0], { 888, 889 });
        insert_back(ranges[0], { 889, 1000 });

        insert_back(ranges[1], { 3, 5 });
        insert_back(ranges[1], { 6, 8 });
        insert_back(ranges[1], { 14, 16 });
        insert_back(ranges[1], { 25, 27 });
        insert_back(ranges[1], { 101, 103 });
        insert_back(ranges[1], { 193, 777 });
        insert_back(ranges[1], { 888, 889 });
        insert_back(ranges[1], { 889, 1000 });

        insert_back(ranges[2], { 2, 5 });
        insert_back(ranges[2], { 7, 8 });
        insert_back(ranges[2], { 14, 15 });
        insert_back(ranges[2], { 15, 16 });
        insert_back(ranges[2], { 100, 102 });
        insert_back(ranges[2], { 102, 210 });
        insert_back(ranges[2], { 888, 889 });
        insert_back(ranges[2], { 889, 2000 });

        insert_back(ranges[3], { 4, 888 });
        insert_back(ranges[3], { 888, 889 });
        insert_back(ranges[3], { 889, 3000 });

        WHEN("Merger is called for intersection") {
            auto interseted = intersect_ranges(ranges);
            THEN("Result should match expected") {
                auto it = interseted.begin();
                REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 4, 5 });
                REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 14, 16 });
                REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 101, 103 });
                REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 193, 200 });
                REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 888, 1000 });
            }
        }

        WHEN("Merger is called for union") {
            auto interseted = unite_ranges(ranges);
            THEN("Result should match expected") {
                auto it = interseted.begin();
                REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 0, 3000 });
            }
        }

        WHEN("Last vector is removed") {
            ranges.resize(3);
            WHEN("Merger is called for union") {
                auto interseted = unite_ranges(ranges);
                THEN("Result should match expected") {
                    auto it = interseted.begin();
                    REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 0, 8 });
                    REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 9, 17 });
                    REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 25, 27 });
                    REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 100, 777 });
                    REQUIRE(*(it++) == std::pair<uint16_t, uint16_t>{ 888, 2000 });
                }
            }
        }
    }

    GIVEN("Dummy manually created range vectors on plain std::vector") {
        std::vector<std::vector<uint16_t>> ranges(4);
        insert_back(ranges[0], { 0, 7 });
        insert_back(ranges[0], { 9, 15 });
        insert_back(ranges[0], { 15, 17 });
        insert_back(ranges[0], { 100, 200 });
        insert_back(ranges[0], { 888, 889 });
        insert_back(ranges[0], { 889, 1000 });

        insert_back(ranges[1], { 3, 5 });
        insert_back(ranges[1], { 6, 8 });
        insert_back(ranges[1], { 14, 16 });
        insert_back(ranges[1], { 25, 27 });
        insert_back(ranges[1], { 101, 103 });
        insert_back(ranges[1], { 193, 777 });
        insert_back(ranges[1], { 888, 889 });
        insert_back(ranges[1], { 889, 1000 });

        insert_back(ranges[2], { 2, 5 });
        insert_back(ranges[2], { 7, 8 });
        insert_back(ranges[2], { 14, 15 });
        insert_back(ranges[2], { 15, 16 });
        insert_back(ranges[2], { 100, 102 });
        insert_back(ranges[2], { 102, 210 });
        insert_back(ranges[2], { 888, 889 });
        insert_back(ranges[2], { 889, 2000 });

        insert_back(ranges[3], { 4, 888 });
        insert_back(ranges[3], { 888, 889 });
        insert_back(ranges[3], { 889, 3000 });

        WHEN("Merger is called for intersection") {
            auto interseted = intersect_ranges(ranges);

            THEN("Result should match expected") {
                std::vector<uint16_t> expected;
                expected.push_back(4);
                expected.push_back(14);
                expected.push_back(15);
                expected.push_back(101);
                expected.push_back(102);
                for (uint16_t i = 193; i < 200; i++) {
                    expected.push_back(i);
                }

                for (uint16_t i = 888; i < 1000; i++) {
                    expected.push_back(i);
                }

                REQUIRE(expected == interseted);
            }
        }

        WHEN("Merger is called for union") {
            auto interseted = unite_ranges(ranges);

            THEN("Result should match expected") {
                std::vector<uint16_t> expected;

                for (uint16_t i = 0; i < 3000; i++) {
                    expected.push_back(i);
                }

                REQUIRE(expected == interseted);
            }
        }

        WHEN("Last vector is removed") {
            ranges.resize(3);
            WHEN("Merger is called for union") {
                auto interseted = unite_ranges(ranges);
                THEN("Result should match expected") {
                    std::vector<uint16_t> expected;

                    for (uint16_t i = 0; i < 8; i++) {
                        expected.push_back(i);
                    }

                    for (uint16_t i = 9; i < 17; i++) {
                        expected.push_back(i);
                    }

                    for (uint16_t i = 25; i < 27; i++) {
                        expected.push_back(i);
                    }

                    for (uint16_t i = 100; i < 777; i++) {
                        expected.push_back(i);
                    }

                    for (uint16_t i = 888; i < 2000; i++) {
                        expected.push_back(i);
                    }

                    REQUIRE(expected == interseted);
                }
            }
        }
    }

    GIVEN("Empty vector of ranges") {
        std::vector<std::vector<uint16_t>> ranges;

        WHEN("Merger is called for intersection") {
            auto interseted = intersect_ranges(ranges);

            THEN("Result should match expected") {
                std::vector<uint16_t> expected;

                REQUIRE(expected == interseted);
            }
        }
        
        WHEN("Merger is called for union") {
            auto interseted = unite_ranges(ranges);

            THEN("Result should match expected") {
                std::vector<uint16_t> expected;

                REQUIRE(expected == interseted);
            }
        }
    }

    GIVEN("One vector of range values") {
        std::vector<std::vector<uint16_t>> ranges(1, { 1, 2, 3 });

        WHEN("Merger is called for intersection") {
            auto interseted = intersect_ranges(ranges);

            THEN("Result should match expected") {
                std::vector<uint16_t> expected{ 1, 2, 3 };

                REQUIRE(expected == interseted);
            }
        }
        
        WHEN("Merger is called for union") {
            auto interseted = unite_ranges(ranges);

            THEN("Result should match expected") {
                std::vector<uint16_t> expected{ 1, 2, 3 };

                REQUIRE(expected == interseted);
            }
        }
    }
}
