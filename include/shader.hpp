#ifndef SHADER_HPP
#define SHADER_HPP
#include <string>
#include <iostream>
#include <fstream>
class ShaderProgram {
public:
	enum class Status {
		success, failed, missing
	};

	unsigned int program = 0, compute = 0, vertex = 0, geom = 0, frag = 0;
	void print(std::string, Status, Status, Status, Status, Status, std::string);
	


	ShaderProgram(std::string);
	ShaderProgram();
	~ShaderProgram();
	ShaderProgram(const ShaderProgram& sp) = delete;
	ShaderProgram(ShaderProgram&& sp);
	ShaderProgram& operator=(const ShaderProgram& sp) = delete;
	ShaderProgram& operator=(ShaderProgram&& sp);
	std::string id;
	bool printDebug = true;
	bool compileFromFile(const std::string&);
	bool compile(const char*, const char*, const char*, const char*);
	unsigned int getHandle();
	void bind();
	void unbind();
	void uniform1f(const char* x, float v1);
	void uniform2f(const char* x, float v1, float v2);
	void uniform3f(const char* x, float v1, float v2, float v3);
	void uniform4f(const char* x, float v1, float v2, float v3, float v4);
	void uniform44(const char* x, const float* mat);
};
ShaderProgram const_shader(float r, float g, float b, float a);
ShaderProgram point_shader();
ShaderProgram texture_shader();
ShaderProgram color_shader();
#endif