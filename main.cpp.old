#include <vector>
#include <fopengl.hpp>
#include <shader.hpp>
int main(){
    fgl::window win(800, 600, "Zwetschgewindow");
    win.set_key_callback([](fgl::window* w, int key, int action){
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
            w->close();
        }
    });
    win.set_mousebutton_callback([](fgl::window* win, int a, int b, int c){
        std::cout << b << std::endl;
    });
    std::vector<float> data = {-0.1, 0.1, 0,
                                0, 1, 0,
                                0.1,0.1, 0, 
                                1,0,0,
                                0.1,-0.1, 0,
                                0,0,1};
    vertex_array va(data, 3, 3);
    ShaderProgram def = const_shader(1,0,0,1);
    //glEnable(GL_PROGRAM_POINT_SIZE);
    while(win){
        win.poll_events();
        def.bind();
        va.draw(GL_TRIANGLES);
        def.unbind();
        win.update();
    }
}