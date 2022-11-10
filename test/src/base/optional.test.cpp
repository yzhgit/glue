//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/optional.hpp"

#include <catch2/catch.hpp>

namespace glue {
namespace test {
    namespace {

        class dtor_test
        {
        public:
            explicit dtor_test(bool& is_called) : m_is_called(is_called)
            {}

            ~dtor_test()
            {
                m_is_called = true;
            }

        private:
            bool& m_is_called;
        };
    } // anonymous namespace

    //=============================================================================
    // class : optional
    //=============================================================================

    //-----------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-----------------------------------------------------------------------------

    TEST_CASE("optional::optional()", "[ctor]")
    {
        auto opt = optional<int>();

        SECTION("Has no value")
        {
            REQUIRE_FALSE(static_cast<bool>(opt));
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::optional( nullopt_t )", "[ctor]")
    {
        auto opt = optional<int>(nullopt);

        SECTION("Has no value")
        {
            REQUIRE_FALSE(static_cast<bool>(opt));
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::optional( const optional& )", "[ctor]")
    {
        SECTION("Copying a null optional")
        {
            auto original = optional<int>();
            auto opt = original;

            SECTION("Has no value")
            {
                REQUIRE_FALSE(static_cast<bool>(opt));
            }
        }

        SECTION("Copying a non-null optional")
        {
            auto value = 42;
            auto original = optional<int>{value};
            auto opt = original;

            SECTION("Has a value")
            {
                REQUIRE(static_cast<bool>(opt));
            }

            SECTION("Value is the same as original")
            {
                REQUIRE(opt.value() == value);
            }
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::optional( optional&& )", "[ctor]")
    {
        SECTION("Copying a null optional")
        {
            auto original = optional<int>();
            auto opt = std::move(original);

            SECTION("Has no value")
            {
                REQUIRE_FALSE(static_cast<bool>(opt));
            }
        }

        SECTION("Copying a non-null optional")
        {
            auto value = 42;
            auto original = optional<int>(value);
            auto opt = std::move(original);

            SECTION("Has a value")
            {
                REQUIRE(static_cast<bool>(opt));
            }

            SECTION("Value is the same as original")
            {
                REQUIRE(opt.value() == value);
            }
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::optional( const value_type& )", "[ctor]")
    {
        auto value = 42;
        auto opt = optional<int>(value);

        SECTION("Has a value")
        {
            REQUIRE(static_cast<bool>(opt));
        }

        SECTION("Value is the same as original")
        {
            REQUIRE(opt.value() == value);
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::optional( value_type&& )", "[ctor]")
    {
        auto value = 42;
        auto expectedValue = 42;
        auto opt = optional<int>(std::move(value));

        SECTION("Has a value")
        {
            REQUIRE(static_cast<bool>(opt));
        }

        SECTION("Value is the same as original")
        {
            REQUIRE(opt.value() == expectedValue);
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::~optional()", "[dtor]")
    {
        bool is_called = false;
        auto dtor = dtor_test(is_called);
        {
            auto opt = optional<dtor_test>(dtor);
        }

        SECTION("Destructor gets called")
        {
            REQUIRE(is_called);
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator=( nullopt_t )", "[assignment]")
    {
        SECTION("Assigning over non-null value")
        {
            auto is_called = false;
            auto dtor = dtor_test(is_called);
            auto opt = optional<dtor_test>(dtor);
            opt = nullopt;

            SECTION("Calls destructor on previous value")
            {
                REQUIRE(is_called);
            }

            SECTION("Converts to null")
            {
                REQUIRE_FALSE(static_cast<bool>(opt));
            }
        }

        SECTION("Assigning over null value")
        {
            auto opt = optional<int>(nullopt);

            SECTION("Converts to null")
            {
                REQUIRE_FALSE(static_cast<bool>(opt));
            }
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator=( const optional& )", "[assignment]")
    {
        SECTION("Constructs unconstructed optional")
        {
            // TODO(bitwizeshift): Add unit tests
        }

        SECTION("Copy-assigns constructed optional")
        {
            // TODO(bitwizeshift): Add unit tests
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator=( optional&& )", "[assignment]")
    {
        SECTION("Constructs unconstructed optional")
        {
            // TODO(bitwizeshift): Add unit tests
        }

        SECTION("Destructs previous optional before constructing new one")
        {
            // TODO(bitwizeshift): Add unit tests
        }
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator=( U&& )", "[assignment]")
    {}

    //-----------------------------------------------------------------------------
    // Observers
    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator->()", "[observers]")
    {
        // TODO(bitwizeshift): Add unit tests
    }

    TEST_CASE("optional::operator->() const", "[observers]")
    {
        // TODO(bitwizeshift): Add unit tests
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator*() &", "[observers]")
    {
        // TODO(bitwizeshift): Add unit tests
    }

    TEST_CASE("optional::operator*() &&", "[observers]")
    {
        // TODO(bitwizeshift): Add unit tests
    }

    TEST_CASE("optional::operator*() const &", "[observers]")
    {
        // TODO(bitwizeshift): Add unit tests
    }

    TEST_CASE("optional::operator*() const &&", "[observers]")
    {
        // TODO(bitwizeshift): Add unit tests
    }

    //-----------------------------------------------------------------------------

    TEST_CASE("optional::operator bool()", "[observers]")
    {
        SECTION("Optional does not contain a value")
        {
            optional<int> op;

            SECTION("Is implicitly false")
            {
                REQUIRE_FALSE(op);
            }
        }
        SECTION("Optional contains a value")
        {
            optional<int> op = 42;

            SECTION("Is implicitly true")
            {
                REQUIRE(op);
            }
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::value() &", "[observers]")
    {
        SECTION("Optional does not contain value")
        {
            SECTION("Throws bad_optional_access")
            {
                // Arrange
                optional<int> op;

                // Act & Assert
                REQUIRE_THROWS_AS(op.value(), bad_optional_access);
            }
        }
        SECTION("Optional contains a value")
        {
            SECTION("Returns value")
            {
                int value = 42;
                optional<int> op = value;

                REQUIRE(op.value() == value);
            }

            SECTION("Returns a value by lvalue-reference")
            {
                int value = 42;
                optional<int> op = value;

                REQUIRE((std::is_same<int&, decltype(op.value())>::value));
            }
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::value() const &", "[observers]")
    {
        SECTION("Optional does not contain value")
        {
            SECTION("Throws bad_optional_access")
            {
                optional<int> const op;

                REQUIRE_THROWS_AS(op.value(), bad_optional_access);
            }
        }
        SECTION("Optional contains a value")
        {
            SECTION("Returns value")
            {
                int value = 42;
                optional<int> const op = value;

                REQUIRE(op.value() == value);
            }

            SECTION("Returns a value by const lvalue-reference")
            {
                int value = 42;
                optional<int> const op = value;

                // Act & Assert
                REQUIRE((std::is_same<int const&, decltype(op.value())>::value));
            }
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::value() &&", "[observers]")
    {
        SECTION("Optional does not contain value")
        {
            SECTION("Throws bad_optional_access")
            {
                optional<int> op;

                REQUIRE_THROWS_AS(std::move(op).value(), bad_optional_access); // NOLINT
            }
        }
        SECTION("Optional contains a value")
        {
            SECTION("Returns value")
            {
                int value = 42;
                optional<int> op = value;

                REQUIRE(std::move(op).value() == value); // NOLINT
            }

            SECTION("Returns a value by rvalue-reference")
            {
                int value = 42;
                optional<int> op = value;

                REQUIRE((std::is_same<int&&, decltype(std::move(op).value())>::value));
            }
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::value() const &&", "[observers]")
    {
        SECTION("Optional does not contain value")
        {
            SECTION("Throws bad_optional_access")
            {
                // Arrange
                optional<int> const op;

                // Act & Assert
                REQUIRE_THROWS_AS(std::move(op).value(), bad_optional_access); // NOLINT
            }
        }
        SECTION("Optional contains a value")
        {
            SECTION("Returns value")
            {
                // Arrange
                int value = 42;
                optional<int> const op = value;

                // Act & Assert
                REQUIRE(std::move(op).value() == value); // NOLINT
            }

            SECTION("Returns a value by const rvalue-reference")
            {
                // Arrange
                int value = 42;
                optional<int> const op = value;

                // Act & Assert
                REQUIRE((std::is_same<int const&&, decltype(std::move(op).value())>::value));
            }
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::value_or( U&& ) const &", "[observers]")
    {
        SECTION("Optional does not contain a value")
        {
            auto opt = optional<int>();

            REQUIRE(opt.value_or(42) == 42);
        }

        SECTION("Optional contains a value")
        {
            auto opt = optional<int>(32);

            REQUIRE(opt.value_or(42) == 32);
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::value_or( U&& ) &&", "[observers]")
    {
        SECTION("Optional does not contain a value")
        {
            auto opt = optional<int>();

            REQUIRE(std::move(opt).value_or(42) == 42); // NOLINT
        }

        SECTION("Optional contains a value")
        {
            auto opt = optional<int>(32);

            REQUIRE(std::move(opt).value_or(42) == 32); // NOLINT
        }
    }

    //----------------------------------------------------------------------------
    // Modifiers
    //----------------------------------------------------------------------------

    TEST_CASE("optional::swap( optional<T>& )", "[modifiers]")
    {
        SECTION("Both optionals are null")
        {
            auto op1 = optional<int>();
            auto op2 = optional<int>();

            op1.swap(op2);

            SECTION("Values swapped")
            {
                SECTION("op1 is null")
                {
                    REQUIRE(op1 == nullopt);
                }
                SECTION("op2 is null")
                {
                    REQUIRE(op2 == nullopt);
                }
            }
        }

        SECTION("Both optionals are non-null")
        {
            auto value1 = 32;
            auto value2 = 64;
            auto op1 = optional<int>(value1);
            auto op2 = optional<int>(value2);

            op1.swap(op2);

            SECTION("Values swapped")
            {
                SECTION("op1 contains op2's value")
                {
                    REQUIRE(op1.value() == value2);
                }
                SECTION("op2 contains op1's value")
                {
                    REQUIRE(op2.value() == value1);
                }
            }

            SECTION("Optionals are non-null")
            {
                SECTION("op1 is non-null")
                {
                    REQUIRE(static_cast<bool>(op1));
                }
                SECTION("op2 is non-null")
                {
                    REQUIRE(static_cast<bool>(op2));
                }
            }
        }

        SECTION("(*this) optional is null")
        {
            auto value1 = 32;
            auto value2 = nullopt;
            auto op1 = optional<int>(value1);
            auto op2 = optional<int>(value2);

            op1.swap(op2);

            SECTION("Values swapped")
            {
                SECTION("op1 contains op2's value")
                {
                    REQUIRE(!op1.has_value());
                }
                SECTION("op2 contains op1's value")
                {
                    REQUIRE(op2 == value1);
                }
            }
        }

        SECTION("other optional is null")
        {
            // Arrange
            auto value1 = nullopt;
            auto value2 = 32;
            auto op1 = optional<int>(value1);
            auto op2 = optional<int>(value2);

            // Act
            op1.swap(op2);

            SECTION("Values swapped")
            {
                SECTION("op1 contains op2's value")
                {
                    REQUIRE(op1 == value2);
                }
                SECTION("op2 contains op1's value")
                {
                    REQUIRE(!op2.has_value());
                }
            }
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::emplace( Args&&... )", "[modifiers]")
    {
        SECTION("Original optional is null")
        {
            // TODO(bitwizeshift): Add unit tests
        }

        SECTION("Original optional is non-null")
        {
            // TODO(bitwizeshift): Add unit tests
        }
    }

    //----------------------------------------------------------------------------

    TEST_CASE("optional::emplace( std::initializer_list<U>, Args&&... )", "[modifiers]")
    {
        SECTION("Original optional is null")
        {
            // TODO(bitwizeshift): Add unit tests
        }

        SECTION("Original optional is non-null")
        {
            // TODO(bitwizeshift): Add unit tests
        }
    }

} // namespace test
} // namespace glue
