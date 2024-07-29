#include "ShaderLibrary.h"

GLuint ShaderLibrary::LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {
	std::ifstream vsStream(vertex_file_path, std::ios::in);
	if (!vsStream.is_open()) {
		std::cout << "Failed to open vertex shader: \"" << vertex_file_path << "\"\n";
		return 0;
	}
	std::stringstream vssstr;
	vssstr << vsStream.rdbuf();
	std::string vsSource = vssstr.str();
	vsStream.close();

	std::ifstream fsStream(fragment_file_path, std::ios::in);
	if (!fsStream.is_open()) {
		std::cout << "Failed to open fragment shader: \"" << fragment_file_path << "\"\n";
		return 0;
	}
	std::stringstream fssstr;
	fssstr << fsStream.rdbuf();
	std::string fsSource = fssstr.str();
	fsStream.close();

	return CreateShaderProgramm(vsSource, fsSource);
}

GLuint ShaderLibrary::CreateShaderProgramm(const std::string& vsSource, const std::string& fsSource) {
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	char const* VertexSourcePointer = vsSource.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	char const* FragmentSourcePointer = fsSource.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}