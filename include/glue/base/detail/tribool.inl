#ifndef GLUE_DETAIL_TRIBOOL_INL
#define GLUE_DETAIL_TRIBOOL_INL

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

inline constexpr glue::tribool::tribool() : m_state(state::e_indeterminate) {}

inline constexpr glue::tribool::tribool(bool b) : m_state(b ? state::e_true : state::e_false) {}

inline constexpr glue::tribool::tribool(indeterminate_t) : m_state(state::e_indeterminate) {}

//----------------------------------------------------------------------------
// Observers
//----------------------------------------------------------------------------

inline constexpr glue::tribool::operator bool() const noexcept { return m_state == state::e_true; }

inline constexpr glue::tribool glue::tribool::operator!() const noexcept {
    return m_state == state::e_indeterminate ? (*this) : tribool(!bool(*this));
}

//----------------------------------------------------------------------------
// Free Functions
//----------------------------------------------------------------------------

inline constexpr bool glue::indeterminate(const tribool& t, detail::tribool_t) noexcept {
    return t.m_state == tribool::state::e_indeterminate;
}

inline constexpr glue::hash_t glue::hash_value(tribool t) noexcept {
    if (t)
        return static_cast<hash_t>(1);
    else if (!t)
        return static_cast<hash_t>(2);
    return static_cast<hash_t>(0);
}

//----------------------------------------------------------------------------
// Equality Comparisons
//----------------------------------------------------------------------------

inline constexpr bool glue::operator==(const tribool& lhs, const tribool& rhs) noexcept {
    return lhs.m_state == rhs.m_state;
}

inline constexpr bool glue::operator==(indeterminate_t, const tribool& rhs) noexcept {
    return tribool(indeterminate) == rhs;
}

inline constexpr bool glue::operator==(const tribool& lhs, indeterminate_t) noexcept {
    return lhs == tribool(indeterminate);
}

inline constexpr bool glue::operator==(const tribool& lhs, bool rhs) noexcept {
    return lhs == tribool(rhs);
}

inline constexpr bool glue::operator==(bool lhs, const tribool& rhs) noexcept {
    return tribool(lhs) == rhs;
}

//----------------------------------------------------------------------------

inline constexpr bool glue::operator!=(const tribool& lhs, const tribool& rhs) noexcept {
    return !(lhs == rhs);
}

inline constexpr bool glue::operator!=(indeterminate_t, const tribool& rhs) noexcept {
    return tribool(indeterminate) != rhs;
}

inline constexpr bool glue::operator!=(const tribool& lhs, indeterminate_t) noexcept {
    return lhs != tribool(indeterminate);
}

inline constexpr bool glue::operator!=(const tribool& lhs, bool rhs) noexcept {
    return lhs != tribool(rhs);
}

inline constexpr bool glue::operator!=(bool lhs, const tribool& rhs) noexcept {
    return tribool(lhs) != rhs;
}

//----------------------------------------------------------------------------
// Triboolean Binary Operations
//----------------------------------------------------------------------------

inline constexpr glue::tribool glue::operator&&(const tribool& lhs, const tribool& rhs) noexcept {
    if (lhs.m_state == tribool::state::e_indeterminate ||
        rhs.m_state == tribool::state::e_indeterminate) {
        return tribool(indeterminate);
    }

    return tribool(bool(lhs.m_state) && bool(rhs.m_state));
}

inline constexpr glue::tribool glue::operator||(const tribool& lhs, const tribool& rhs) noexcept {
    if (lhs.m_state == rhs.m_state) return lhs;

    if (lhs.m_state == tribool::state::e_true) return tribool(true);
    if (rhs.m_state == tribool::state::e_true) return tribool(true);

    return tribool(indeterminate);
}

#endif /* GLUE_DETAIL_TRIBOOL_INL */
