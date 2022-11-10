#ifndef GLUE_DETAIL_INVOKE_INL
#define GLUE_DETAIL_INVOKE_INL

namespace glue {

template <typename Func, typename... Args>
inline constexpr auto invoke(Func&& func, Args&&... args)
#ifndef _MSC_VER
    noexcept(noexcept(glue::detail::invoke_impl(std::forward<Func>(func),
                                                std::forward<Args>(args)...)))
#endif
        -> decltype(detail::invoke_impl(std::forward<Func>(func), std::forward<Args>(args)...))
{
    return detail::invoke_impl(std::forward<Func>(func), std::forward<Args>(args)...);
}

} // namespace glue

#endif /* GLUE_DETAIL_INVOKE_INL */
