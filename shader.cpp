#include "shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#define LOG(x,y) //std::cout << x << std::endl
void ShaderProgram::print(std::string _id, ShaderProgram::Status _compComp, ShaderProgram::Status _compVert,
	ShaderProgram::Status _compGeom, ShaderProgram::Status _compFrag, ShaderProgram::Status _link, std::string _errorLog) {
	if (!printDebug) return;
	LOG("   Shader: " + std::string(_id), true);
	LOG("Compiling: "
		+ std::string(_compComp == Status::failed ? " X |" : _compComp == Status::success ? " S |" : " - |")
		+ std::string(_compVert == Status::failed ? " X |" : _compVert == Status::success ? " S |" : " - |")
		+ std::string(_compGeom == Status::failed ? " X |" : _compGeom == Status::success ? " S |" : " - |")
		+ std::string(_compFrag == Status::failed ? " X |" : _compFrag == Status::success ? " S |" : " - |")
		+ "", false);
	LOG("  Linking: " + std::string(_link == Status::failed ? "Failed!" : _link == Status::success ? "Success!" : " - "), false);

	if (!_errorLog.empty()) {
		LOG(std::string(_errorLog) + "", false);
	} else
		LOG("", false);

}

bool ShaderProgram::compileFromFile(const std::string& _path) {
	bool cExists = true;
	bool vExists = true;
	bool gExists = true;
	bool fExists = true;

	std::ifstream compB(_path + ".comp");
	cExists = compB.good();

	std::ifstream vertB(_path + ".vert");
	vExists = vertB.good();

	std::ifstream geomB(_path + ".geom");
	gExists = geomB.good();

	std::ifstream fragB(_path + ".frag");
	fExists = fragB.good();

	bool success = compile(
		(cExists ? std::string{ std::istreambuf_iterator<char>(compB), std::istreambuf_iterator<char>() } : "").c_str(),
		(vExists ? std::string{ std::istreambuf_iterator<char>(vertB), std::istreambuf_iterator<char>() } : "").c_str(),
		(gExists ? std::string{ std::istreambuf_iterator<char>(geomB), std::istreambuf_iterator<char>() } : "").c_str(),
		(fExists ? std::string{ std::istreambuf_iterator<char>(fragB), std::istreambuf_iterator<char>() } : "").c_str());

	compB.close();
	vertB.close();
	geomB.close();
	fragB.close();

	return success;
}

bool ShaderProgram::compile(const char* _compute, const char* _vertex, const char* _geom, const char* _frag) {
	Status compStatus = Status::missing;
	Status vertStatus = Status::missing;
	Status geomStatus = Status::missing;
	Status fragStatus = Status::missing;
	Status linkStatus = Status::missing;

	//std::cout << _compute << std::endl;

	if (compute != -1) {
		glDeleteShader(compute);
		compute = -1;
	}
	if (vertex != -1) {
		glDeleteShader(vertex);
		vertex = -1;
	}
	if (geom != -1) {
		glDeleteShader(geom);
		geom = -1;
	}
	if (frag != -1) {
		glDeleteShader(frag);
		frag = -1;
	}
	if (program != -1) {
		glDeleteShader(program);
		program = -1;
	}

	//Compile Compute
	if (_compute != NULL && _compute[0] != '\0') {
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &_compute, nullptr);
		glCompileShader(compute);
		GLint isCompiled = 0;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(compute, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(compute);
			compStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else compStatus = Status::success;
	}

	//Compile Vertex
	if (_vertex != NULL && _vertex[0] != '\0') {
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &_vertex, nullptr);
		glCompileShader(vertex);
		GLint isCompiled = 0;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(vertex, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(vertex);
			vertStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else vertStatus = Status::success;
	}

	//Compile Geom
	if (_geom != NULL && _geom[0] != '\0') {
		geom = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geom, 1, &_geom, nullptr);
		glCompileShader(geom);
		GLint isCompiled = 0;
		glGetShaderiv(geom, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(geom, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(geom, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(geom);
			geomStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else geomStatus = Status::success;
	}

	//Compile Frag
	if (_frag != NULL && _frag[0] != '\0') {
		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &_frag, nullptr);
		glCompileShader(frag);
		GLint isCompiled = 0;
		glGetShaderiv(frag, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(frag, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(frag);
			fragStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else fragStatus = Status::success;
	}

	//Link
	program = glCreateProgram();
	if (_compute != NULL && _compute[0] != '\0') glAttachShader(program, compute);
	if (_vertex != NULL && _vertex[0] != '\0') glAttachShader(program, vertex);
	if (_geom != NULL && _geom[0] != '\0') glAttachShader(program, geom);
	if (_frag != NULL && _frag[0] != '\0') glAttachShader(program, frag);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
		if (compute != -1)glDeleteShader(compute);
		if (vertex != -1)glDeleteShader(vertex);
		if (geom != -1)glDeleteShader(geom);
		if (frag != -1)glDeleteShader(frag);
		if (program != -1) glDeleteProgram(program);
		linkStatus = Status::failed;

		print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
		return false;
	} else linkStatus = Status::success;

	if (_compute != NULL && _compute[0] != '\0')glDetachShader(program, compute);
	if (_vertex != NULL && _vertex[0] != '\0')glDetachShader(program, vertex);
	if (_geom != NULL && _geom[0] != '\0')glDetachShader(program, geom);
	if (_frag != NULL && _frag[0] != '\0')glDetachShader(program, frag);

	print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, "");

	unbind();
	return true;
}

GLuint ShaderProgram::getHandle() {
	return program;
}

ShaderProgram::ShaderProgram(std::string _id) : id(_id) {}

ShaderProgram::ShaderProgram() : ShaderProgram(""){}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program);
}

void ShaderProgram::bind() {
	glUseProgram(getHandle());
}
void ShaderProgram::unbind() {
	glUseProgram(0);
}
void ShaderProgram::uniform1f(const char* x, float v1){
	int loc = glGetUniformLocation(getHandle(), x);
	glUniform1f(loc, v1);
}
void ShaderProgram::uniform2f(const char* x, float v1, float v2){
	int loc = glGetUniformLocation(getHandle(), x);
	glUniform2f(loc, v1, v2);
}
void ShaderProgram::uniform3f(const char* x, float v1, float v2, float v3){
	int loc = glGetUniformLocation(getHandle(), x);
	glUniform3f(loc, v1, v2, v3);
}
void ShaderProgram::uniform4f(const char* x, float v1, float v2, float v3, float v4){
	int loc = glGetUniformLocation(getHandle(), x);
	glUniform4f(loc, v1, v2, v3, v4);
}
void ShaderProgram::uniform44(const char* x, const float* mat){
	int loc = glGetUniformLocation(getHandle(), x);
	glProgramUniformMatrix4fv(getHandle(),loc, 1, GL_FALSE, mat); 
}
ShaderProgram const_shader(){
	
	std::string vert = 	R"(
						#version 460 core
						layout (location = 0) in vec3 aPos;
						uniform vec4 fill;
						out vec4 vertexColor;
						void main(){
						gl_Position = vec4(aPos, 1);
						vertexColor = fill;})";
	std::string frag  = "#version 460 core\n"
						"out vec4 FragColor;\n"
						"in vec4 vertexColor;\n"
						"void main(){\n"
						"FragColor = vertexColor;\n"
						"}";
	ShaderProgram prog;
	prog.compile(nullptr, vert.c_str(), nullptr, frag.c_str());
	prog.bind();
	prog.uniform4f("fill", 1,0,0,1);
	prog.unbind();
	return prog;
}
ShaderProgram point_shader(){
	
	std::string vert = 	"#version 420 core\n"
						"layout (location = 0) in vec3 aPos;\n"
						"layout (location = 1) in vec3 col;\n"
						"out vec4 vertexColor;\n"
						"void main(){\n"
	//					"gl_PointSize = 6;\n"
						"gl_Position = vec4(aPos, 1);\n"
	//					"gl_Position.x += gl_InstanceID / 200.0;\n"
						"vertexColor = vec4(col, 1);\n"
	//					"vertexColor.x += gl_InstanceID / 200.0;\n"
						"}";
	std::string frag  = "#version 420 core\n"
						"out vec4 FragColor;\n"
						"in vec4 vertexColor;\n"
						"void main(){\n"
						"FragColor = vertexColor;\n"
						"}";
	ShaderProgram prog;
	prog.compile(nullptr, vert.c_str(), nullptr, frag.c_str());
	prog.bind();
	prog.uniform4f("fill", 1,0,0,1);
	prog.unbind();
	return prog;
}
ShaderProgram color_shader(){
	std::string vert = 	"#version 460 core\n"
						"layout (location = 0) in vec3 aPos;\n"
						"layout (location = 1) in vec3 col;\n"
						"out vec4 vertexColor;\n"
						"void main(){\n"
						"gl_Position = vec4(aPos, 1);\n"
						"vertexColor = vec4(col, 1);}\n";
	std::string frag  = "#version 460 core\n"
						"out vec4 FragColor;\n"
						"in vec4 vertexColor;\n"
						"void main(){\n"
						"FragColor = vertexColor;\n"
						"}";
	ShaderProgram prog;
	prog.compile(nullptr, vert.c_str(), nullptr, frag.c_str());
	return prog;
}