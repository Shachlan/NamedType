#ifndef UNDERLYING_FUNCTIONALITIES_HPP
#define UNDERLYING_FUNCTIONALITIES_HPP

#include "crtp.hpp"
#include "named_type_impl.hpp"

#include <functional>
#include <iostream>
#include <memory>

namespace fluent
{

template <typename T>
struct Incrementable : crtp<T, Incrementable>
{
    constexpr T& operator+=(T const& other) { this->underlying().get() += other.get(); return this->underlying(); }
};

template <typename T>
struct Decrementable : crtp<T, Decrementable>
{
  T& operator-=(T const& other) { this->underlying().get() -= other.get(); return this->underlying(); }
};

template <typename T>
struct PreIncrementable : crtp<T, PreIncrementable>
{
    constexpr T& operator++() { ++this->underlying().get(); return this->underlying(); }
};

template <typename T>
struct PostIncrementable : crtp<T, PostIncrementable>
{
    constexpr T operator++(int) { return this->underlying().get()++; }
};

template <typename T>
struct PreDecrementable : crtp<T, PreDecrementable>
{
    constexpr T& operator--() { --this->underlying().get(); return this->underlying(); }
};

template <typename T>
struct PostDecrementable : crtp<T, PostDecrementable>
{
    constexpr T operator--(int) { return this->underlying().get()--; }
};

template <typename T>
struct Addable : crtp<T, Addable>
{
    constexpr T operator+(T const& other) const { return T(this->underlying().get() + other.get()); }
};

template <typename T>
struct Subtractable : crtp<T, Subtractable>
{
    constexpr T operator-(T const& other) const { return T(this->underlying().get() - other.get()); }
};
    
template <typename T>
struct Multiplicable : crtp<T, Multiplicable>
{
    constexpr T operator*(T const& other) const { return T(this->underlying().get() * other.get()); }
};

template <typename T>
struct Divisible : crtp<T, Divisible>
{
    constexpr T operator*(T const& other) const { return T(this->underlying().get() / other.get()); }
};

template <typename T>
struct Negatable : crtp<T, Negatable>
{
    constexpr T operator-() const { return T(-this->underlying().get()); }
};
    
template <typename T>
struct Comparable : crtp<T, Comparable>
{
    constexpr bool operator<(T const& other) const  { return this->underlying().get() < other.get(); }
    constexpr bool operator>(T const& other) const  { return other.get() < this->underlying().get(); }
    constexpr bool operator<=(T const& other) const { return !(*this > other); }
    constexpr bool operator>=(T const& other) const { return !(*this < other); }
    constexpr bool operator==(T const& other) const { return !(*this < other || *this > other); }
    constexpr bool operator!=(T const& other) const { return !(*this == other); }
};

template <typename T>
struct Printable : crtp<T, Printable>
{
    void print(std::ostream& os) const { os << this->underlying().get(); }
};

template <typename Destination>
struct ImplicitlyConvertibleTo
{
    template <typename T>
    struct templ : crtp<T, templ>
    {
        operator Destination() const
        {
            return this->underlying().get();
        }
    };
};

template <typename T, typename Parameter, template<typename> class... Skills>
std::ostream& operator<<(std::ostream& os, NamedType<T, Parameter, Skills...> const& object)
{
    object.print(os);
    return os;
}

template<typename T>
struct Hashable
{
    static constexpr bool is_hashable = true;
};

template<typename NamedType_>
struct FunctionCallable;
    
template <typename T, typename Parameter, template<typename> class... Skills>
struct FunctionCallable<NamedType<T, Parameter, Skills...>> : crtp<NamedType<T, Parameter, Skills...>, FunctionCallable>
{
    constexpr operator T const&() const
    {
        return this->underlying().get();
    }
    constexpr operator T&()
    {
        return this->underlying().get();
    }
};
    
template<typename NamedType_>
struct MethodCallable;
    
template <typename T, typename Parameter, template<typename> class... Skills>
struct MethodCallable<NamedType<T, Parameter, Skills...>> : crtp<NamedType<T, Parameter, Skills...>, MethodCallable>
{
    constexpr T const* operator->() const { return std::addressof(this->underlying().get()); }
    constexpr T* operator->() { return std::addressof(this->underlying().get()); }
};

template<typename NamedType_>
struct Callable : FunctionCallable<NamedType_>, MethodCallable<NamedType_>{};
    
template <typename T>
struct Arithmetic
    : Incrementable<T>
    , PreIncrementable<T>
    , PostIncrementable<T>
    , PreDecrementable<T>
    , PostDecrementable<T>
    , Addable<T>
    , Subtractable<T>
    , Multiplicable<T>
    , Divisible<T>
    , Negatable<T>
    , Comparable<T>
    , Printable<T>
    , Hashable<T>
{
};

} // namespace fluent

namespace std
{
template <typename T, typename Parameter, template<typename> class... Skills>
struct hash<fluent::NamedType<T, Parameter, Skills...>>
{
    using NamedType = fluent::NamedType<T, Parameter, Skills...>;
    using checkIfHashable = typename std::enable_if<NamedType::is_hashable, void>::type;
    
    size_t operator()(fluent::NamedType<T, Parameter, Skills...> const& x) const
    {
        return std::hash<T>()(x.get());
    }
};

}
    

#endif
