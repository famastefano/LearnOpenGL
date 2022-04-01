#include <utils/shaders.h>

#include <fstream>
#include <algorithm>

std::string load_shader(std::string_view name) noexcept
{
    constexpr std::size_t read_size = 4096;

    std::string path = "shaders\\" + std::string(name);
    auto stream = std::ifstream(path.data());

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

std::string get_shader_log(unsigned int id) noexcept
{
    constexpr int size = 512;
    int iteration = 1;
    std::string log;
    int len;
    do
    {
        log.resize(size * iteration++);
        glGetShaderInfoLog(id, log.size(), &len, log.data());
    } while (len == log.size());
    log.resize(len);
    return log;
}

namespace shaders
{
    shader_builder& shader_builder::add_shader(std::string_view name, unsigned int type) noexcept
    {
        shaders.emplace_back(load_shader(name), type);
        return *this;
    }

    expect<std::string, shader_program> shader_builder::compile() noexcept
    {
        std::vector<unsigned int> compiled;
        compiled.reserve(shaders.size());

        std::string errors;

        for (auto const& [text, type] : shaders)
        {
            char const* const src = text.c_str();
            
            unsigned int id = glCreateShader(type);
            glShaderSource(id, 1, &src, NULL);
            glCompileShader(id);
            
            int ok;
            glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
            if (!ok)
            {
                errors.append(get_shader_log(id));
                errors.append("\n");
            }
            else
            {
                compiled.emplace_back(id);
            }
        }

        unsigned int program{};
        if (errors.empty())
        {
            program = glCreateProgram();

            for (unsigned int id : compiled)
                glAttachShader(program, id);

            glLinkProgram(program);

            int ok;
            glGetProgramiv(program, GL_LINK_STATUS, &ok);
            if (!ok)
                errors.append(get_shader_log(program));
        }

        for (unsigned int id : compiled)
            glDeleteShader(id);

        if (errors.empty())
            return shader_program(program);
        
        return errors;
    }
    shader_program::shader_program() noexcept : shader_program(0)
    {
    }

    shader_program::shader_program(unsigned int id) noexcept : _id(id)
    {
    }

    shader_program::shader_program(shader_program&& other) noexcept : shader_program(other._id)
    {
        other._id = 0;
    }

    shader_program& shader_program::operator=(shader_program&& rhs) noexcept
    {
        if(valid())
            glDeleteProgram(_id);

        _id = rhs._id;
        rhs._id = 0;

        return *this;
    }

    shader_program::~shader_program()
    {
        if(valid())
            glDeleteProgram(_id);
    }

    void shader_program::use() noexcept
    {
        if(valid())
            glUseProgram(_id);
    }
}