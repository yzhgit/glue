//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/tribool.hpp"

#include <catch2/catch.hpp>

namespace glue {
namespace test {
    //----------------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------------

    TEST_CASE("tribool::tribool()")
    {
        auto t = tribool();

        SECTION("Constructor creates indeterminate tribool")
        {
            REQUIRE(indeterminate(t));
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("tribool::tribool( bool t )")
    {
        SECTION("Constructor accepting true constructs true tribool")
        {
            tribool t = true;

            REQUIRE(t == true);
        }

        SECTION("Constructor accepting false constructs false tribool")
        {
            tribool t = false;

            REQUIRE(t == false);
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("tribool::tribool( indeterminate_t )")
    {
        SECTION("Constructor accepting indeterminate constructs indeterminate tribool")
        {
            tribool t = indeterminate;

            REQUIRE((t == indeterminate));
        }
    }

    //----------------------------------------------------------------------------
    // Boolean Operators
    //----------------------------------------------------------------------------

    TEST_CASE("tribool::operator bool()")
    {
        SECTION("false returns false")
        {
            auto result = tribool(false);

            REQUIRE_FALSE(bool(result));
        }

        SECTION("indeterminate returns false")
        {
            auto result = tribool(indeterminate);

            REQUIRE_FALSE(bool(result));
        }

        SECTION("true returns true")
        {
            auto result = tribool(true);

            REQUIRE(bool(result));
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("tribool::operator !()")
    {
        SECTION("false returns true")
        {
            auto result = tribool(false);

            REQUIRE(!result);
        }

        SECTION("indeterminate returns false")
        {
            auto result = tribool(indeterminate);

            REQUIRE_FALSE(!result);
        }

        SECTION("true returns false")
        {
            auto result = tribool(true);

            REQUIRE_FALSE(!result);
        }
    }

    //----------------------------------------------------------------------------
    // Binary Boolean Operators
    //----------------------------------------------------------------------------

    TEST_CASE("tribool operator && ( const tribool&, const tribool& )")
    {
        SECTION("false && false returns false")
        {
            auto result = tribool(false) && tribool(false);

            REQUIRE(result == false);
        }

        SECTION("false && true returns false")
        {
            auto result = tribool(false) && tribool(true);

            REQUIRE(result == false);
        }

        SECTION("true && true returns true")
        {
            auto result = tribool(true) && tribool(true);

            REQUIRE(result == true);
        }

        SECTION("false && indeterminate returns indeterminate")
        {
            auto result = tribool(false) && tribool(indeterminate);

            REQUIRE((result == indeterminate));
        }

        SECTION("true && indeterminate returns indeterminate")
        {
            auto result = tribool(true) && tribool(indeterminate);

            REQUIRE((result == indeterminate));
        }

        SECTION("indeterminate && indeterminate returns indeterminate")
        {
            auto result = tribool(indeterminate) && tribool(indeterminate);

            REQUIRE((result == indeterminate));
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("tribool operator || ( const tribool&, const tribool& )")
    {
        SECTION("false || false returns false")
        {
            auto result = tribool(false) || tribool(false);

            REQUIRE(result == false);
        }

        SECTION("false || true returns true")
        {
            auto result = tribool(false) || tribool(true);

            REQUIRE(result == true);
        }

        SECTION("true || true returns true")
        {
            auto result = tribool(true) || tribool(true);

            REQUIRE(result == true);
        }

        SECTION("false || indeterminate returns indeterminate")
        {
            auto result = tribool(false) || tribool(indeterminate);

            REQUIRE((result == indeterminate));
        }

        SECTION("true || indeterminate returns true")
        {
            auto result = tribool(true) || tribool(indeterminate);

            REQUIRE((result == true));
        }

        SECTION("indeterminate || indeterminate returns indeterminate")
        {
            auto result = tribool(indeterminate) || tribool(indeterminate);

            REQUIRE((result == indeterminate));
        }
    }

} // namespace test
} // namespace glue
