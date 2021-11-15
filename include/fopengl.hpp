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
        void mousebcb(GLFWwindow* window, int button, int action, int mods);
    }
    struct window{
        GLFWwindow* m_window;
        int width, height;
        bool closed;
        window(unsigned int _width, unsigned int _height, const std::string& title) : closed(false){
            if(!glfwInit()){
                std::cerr << "glfw_init failed" << "\n";
                std::terminate();
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
                std::cerr << "gladLoad failed" << "\n";
	        	std::terminate();
	        }

	        if (glfwExtensionSupported("GL_ARB_buffer_storage") == GLFW_FALSE) {
                std::cerr << "GL_ARB_buffer_storage failed" << "\n";
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
            this->mousebutton_callback = [this](window*, int a, int b, int c){};
            glDebugMessageCallback(MessageCallback, 0);
            glfwSetMouseButtonCallback(m_window, impl::mousebcb);
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
        void set_framebuffer_resize_callback(std::function<void(window*, int, int)> kcb){
            this->framebr_callback = kcb;
        }
        void set_mousebutton_callback(std::function<void(window*, int, int, int)> kcb){
            this->mousebutton_callback = kcb;
        }
        std::function<void(window*, int, int)> key_callback;
        std::function<void(window*, int, int, int)> mousebutton_callback;
        std::function<void(window*, int, int)> framebr_callback;
    };
    namespace impl{
        void keycb(GLFWwindow* window, int key, int scancode, int action, int mods){
            winwin_map[window]->key_callback(winwin_map[window], key, action);
        }
        void framebrcb(GLFWwindow* window, int width, int height){
            winwin_map[window]->framebr_callback(winwin_map[window], width, height);
        }
        void mousebcb(GLFWwindow* window, int button, int action, int mods){
            winwin_map[window]->mousebutton_callback(winwin_map[window], button, action, mods);
        }
    }
}
struct vertex_array{
    GLuint vao, vbo;
    size_t vcount;
    vertex_array() : vao(0), vbo(0), vcount(0){}
    template<typename container, typename... Ts>
    vertex_array(const container& data, Ts... ts){
        std::vector<unsigned int> sizes({static_cast<unsigned int>(ts)...});
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(typename container::value_type) * data.size(), &(*data.begin()), GL_STATIC_DRAW);
        
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
        if(vbo)
        glDeleteBuffers(1, &vbo);
        if(vao)
        glDeleteVertexArrays(1, &vao);
    }
    vertex_array& operator=(const vertex_array&) = delete;
    vertex_array& operator=(vertex_array&& o){
        vao = o.vao;
        vbo = o.vbo;
        vcount = o.vcount;
        o.vao = 0;
        o.vbo = 0;
        o.vcount = 0;
        return *this;
    }
    vertex_array(const vertex_array& o) = delete;
    vertex_array(vertex_array&& o) : vao(o.vao), vbo(o.vbo), vcount(o.vcount){
        o.vao = 0;
        o.vbo = 0;
        o.vcount = 0;
    }
};
struct texture{
    private:
    GLuint tex_handle;
    size_t m_width, m_height;
    public:
    size_t width()const{
        return m_width;
    }
    size_t height()const{
        return m_height;
    }
    GLuint handle()const{
        return tex_handle;
    }
    texture() : tex_handle(0), m_width(0), m_height(0){}
    texture(size_t w, size_t h) : m_width(w), m_height(h){
        glGenTextures(1, &tex_handle);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    texture(size_t w, size_t h, const std::vector<unsigned char>& data) : m_width(w), m_height(h){
        assert(data.size() == 4 * width() * height());
        glGenTextures(1, &tex_handle);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    void update(const std::vector<unsigned char>& data){
        assert(data.size() == 4 * width() * height());
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    }
    void update(const std::vector<unsigned char>& data, size_t w, size_t h){
        m_width = w;
        m_height = h;
        assert(data.size() == 4 * width() * height());
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    }
    template<typename image_type>
    void update(const image_type& img){
        //m_width = img.cols();
        //m_height = img.rows();
        std::vector<unsigned char> data(4 * img.rows() * img.cols());
        for(size_t i = 0; i < img.rows();i++){
            for(size_t j = 0; j < img.cols();j++){
                size_t pos = i * img.cols() + j;
                size_t offset = pos * 4;
                if constexpr(std::is_floating_point_v<typename std::remove_cvref_t<decltype(img(i, j))>::Scalar>){
                    if(img(i, j).rows() == 4){
                        data[offset + 0] = uint8_t(std::min(255.0f,img(i, j)(0) * 256));
                        data[offset + 1] = uint8_t(std::min(255.0f,img(i, j)(1) * 256));
                        data[offset + 2] = uint8_t(std::min(255.0f,img(i, j)(2) * 256));
                        data[offset + 3] = uint8_t(std::min(255.0f,img(i, j)(3) * 256));
                    }
                    if(img(i, j).rows() == 3){
                        data[offset + 0] = uint8_t(std::min(255.0f,img(i, j)(0) * 256));
                        data[offset + 1] = uint8_t(std::min(255.0f,img(i, j)(1) * 256));
                        data[offset + 2] = uint8_t(std::min(255.0f,img(i, j)(2) * 256));
                        data[offset + 3] = 255;
                    }
                }
                else{
                    if(img(i, j).rows() == 4){
                        data[offset + 0] = uint8_t(img(i, j)(0));
                        data[offset + 1] = uint8_t(img(i, j)(1));
                        data[offset + 2] = uint8_t(img(i, j)(2));
                        data[offset + 3] = uint8_t(img(i, j)(3));
                    }
                    if(img(i, j).rows() == 3){
                        data[offset + 0] = uint8_t(img(i, j)(0));
                        data[offset + 1] = uint8_t(img(i, j)(1));
                        data[offset + 2] = uint8_t(img(i, j)(2));
                        data[offset + 3] = uint8_t(255);
                    }
                }
            }
        }
        update(data, img.cols(), img.rows());
    }
    void bind(){
        glBindTexture(GL_TEXTURE_2D, handle());
    }
    ~texture(){
        if(tex_handle){
            glDeleteTextures(1, &tex_handle);
        }
    }
    texture& operator=(const texture&) = delete;
    texture& operator=(texture&& o){
        tex_handle = o.tex_handle;
        o.tex_handle = 0;
        return *this;
    }
    texture(const texture& o) = delete;
    texture(texture&& o) : tex_handle(o.tex_handle){
        o.tex_handle = 0;
    }
};
#endif
