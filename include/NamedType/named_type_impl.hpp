#ifndef named_type_impl_h
#define named_type_impl_h

#include <tuple>
#include <type_traits>
#include <utility>

// Enable empty base class optimization with multiple inheritance on Visual Studio.
#if defined(_MSC_VER) && _MSC_VER >= 1910
#    define FLUENT_EBCO __declspec(empty_bases)
#else
#    define FLUENT_EBCO
#endif

#if defined(__clang__) || defined(__GNUC__)
#   define IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_BEGIN                                                                \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Weffc++\"")
#   define IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_END _Pragma("GCC diagnostic pop")
#else
#   define IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_BEGIN /* Nothing */
#   define IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_END   /* Nothing */
#endif

namespace fluent
{

template <typename T>
using IsNotReference = typename std::enable_if<!std::is_reference<T>::value, void>::type;

template <typename T, typename Parameter, template <typename> class... Skills>
class FLUENT_EBCO NamedType : public Skills<NamedType<T, Parameter, Skills...>>...
{
public:
    using UnderlyingType = T;

    // constructor
    template <typename T_ = T, typename = std::enable_if<std::is_default_constructible<T>::value, void>>
    constexpr NamedType() noexcept(std::is_nothrow_constructible<T>::value) : value_()
    {
    }

    explicit constexpr NamedType(T const& value) noexcept(std::is_nothrow_copy_constructible<T>::value) : value_(value)
    {
    }

    template <typename T_ = T, typename = IsNotReference<T_>>
    explicit constexpr NamedType(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value)
        : value_(std::move(value))
    {
    }

    // get
    constexpr T& get() noexcept
    {
        return value_;
    }

    constexpr std::remove_reference_t<T> const& get() const noexcept
    {
        return value_;
    }

    // conversions
    using ref = NamedType<T&, Parameter, Skills...>;
    operator ref()
    {
        return ref(value_);
    }

    struct argument
    {
       NamedType operator=(T&& value) const
       {
           IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_BEGIN

           return NamedType(std::forward<T>(value));

           IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_END
       }
        template <typename U>
        NamedType operator=(U&& value) const
        {
            IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_BEGIN

            return NamedType(std::forward<U>(value));

            IGNORE_SHOULD_RETURN_REFERENCE_TO_THIS_END
        }

        argument() = default;
        argument(argument const&) = delete;
        argument(argument&&) = delete;
        argument& operator=(argument const&) = delete;
        argument& operator=(argument&&) = delete;
    };
    constexpr const T &operator*() const & {
      return value_;
    }
    constexpr T &operator*() & {
      return value_;
    }
    constexpr const T &&operator*() const && {
      return std::move(value_);
    }
    constexpr T &&operator*() && {
      return std::move(value_);
    }

private:
    T value_;
};

template <template <typename T> class StrongType, typename T>
constexpr StrongType<T> make_named(T const& value)
{
    return StrongType<T>(value);
}

namespace details {
template <class F, class... Ts>
struct AnyOrderCallable{
   F f;
   template <class... Us>
   auto operator()(Us&&...args) const
   {
       static_assert(sizeof...(Ts) == sizeof...(Us), "Passing wrong number of arguments");
       auto x = std::make_tuple(std::forward<Us>(args)...);
       return f(std::move(std::get<Ts>(x))...);
   }
};
} //namespace details

// EXPERIMENTAL - CAN BE CHANGED IN THE FUTURE. FEEDBACK WELCOME FOR IMPROVEMENTS!
template <class... Args, class F>
auto make_named_arg_function(F&& f)
{
   return details::AnyOrderCallable<F, Args...>{std::forward<F>(f)};
}
} // namespace fluent

#endif /* named_type_impl_h */
