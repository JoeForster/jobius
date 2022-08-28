#pragma once

// TODO ENUM HELPERS string conversion, checks, other stuff

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}