//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/expected.hpp"

#include <catch2/catch.hpp>

namespace glue {
namespace test {
    //-----------------------------------------------------------------------------
    // Constructors / Assignment
    //-----------------------------------------------------------------------------

    TEST_CASE("expected::expected()")
    {
        auto t = expected<int>();

        SECTION("Default-constructs underlying type T")
        {
            REQUIRE(t.has_value());
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("expected::expected( const expected& )")
    {
        SECTION("Contains Value")
        {
            auto t = expected<int>(5);
            auto copy = t;

            SECTION("Copy contains same value")
            {
                REQUIRE(*copy == *t);
            }
            SECTION("Calls underlying value's copy constructor")
            {
                // TODO: Check that the copy constructor is called
            }
            SECTION("Copied expected contains a value")
            {
                REQUIRE(copy.has_value());
            }
        }

        SECTION("Contains Error")
        {
            auto unexpect =
                make_unexpected<std::error_condition>(std::errc::address_family_not_supported);
            auto t = expected<int>(unexpect);
            auto copy = t;

            SECTION("Copy contains same error")
            {
                REQUIRE(copy.error() == t.error());
            }
            SECTION("Calls underlying error's copy constructor")
            {
                // TODO: Check that the copy constructor is called
            }
            SECTION("Copied expected contains an error")
            {
                REQUIRE(copy.has_error());
            }
        }

        SECTION("Contains Nothing")
        {
            // TODO: Trigger valueless_by_exception and ensure state copies over
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("expected::expected( expected&& )")
    {
        SECTION("Contains Value")
        {
            auto t = expected<int>(5);
            auto copy = t;
            auto move = std::move(t);

            SECTION("Copy contains same value")
            {
                REQUIRE(*copy == *move);
            }
            SECTION("Calls underlying value's move constructor")
            {
                // TODO: Check that the move constructor is called
            }
            SECTION("Moved expected contains a value")
            {
                REQUIRE(move.has_value());
            }
        }

        SECTION("Contains Error")
        {
            auto unexpect =
                make_unexpected<std::error_condition>(std::errc::address_family_not_supported);
            auto t = expected<int>(unexpect);
            auto copy = t;
            auto move = std::move(t);

            SECTION("Copy contains same error")
            {
                REQUIRE(copy.error() == move.error());
            }
            SECTION("Calls underlying error's move constructor")
            {
                // TODO Check that the move constructor is called
            }
            SECTION("Moved expected contains an error")
            {
                REQUIRE(move.has_error());
            }
        }

        SECTION("Contains Nothing")
        {
            // TODO: Trigger valueless_by_exception and ensure state copies over
        }
    }

} // namespace test
} // namespace glue
