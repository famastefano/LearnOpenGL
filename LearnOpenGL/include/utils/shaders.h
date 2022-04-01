#pragma once

#include <glad2/glad2.h>
#include <utils/expect.h>

#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <type_traits>

namespace shaders
{
    class shader_program
    {
    public:
        explicit shader_program() noexcept;
        explicit shader_program(unsigned int id) noexcept;
        shader_program(shader_program&& other) noexcept;
        shader_program& operator=(shader_program&& rhs) noexcept;
        ~shader_program();

        void use() noexcept;
        
        inline unsigned int id() const noexcept { return _id; }
        inline bool valid() const noexcept { return _id > 0; }

        template<typename T>
        bool set_uniform(std::string_view name, T&& v0) noexcept;
        template<typename T>
        bool set_uniform(std::string_view name, T&& v0, T&& v1) noexcept;
        template<typename T>
        bool set_uniform(std::string_view name, T&& v0, T&& v1, T&& v2) noexcept;
        template<typename T>
        bool set_uniform(std::string_view name, T&& v0, T&& v1, T&& v2, T&& v3) noexcept;

    private:
        unsigned int _id;
    };

    class shader_builder
    {
    public:
        shader_builder& add_shader(std::string_view name, unsigned int type) noexcept;
        expect<std::string, shader_program> compile() noexcept;

    private:
        std::vector<std::pair<std::string, unsigned int>> shaders;
    };

    template<typename T>
    inline bool shader_program::set_uniform(std::string_view name, T&& v0) noexcept
    {
        int loc = glGetUniformLocation(id, name.data());
        if (loc == -1)
            return false;

        if constexpr (std::is_same_v<T, float>)
            glUniform1f(loc, v0);
        else if (std::is_same_v<T, int> || std::is_same_v<T, bool>)
            glUniform1i(loc, (int)v0);
        else if (std::is_same_v<T, unsigned int>)
            glUniform1ui(loc, v0);
        else if (std::is_same_v<T, double>)
            glUniform1d(loc, v0);
        else
            static_assert(std::is_same_v<T, const T>, "The shader doesn't support this type");

        return true;
    }

    template<typename T>
    inline bool shader_program::set_uniform(std::string_view name, T&& v0, T&& v1) noexcept
    {
        int loc = glGetUniformLocation(id, name.data());
        if (loc == -1)
            return false;

        if constexpr (std::is_same_v<T, float>)
            glUniform2f(loc, v0, v1);
        else if (std::is_same_v<T, int> || std::is_same_v<T, bool>)
            glUniform2i(loc, (int)v0, (int)v1);
        else if (std::is_same_v<T, unsigned int>)
            glUniform2ui(loc, v0, v1);
        else if (std::is_same_v<T, double>)
            glUniform2d(loc, v0, v1);
        else
            static_assert(std::is_same_v<T, const T>, "The shader doesn't support this type");

        return true;
    }

    template<typename T>
    inline bool shader_program::set_uniform(std::string_view name, T&& v0, T&& v1, T&& v2) noexcept
    {
        int loc = glGetUniformLocation(id, name.data());
        if (loc == -1)
            return false;

        if constexpr (std::is_same_v<T, float>)
            glUniform3f(loc, v0, v1, v2);
        else if (std::is_same_v<T, int> || std::is_same_v<T, bool>)
            glUniform3i(loc, (int)v0, (int)v1, (int)v2);
        else if (std::is_same_v<T, unsigned int>)
            glUniform3ui(loc, v0, v1, v2);
        else if (std::is_same_v<T, double>)
            glUniform3d(loc, v0, v1, v2);
        else
            static_assert(std::is_same_v<T, const T>, "The shader doesn't support this type");

        return true;
    }

    template<typename T>
    inline bool shader_program::set_uniform(std::string_view name, T&& v0, T&& v1, T&& v2, T&& v3) noexcept
    {
        int loc = glGetUniformLocation(id, name.data());
        if (loc == -1)
            return false;

        if constexpr (std::is_same_v<T, float>)
            glUniform4f(loc, v0, v1, v2, v3);
        else if (std::is_same_v<T, int> || std::is_same_v<T, bool>)
            glUniform4i(loc, (int)v0, (int)v1, (int)v2, (int)v3);
        else if (std::is_same_v<T, unsigned int>)
            glUniform4ui(loc, v0, v1, v2, v3);
        else if (std::is_same_v<T, double>)
            glUniform4d(loc, v0, v1, v2, v3);
        else
            static_assert(std::is_same_v<T, const T>, "The shader doesn't support this type");

        return true;
    }

}