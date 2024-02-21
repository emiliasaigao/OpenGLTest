#include "Shader.h"

Shader::Shader(const std::string& vertexPath, 
				const std::string& fragmentPath, 
				const std::string& geometryPath) {
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	std::ifstream geometryFile;
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	// 保证ifstream对象可以抛出异常：
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	geometryFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		std::stringstream vertexss;
		std::stringstream fragmentss;
		vertexss << vertexFile.rdbuf();
		fragmentss << fragmentFile.rdbuf();
		vertexCode = vertexss.str();
		fragmentCode = fragmentss.str();
		if (geometryPath != "") {
			geometryFile.open(geometryPath);
			std::stringstream geometryss;
			geometryss << geometryFile.rdbuf();
			geometryCode = geometryss.str();
		}
	}
	catch(std::fstream::failure e){
		std::cout << "打开shader文件失败！" << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int success;
	char infolog[1024];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, NULL, infolog);
		std::cout << "顶点着色器编译失败！错误信息：\n" << infolog << std::endl;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		std::cout << "片元着色器编译失败！错误信息：\n" << infolog << std::endl;
	}


	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	if (geometryPath != "") {
		const char* geometryShaderSource = geometryCode.c_str();
		unsigned int geometryShader;
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometryShader, 512, NULL, infolog);
			std::cout << "几何着色器编译失败！错误信息：\n" << infolog << std::endl;
		}
		glAttachShader(ID, geometryShader);
	}

	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 1024, NULL, infolog);
		std::cout << "着色器程序链接失败！错误信息：\n" << infolog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
