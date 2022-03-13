#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShaders(std::string filepath) {
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream src(filepath);
	std::string line;
	ShaderType type = ShaderType::NONE;
	std::stringstream ss[2];
	while (getline(src, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int) type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, std::string source) {
	const char* src = source.c_str();
	GLCall(unsigned int id = glCreateShader(type));
	GLCall(glShaderSource(id, 1, &src, 0));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		std::cout << "error" << std::endl;
	}

	return id;
}

static unsigned int CreateProgram(std::string vertexSrc, std::string fragmentSrc) {
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	return program;
}

int main() {

	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);

	glfwShowWindow(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	ShaderProgramSource shaders = ParseShaders("res/shaders/Basic.shader");
	unsigned int program = CreateProgram(shaders.VertexSource, shaders.FragmentSource);
	GLCall(glUseProgram(program));
	{
		float vert[] = {
		-0.5f,-0.5f,0.0f,   0.0f,1.0f,0.5f,1.0f,
		 0.5f,-0.5f,0.0f,   0.0f,1.0f,1.0f,1.0f,
		 0.5f, 0.5f,0.0f,   1.0f,1.0f,0.0f,1.0f,
		-0.5f, 0.5f,0.0f,   0.0f,0.0f,1.0f,1.0f,
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexBuffer vb(vert, 7*4 * sizeof(float));

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void*)(3 * (sizeof(float)))));

		IndexBuffer ib(indices, 6);

		float vert2[] = {
			 0.6f, 0.0f,0.0f,   1.0f,0.0f,0.0f,1.0f,
			 0.8f, 0.0f,0.0f,   1.0f,0.0f,0.0f,1.0f,
			 0.8f, 0.4f,0.0f,   1.0f,0.0f,0.0f,1.0f,
			 0.6f, 0.4f,0.0f,   1.0f,0.0f,0.0f,1.0f,
		};

		unsigned int indices2[] = {
			0, 1, 2,
			2, 3, 0
		};

		unsigned int vao2;
		glGenVertexArrays(1, &vao2);
		glBindVertexArray(vao2);

		VertexBuffer vb2(vert2, 7 * 4 * sizeof(float));

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void*)(3 * (sizeof(float)))));

		IndexBuffer ib2(indices2, 6);

		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT);

			GLCall(glUseProgram(program));

			GLCall(glBindVertexArray(vao));

			GLCall(glEnableVertexAttribArray(0));
			GLCall(glEnableVertexAttribArray(1));

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			GLCall(glDisableVertexAttribArray(0));
			GLCall(glDisableVertexAttribArray(1));

			GLCall(glBindVertexArray(vao2));

			GLCall(glEnableVertexAttribArray(0));
			GLCall(glEnableVertexAttribArray(1));

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			GLCall(glDisableVertexAttribArray(0));
			GLCall(glDisableVertexAttribArray(1));

			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		glDeleteProgram(program);
	}

	glfwTerminate();

	return 0;
}