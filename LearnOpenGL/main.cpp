#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <glad2/glad2.h>
#include <GLFW/glfw3.h>

#include <spdlog/fmt/xchar.h>
#include <string_view>

#include <utils/shaders.h>

#include <iostream>

void show_info(std::wstring_view msg, std::wstring_view title = L"Info")
{
    MessageBoxW(0, msg.data(), title.data(), MB_OK);
}
void show_error(std::wstring_view msg, std::wstring_view title = L"Error")
{
    MessageBoxW(0, msg.data(), title.data(), MB_OK);
}

void show_info(std::string_view msg, std::string_view title = "Info")
{
    MessageBoxA(0, msg.data(), title.data(), MB_OK);
}
void show_error(std::string_view msg, std::string_view title = "Error")
{
    MessageBoxA(0, msg.data(), title.data(), MB_OK);
}

GLFWwindow* init_opengl(int width, int height);
void cleanup();

void resize_framebuffer(GLFWwindow*, int width, int height);
void process_input(GLFWwindow* w);

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);

char const* source_to_string(GLenum source) noexcept;
char const* type_to_string(GLenum type)noexcept;
char const* severity_to_string(GLenum severity)noexcept;

unsigned int VBO, VAO, EBO;

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ INT nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    GLFWwindow* window = init_opengl(800, 600);

    auto compile_result = shaders::shader_builder{}
        .add_shader("vertex_default.vert", GL_VERTEX_SHADER)
        .add_shader("fragment_default.frag", GL_FRAGMENT_SHADER)
        .compile();

    if (compile_result.has_error)
    {
        show_error(compile_result.error, "Couldn't create default shader");
        return -1;
    }

    shaders::shader_program defaultShader(std::move(compile_result.value));

    //float vertices[] = {
    //    0.5f,  0.5f, 0.0f,  // top right
    //    0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f, // bottom left
    //    -0.5f,  0.5f, 0.0f  // top left 
    //};
    //unsigned int indices[] = {
    //    0, 1, 3,   // first triangle
    //    1, 2, 3    // second triangle
    //};
    float vertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    /*glGenBuffers(1, &EBO);*/
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // This "enables" the attribute pointer to make it available to the shader
    // glEnableVertexAttribArray(x); means that attribute X will be available (layout = x)
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultShader.use();
        glBindVertexArray(VAO);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();

    return 0;
}

GLFWwindow* init_opengl(int width, int height)
{
    // 1. Initializes GLFW
    glfwInit();

    // 2. Tell GLFW that we want OpenGL 4.6 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // 3. Create a window
    GLFWwindow* window = glfwCreateWindow(width, height, "Learn OpenGL", NULL, NULL);

    // 4. Set current thread as OpenGL Context
    glfwMakeContextCurrent(window);

    // 5. Load GLAD2
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        show_error(L"Failed to initialize OpenGL context");
        glfwTerminate();
        abort();
    }

    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // 6. Set Viewport size for OpenGL when rendering
    glViewport(0, 0, width, height);

    // 7. Register callback if the window is resized
    glfwSetFramebufferSizeCallback(window, resize_framebuffer);

    //ShowInfo(fmt::format(L"OpneGL {}.{} loaded.", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version)));

    return window;
}

void cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    gladLoaderUnloadGL();
    glfwTerminate();
}

void resize_framebuffer(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* w)
{
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);
}

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    show_info(
        fmt::format(
            "Type #{}\n"
            "Source: {}\n"
            "Severity: {}\n"
            "{}"
            , id
            , source_to_string(source)
            , severity_to_string(severity)
            , message
        ), type_to_string(type));
}

char const* source_to_string(GLenum source) noexcept
{
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:               return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:       return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION:       return "Application";
    case GL_DEBUG_SOURCE_OTHER:             return "Other";
    }
}

char const* type_to_string(GLenum type)noexcept
{
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
    case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
    case GL_DEBUG_TYPE_MARKER:              return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
    case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
    case GL_DEBUG_TYPE_OTHER:               return "Other";
    }
}

char const* severity_to_string(GLenum severity)noexcept
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:            return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:          return "Medium";
    case GL_DEBUG_SEVERITY_LOW:             return "Low";
    case GL_DEBUG_SEVERITY_NOTIFICATION:    return "Notification";
    }
}