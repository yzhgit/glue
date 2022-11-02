//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/Identifier.h"

#include "glue/base/StringPool.h"

namespace glue
{

Identifier::Identifier() noexcept {}
Identifier::~Identifier() noexcept {}

Identifier::Identifier(const Identifier& other) noexcept : m_name(other.m_name) {}

Identifier::Identifier(Identifier&& other) noexcept : m_name(std::move(other.m_name)) {}

Identifier& Identifier::operator=(Identifier&& other) noexcept
{
    m_name = std::move(other.m_name);
    return *this;
}

Identifier& Identifier::operator=(const Identifier& other) noexcept
{
    m_name = other.m_name;
    return *this;
}

Identifier::Identifier(const String& nm) : m_name(StringPool::getGlobalPool().getPooledString(nm))
{
    // An Identifier cannot be created from an empty string!
    GLUE_ASSERT(nm.isNotEmpty());
}

Identifier::Identifier(const char* nm) : m_name(StringPool::getGlobalPool().getPooledString(nm))
{
    // An Identifier cannot be created from an empty string!
    GLUE_ASSERT(nm != nullptr && nm[0] != 0);
}

Identifier::Identifier(String::CharPointerType start, String::CharPointerType end)
    : m_name(StringPool::getGlobalPool().getPooledString(start, end))
{
    // An Identifier cannot be created from an empty string!
    GLUE_ASSERT(start < end);
}

Identifier Identifier::null;

bool Identifier::isValidIdentifier(const String& possibleIdentifier) noexcept
{
    return possibleIdentifier.isNotEmpty() &&
           possibleIdentifier.containsOnly(
               "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-:#@$%");
}

} // namespace glue
