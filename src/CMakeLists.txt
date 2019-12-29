# Copyright 2019 Dmitry Valter
# Copyright 2019 Sviatoslav Dmitriev
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

add_executable(IntegralRangeTest IntegralRangeVector.h IntegralRangeTest.cpp RangeMerger.h)
add_test(IntegralRangeTest IntegralRangeTest)
