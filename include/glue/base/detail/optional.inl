#ifndef GLUE_DETAIL_OPTIONAL_INL
#define GLUE_DETAIL_OPTIONAL_INL

//=============================================================================
// class : detail::optional_base<T,true>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr glue::detail::optional_base<T, true>::optional_base(nullopt_t) noexcept
    : m_storage{}, m_engaged{false}
{}

template <typename T>
template <typename... Args>
inline constexpr glue::detail::optional_base<T, true>::optional_base(
    in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
    : m_storage(in_place, std::forward<Args>(args)...), m_engaged{true}
{}

//-----------------------------------------------------------------------------
// Protected Observers
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr T* glue::detail::optional_base<T, true>::val() noexcept
{
    return &m_storage.something;
}

template <typename T>
inline constexpr const T* glue::detail::optional_base<T, true>::val() const noexcept
{
    return &m_storage.something;
}

template <typename T>
inline constexpr bool glue::detail::optional_base<T, true>::contains_value() const noexcept
{
    return m_engaged;
}

//-----------------------------------------------------------------------------
// Protected Modifiers
//-----------------------------------------------------------------------------

template <typename T>
template <typename... Args>
inline void glue::detail::optional_base<T, true>::construct(Args&&... args)
{
    m_storage.something = T(std::forward<Args>(args)...);
    m_engaged = true;
}

template <typename T>
inline void glue::detail::optional_base<T, true>::destruct()
{
    m_engaged = false;
}

//=============================================================================
// class : detail::optional_base<T,false>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor
//-----------------------------------------------------------------------------

template <typename T>
inline glue::detail::optional_base<T, false>::optional_base(nullopt_t) noexcept
    : m_storage{}, m_engaged{false}
{}

template <typename T>
template <typename... Args>
inline glue::detail::optional_base<T, false>::optional_base(in_place_t, Args&&... args) noexcept(
    std::is_nothrow_constructible<T, Args...>::value)
    : m_storage(in_place, std::forward<Args>(args)...), m_engaged{true}
{}

//-----------------------------------------------------------------------------

template <typename T>
inline glue::detail::optional_base<T, false>::~optional_base() noexcept(
    std::is_nothrow_destructible<T>::value)
{
    if (m_engaged) { m_storage.something.~T(); }
}

//-----------------------------------------------------------------------------
// Protected Observers
//-----------------------------------------------------------------------------

template <typename T>
inline T* glue::detail::optional_base<T, false>::val() noexcept
{
    return &m_storage.something;
}

template <typename T>
inline const T* glue::detail::optional_base<T, false>::val() const noexcept
{
    return &m_storage.something;
}

template <typename T>
inline bool glue::detail::optional_base<T, false>::contains_value() const noexcept
{
    return m_engaged;
}

//-----------------------------------------------------------------------------
// Protected Modifiers
//-----------------------------------------------------------------------------

template <typename T>
template <typename... Args>
inline void glue::detail::optional_base<T, false>::construct(Args&&... args)
{
    BIT_ASSERT(!m_engaged);

    new (&m_storage.something) T(std::forward<Args>(args)...);
    m_engaged = true;
}

template <typename T>
inline void glue::detail::optional_base<T, false>::destruct()
{
    BIT_ASSERT(m_engaged);

    if (m_engaged)
    {
        m_storage.something.~T();
        m_engaged = false;
    }
}

//=============================================================================
// class : optional
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor / Assignment
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr glue::optional<T>::optional() noexcept : base_type{nullopt}
{}

template <typename T>
inline constexpr glue::optional<T>::optional(nullopt_t) noexcept : optional{}
{}

//-----------------------------------------------------------------------------

template <typename T>
inline glue::optional<T>::optional(
    enable_overload_if_t<std::is_copy_constructible<T>::value, const optional&> other)
    : optional{}
{
    if (other.has_value()) { base_type::construct(*other); }
}

template <typename T>
inline glue::optional<T>::optional(
    enable_overload_if_t<std::is_move_constructible<T>::value, optional&&> other)
    : optional{}
{
    if (other.has_value()) { base_type::construct(std::move(*other)); }
}

//-----------------------------------------------------------------------------

template <typename T>
template <typename U, std::enable_if_t<glue::detail::optional_is_copy_convertible<T, U>::value &&
                                       std::is_convertible<const U&, T>::value>*>
inline glue::optional<T>::optional(const optional<U>& other) : optional{}
{
    if (other.has_value()) { base_type::construct(*other); }
}

template <typename T>
template <typename U, std::enable_if_t<glue::detail::optional_is_copy_convertible<T, U>::value &&
                                       !std::is_convertible<const U&, T>::value>*>
inline glue::optional<T>::optional(const optional<U>& other) : optional{}
{
    if (other.has_value()) { base_type::construct(*other); }
}

//-----------------------------------------------------------------------------

template <typename T>
template <typename U, std::enable_if_t<glue::detail::optional_is_move_convertible<T, U>::value &&
                                       std::is_convertible<U&&, T>::value>*>
inline glue::optional<T>::optional(optional<U>&& other) : optional{}
{
    if (other.has_value()) { base_type::construct(std::move(*other)); }
}
template <typename T>
template <typename U, std::enable_if_t<glue::detail::optional_is_move_convertible<T, U>::value &&
                                       !std::is_convertible<U&&, T>::value>*>
inline glue::optional<T>::optional(optional<U>&& other) : optional{}
{
    if (other.has_value()) { base_type::construct(std::move(*other)); }
}

template <typename T>
template <typename... Args, typename>
inline constexpr glue::optional<T>::optional(in_place_t, Args&&... args)
    : base_type{in_place, std::forward<Args>(args)...}
{}

template <typename T>
template <typename U, typename... Args, typename>
inline constexpr glue::optional<T>::optional(in_place_t, std::initializer_list<U> ilist,
                                             Args&&... args)
    : base_type{in_place, ilist, std::forward<Args>(args)...}
{}

template <typename T>
template <typename U, std::enable_if_t<glue::detail::optional_is_value_convertible<T, U>::value &&
                                       std::is_convertible<U&&, T>::value>*>
inline constexpr glue::optional<T>::optional(U&& value)
    : base_type{in_place, std::forward<U>(value)}
{}

template <typename T>
template <typename U, std::enable_if_t<glue::detail::optional_is_value_convertible<T, U>::value &&
                                       !std::is_convertible<U&&, T>::value>*>
inline constexpr glue::optional<T>::optional(U&& value)
    : base_type{in_place, std::forward<U>(value)}
{}

//-----------------------------------------------------------------------------

template <typename T>
inline glue::optional<T>& glue::optional<T>::operator=(nullopt_t)
{
    base_type::destruct();
    return (*this);
}

template <typename T>
inline glue::optional<T>& glue::optional<T>::operator=(
    enable_overload_if_t<std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value,
                         const optional&>
        other)
{
    if (has_value() && other.has_value()) { (*base_type::val()) = (*other); }
    else if (has_value())
    {
        base_type::destruct();
    }
    else if (other.has_value())
    {
        base_type::construct(*other);
    }

    return (*this);
}

template <typename T>
inline glue::optional<T>& glue::optional<T>::operator=(
    enable_overload_if_t<std::is_move_constructible<T>::value && std::is_move_assignable<T>::value,
                         optional&&>
        other)
{
    if (has_value() && other.has_value()) { (*base_type::val()) = std::move(*other); }
    else if (has_value())
    {
        base_type::destruct();
    }
    else if (other.has_value())
    {
        base_type::construct(std::move(*other));
    }

    return (*this);
}

template <typename T>
template <typename U, typename>
inline glue::optional<T>& glue::optional<T>::operator=(U&& value)
{
    if (has_value()) { (*base_type::val()) = std::forward<U>(value); }
    else
    {
        base_type::construct(std::forward<U>(value));
    }
    return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr glue::optional<T>::operator bool() const noexcept
{
    return base_type::contains_value();
}

template <typename T>
inline constexpr bool glue::optional<T>::has_value() const noexcept
{
    return base_type::contains_value();
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr typename glue::optional<T>::value_type* glue::optional<T>::operator->() noexcept
{
    BIT_ASSERT(has_value(), "optional must be engaged!");

    return base_type::val();
}

template <typename T>
inline constexpr const typename glue::optional<T>::value_type*
glue::optional<T>::operator->() const noexcept
{
    BIT_ASSERT(has_value(), "optional must be engaged!");

    return base_type::val();
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr typename glue::optional<T>::value_type& glue::optional<T>::operator*() & noexcept
{
    BIT_ASSERT(has_value(), "optional must be engaged!");

    return *base_type::val();
}

template <typename T>
inline constexpr typename glue::optional<T>::value_type&& glue::optional<T>::operator*() && noexcept
{
    BIT_ASSERT(has_value(), "optional must be engaged!");

    return std::move(*base_type::val());
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr const typename glue::optional<T>::value_type&
glue::optional<T>::operator*() const& noexcept
{
    BIT_ASSERT(has_value(), "optional must be engaged!");

    return *base_type::val();
}

template <typename T>
inline constexpr const typename glue::optional<T>::value_type&&
glue::optional<T>::operator*() const&& noexcept
{
    BIT_ASSERT(has_value(), "optional must be engaged!");

    return std::move(*base_type::val());
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr typename glue::optional<T>::value_type& glue::optional<T>::value() &
{
    if (bool(*this)) { return *base_type::val(); }
    throw bad_optional_access{};
}

template <typename T>
inline constexpr const typename glue::optional<T>::value_type& glue::optional<T>::value() const&
{
    if (bool(*this)) { return *base_type::val(); }
    throw bad_optional_access{};
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr typename glue::optional<T>::value_type&& glue::optional<T>::value() &&
{
    if (bool(*this)) { return std::move(*base_type::val()); }
    throw bad_optional_access{};
}

template <typename T>
inline constexpr const typename glue::optional<T>::value_type&& glue::optional<T>::value() const&&
{
    if (bool(*this)) { return std::move(*base_type::val()); }
    throw bad_optional_access{};
}

//-----------------------------------------------------------------------------

template <typename T>
template <typename U>
inline constexpr typename glue::optional<T>::value_type
glue::optional<T>::value_or(U&& default_value) const&
{
    return bool(*this) ? (*base_type::val()) : std::forward<U>(default_value);
}

template <typename T>
template <typename U>
inline constexpr typename glue::optional<T>::value_type
glue::optional<T>::value_or(U&& default_value) &&
{
    return bool(*this) ? (*base_type::val()) : std::forward<U>(default_value);
}

//-----------------------------------------------------------------------------
// Monadic Functionality
//-----------------------------------------------------------------------------

template <typename T>
template <typename Fn, typename>
inline glue::invoke_result_t<Fn, const T&> glue::optional<T>::flat_map(Fn&& fn) const
{
    if (has_value()) return invoke(std::forward<Fn>(fn), **this);
    return nullopt;
}

template <typename T>
template <typename Fn, typename>
inline glue::optional<glue::invoke_result_t<Fn, const T&>> glue::optional<T>::map(Fn&& fn) const
{
    if (has_value()) return make_optional(invoke(std::forward<Fn>(fn), **this));
    return nullopt;
}

//-----------------------------------------------------------------------------

template <typename T>
template <typename U>
inline glue::optional<std::decay_t<U>> glue::optional<T>::and_then(U&& u) const
{
    if (has_value()) return make_optional(std::forward<U>(u));
    return nullopt;
}

template <typename T>
template <typename U>
inline glue::optional<std::decay_t<U>> glue::optional<T>::or_else(U&& u) const
{
    if (!has_value()) return make_optional(std::forward<U>(u));
    return nullopt;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename T>
inline void glue::optional<T>::swap(optional& other)
{
    using std::swap;

    if (has_value() && other.has_value()) { swap(*base_type::val(), *other); }
    else if (has_value())
    {
        other = std::move(*this);
        reset(); // leave this unengaged
    }
    else if (other.has_value())
    {
        *this = std::move(other);
        other.reset(); // leave 'other' unengaged
    }
}

template <typename T>
inline void glue::optional<T>::reset() noexcept(std::is_nothrow_destructible<T>::value)
{
    base_type::destruct();
}

//-----------------------------------------------------------------------------

template <typename T>
template <typename... Args>
inline void glue::optional<T>::emplace(Args&&... args)
{
    base_type::destruct();
    base_type::construct(std::forward<Args>(args)...);
}

template <typename T>
template <typename U, typename... Args>
inline void glue::optional<T>::emplace(std::initializer_list<U> ilist, Args&&... args)
{
    base_type::destruct();
    base_type::construct(ilist, std::forward<Args>(args)...);
}

//=============================================================================
// Equality Operators
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr bool glue::operator==(const optional<T>& lhs, const optional<T>& rhs)
{
    if (static_cast<bool>(lhs) != static_cast<bool>(rhs)) return false;
    if (!static_cast<bool>(lhs)) return true;
    return *lhs == *rhs;
}

template <typename T>
inline constexpr bool glue::operator!=(const optional<T>& lhs, const optional<T>& rhs)
{
    if (static_cast<bool>(lhs) != static_cast<bool>(rhs)) return true;
    if (!static_cast<bool>(lhs)) return false;
    return *lhs != *rhs;
}

template <typename T>
inline constexpr bool glue::operator<(const optional<T>& lhs, const optional<T>& rhs)
{
    if (!static_cast<bool>(rhs)) return false;
    if (!static_cast<bool>(lhs)) return true;
    return *lhs < *rhs;
}

template <typename T>
inline constexpr bool glue::operator>(const optional<T>& lhs, const optional<T>& rhs)
{
    if (!static_cast<bool>(lhs)) return false;
    if (!static_cast<bool>(rhs)) return true;
    return *lhs > *rhs;
}

template <typename T>
inline constexpr bool glue::operator<=(const optional<T>& lhs, const optional<T>& rhs)
{
    if (!static_cast<bool>(lhs)) return true;
    if (!static_cast<bool>(rhs)) return false;
    return *lhs <= *rhs;
}

template <typename T>
inline constexpr bool glue::operator>=(const optional<T>& lhs, const optional<T>& rhs)
{
    if (!static_cast<bool>(rhs)) return true;
    if (!static_cast<bool>(lhs)) return false;
    return *lhs >= *rhs;
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr bool glue::operator==(const optional<T>& opt, nullopt_t) noexcept
{
    return !opt;
}

template <typename T>
inline constexpr bool glue::operator==(nullopt_t, const optional<T>& opt) noexcept
{
    return !opt;
}

template <typename T>
inline constexpr bool glue::operator!=(const optional<T>& opt, nullopt_t) noexcept
{
    return static_cast<bool>(opt);
}

template <typename T>
inline constexpr bool glue::operator!=(nullopt_t, const optional<T>& opt) noexcept
{
    return static_cast<bool>(opt);
}

template <typename T>
inline constexpr bool glue::operator<(const optional<T>&, nullopt_t) noexcept
{
    return false;
}

template <typename T>
inline constexpr bool glue::operator<(nullopt_t, const optional<T>& opt) noexcept
{
    return static_cast<bool>(opt);
}

template <typename T>
inline constexpr bool glue::operator>(const optional<T>& opt, nullopt_t) noexcept
{
    return static_cast<bool>(opt);
}

template <typename T>
inline constexpr bool glue::operator>(nullopt_t, const optional<T>&) noexcept
{
    return false;
}

template <typename T>
inline constexpr bool glue::operator<=(const optional<T>& opt, nullopt_t) noexcept
{
    return !opt;
}

template <typename T>
inline constexpr bool glue::operator<=(nullopt_t, const optional<T>&) noexcept
{
    return true;
}

template <typename T>
inline constexpr bool glue::operator>=(const optional<T>&, nullopt_t) noexcept
{
    return true;
}

template <typename T>
inline constexpr bool glue::operator>=(nullopt_t, const optional<T>& opt) noexcept
{
    return !opt;
}

//-----------------------------------------------------------------------------

template <typename T>
inline constexpr bool glue::operator==(const optional<T>& opt, const T& value)
{
    return static_cast<bool>(opt) ? *opt == value : false;
}

template <typename T>
inline constexpr bool glue::operator==(const T& value, const optional<T>& opt)
{
    return static_cast<bool>(opt) ? value == *opt : false;
}

template <typename T>
inline constexpr bool glue::operator!=(const optional<T>& opt, const T& value)
{
    return static_cast<bool>(opt) ? *opt != value : true;
}

template <typename T>
inline constexpr bool glue::operator!=(const T& value, const optional<T>& opt)
{
    return static_cast<bool>(opt) ? value != *opt : true;
}

template <typename T>
inline constexpr bool glue::operator<(const optional<T>& opt, const T& value)
{
    return static_cast<bool>(opt) ? *opt < value : true;
}

template <typename T>
inline constexpr bool glue::operator<(const T& value, const optional<T>& opt)
{
    return static_cast<bool>(opt) ? value < *opt : false;
}

template <typename T>
inline constexpr bool glue::operator>(const optional<T>& opt, const T& value)
{
    return static_cast<bool>(opt) ? *opt > value : false;
}

template <typename T>
inline constexpr bool glue::operator>(const T& value, const optional<T>& opt)
{
    return static_cast<bool>(opt) ? value > *opt : true;
}

template <typename T>
inline constexpr bool glue::operator<=(const optional<T>& opt, const T& value)
{
    return static_cast<bool>(opt) ? *opt <= value : true;
}

template <typename T>
inline constexpr bool glue::operator<=(const T& value, const optional<T>& opt)
{
    return static_cast<bool>(opt) ? value <= *opt : false;
}

template <typename T>
inline constexpr bool glue::operator>=(const optional<T>& opt, const T& value)
{
    return static_cast<bool>(opt) ? *opt >= value : false;
}

template <typename T>
inline constexpr bool glue::operator>=(const T& value, const optional<T>& opt)
{
    return static_cast<bool>(opt) ? value >= *opt : true;
}

//-----------------------------------------------------------------------------
// Non-member functions
//-----------------------------------------------------------------------------

template <typename T>
inline constexpr glue::optional<std::decay_t<T>> glue::make_optional(T&& value)
{
    return optional<std::decay_t<T>>(std::forward<T>(value));
}

template <typename T, typename... Args>
inline constexpr glue::optional<T> glue::make_optional(Args&&... args)
{
    return optional<T>(in_place, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
inline constexpr glue::optional<T> glue::make_optional(std::initializer_list<U> il, Args&&... args)
{
    return optional<T>(in_place, std::forward<std::initializer_list<U>>(il),
                       std::forward<Args>(args)...);
}

template <typename T>
inline void glue::swap(optional<T>& lhs, optional<T>& rhs)
{
    lhs.swap(rhs);
}

//----------------------------------------------------------------------------
// Hashing
//----------------------------------------------------------------------------

template <typename T>
inline constexpr glue::hash_t glue::hash_value(const optional<T>& s) noexcept
{
    if (s) { return hash_value(*s); }
    return static_cast<hash_t>(0);
}

#endif /* GLUE_DETAIL_OPTIONAL_INL */
