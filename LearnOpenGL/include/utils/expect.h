#pragma once

template<typename error_t, typename value_t>
struct expect final
{
public:
    inline expect(value_t value) noexcept : has_error(false), value(static_cast<value_t&&>(value))
    {}

    inline expect(error_t error) noexcept : has_error(true), error(static_cast<error_t&&>(error))
    {}

    bool const has_error;
    error_t error;
    value_t value;
};