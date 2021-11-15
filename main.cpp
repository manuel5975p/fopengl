#include <vector>
#include <fopengl.hpp>
#include <shader.hpp>
#include <Eigen/Dense>
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
    std::vector<float> data = {-1,-1,0,0,1,-1,1,0,1,1,1,1,-1,1,0,1};
    vertex_array va(data, 2, 2);
    ShaderProgram def = texture_shader();
    std::vector<uint8_t> tdata = {255,0,0,255, 255,255,0,255, 0,255,0,255, 0,255,255,255};
    texture tex(2,2);
    Eigen::Matrix<Eigen::Array<float, 3, 1>, Eigen::Dynamic, Eigen::Dynamic> mat(255, 255);
    for(size_t i = 0; i < mat.rows();i++){
        for(size_t j = 0; j < mat.cols();j++){
            mat(i, j)(0) = i / 256.0f;
            mat(i, j)(1) = 0.8f;
            mat(i, j)(2) = 0.8f;
        }
    }
    tex.update(mat);
    tex.bind();
    //glEnable(GL_PROGRAM_POINT_SIZE);
    while(win){
        win.poll_events();
        def.bind();
        tex.bind();
        va.draw(GL_TRIANGLE_FAN);
        def.unbind();
        win.update();
    }
}