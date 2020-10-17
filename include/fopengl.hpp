#ifndef FOPENGL_HPP
#define FOPENGL_HPP
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cassert>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <type_traits>
void GLAPIENTRY MessageCallback(GLenum /*source*/, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {
    if (type != GL_DEBUG_TYPE_ERROR) return;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}
namespace fgl{
    struct window;
    namespace impl{
        static std::unordered_map<GLFWwindow*, window*> winwin_map;
        void keycb(GLFWwindow* window, int key, int scancode, int action, int mods);
        void framebrcb(GLFWwindow* window, int width, int height);
    }
    struct window{
        GLFWwindow* m_window;
        int width, height;
        bool closed;
        window(unsigned int _width, unsigned int _height, const std::string& title) : closed(false){
            if(!glfwInit()){
                std::cout << "glfw_init failed" << "\n";
                std::terminate();
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
            m_window = glfwCreateWindow(_width, _height, title.c_str(), NULL, NULL);
            impl::winwin_map[m_window] = this;
            if (!m_window) {
		        glfwTerminate();
		        assert(false);
	        }
            this->width = _width;
            this->height = _height;
            glfwMakeContextCurrent(m_window);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "gladLoad failed" << "\n";
	        	std::terminate();
	        }

	        if (glfwExtensionSupported("GL_ARB_buffer_storage") == GLFW_FALSE) {
                std::cout << "GL_ARB_buffer_storage failed" << "\n";
	        	std::terminate();
	        }
            glfwSwapInterval(0);
            glfwSetKeyCallback(m_window, impl::keycb);
            GLFWframebuffersizefun g;
            glfwSetFramebufferSizeCallback(m_window, impl::framebrcb);
            this->key_callback = [](window*, int, int){};
            this->framebr_callback = [this](window*, int w, int h){
                this->width = w;
                this->height = h;
                glViewport(0, 0, w, h);
            };
            glDebugMessageCallback(MessageCallback, 0);
        }
        operator bool()const{
            return !glfwWindowShouldClose(m_window);
        }
        void close(){
            closed = true;
            impl::winwin_map.erase(m_window);
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }
        void poll_events(){
            glfwPollEvents();
        }
        void update()const{
            glfwSwapBuffers(m_window);
        }
        void clear(){
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        void set_key_callback(std::function<void(window*, int, int)> kcb){
            this->key_callback = kcb;
        }
        std::function<void(window*, int, int)> key_callback;
        std::function<void(window*, int, int)> framebr_callback;
    };
    namespace impl{
        void keycb(GLFWwindow* window, int key, int scancode, int action, int mods){
            winwin_map[window]->key_callback(winwin_map[window], key, action);
        }
        void framebrcb(GLFWwindow* window, int width, int height){
            winwin_map[window]->framebr_callback(winwin_map[window], width, height);
        }
    }
}
struct vertex_array{
    GLuint vao, vbo;
    size_t vcount;
    template<typename container, typename... Ts>
    vertex_array(const container& data, Ts... ts){
        std::vector<unsigned int> sizes({static_cast<unsigned int>(ts)...});
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(container::value_type) * data.size(), &(*data.begin()), GL_STATIC_DRAW);
        
        std::array<unsigned int, sizeof...(Ts) + 1> offsets;
        std::fill(offsets.begin(), offsets.end(), 0);
        for(size_t i = 1;i < sizes.size() + 1;i++){
            offsets[i] = offsets[i - 1] + sizes[i - 1];
        }
        vcount = data.size() / offsets.back();
        assert((data.size() % offsets.back()) == 0);
        for(auto it = sizes.begin();it != sizes.end();it++){
            glVertexAttribPointer(it - sizes.begin(), *it, GL_FLOAT, GL_FALSE, offsets.back() * sizeof(float), (void*)(offsets[it - sizes.begin()] * sizeof(float)));
            glEnableVertexAttribArray(it - sizes.begin());
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    template<typename container>
    void update(const container& data){
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &(*data.begin()), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void draw(GLenum mode = GL_TRIANGLES)const{
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, vcount);
    }
    ~vertex_array(){
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
};
#endif