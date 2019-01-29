#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shape.h"
#include "ShapeIndex.h"
#include "SolidShape.h"
#include "SolidShapeIndex.h"
#include "Window.h"
#include "Matrix.h"

using namespace std;

const GLfloat PI = 3.141519653589793238462643383279;

bool readShaderSource(const char *name, vector<GLchar> &buffer) {
	// �t�@�C������NULL������
	if (name == NULL) {
		return false;
	}

	// �\�[�X�t�@�C�����J��
	ifstream file(name, ios::binary);
	if (file.fail()) {
		// �J���Ȃ�����
		cerr << "Error: Can't open source file: " << name << endl;
		return false;
	}

	// �t�@�C���̖����Ɉړ������݈ʒu�i���t�@�C���T�C�Y�j�𓾂�
	file.seekg(0L, ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	// �t�@�C���T�C�Y�̃��������m��
	buffer.resize(length + 1);

	// �t�@�C����擪����ǂݍ���
	file.seekg(0L, ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail()) {
		// �ǂݍ��߂Ȃ�����
		cerr << "Error: Could not read source file:" << name << endl;
		file.close();
		return false;
	}

	// �ǂݍ��ݐ���
	file.close();
	return true;
}

// �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
//  shader: �V�F�[�_�I�u�W�F�N�g��
//  str: �R���p�C���G���[�����������ꏊ������������
GLboolean printShaderInfoLog(GLuint shader, const char *str) {
	// �R���p�C�����ʂ��擾����
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "Compile Error in " << str << endl;
	}

	// �V�F�[�_�̃R���p�C�����̃��O�̒������擾����
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		cerr << &infoLog[0] << endl;
	}
	return static_cast<GLboolean>(status);
}

// �v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
//  program: �v���O�����I�u�W�F�N�g��
GLboolean printProgramInfoLog(GLuint program) {
	// �����N���ʂ��擾����
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "Link Error." << endl;
	}

	// �V�F�[�_�̃����N���̃��O�̒������擾����
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1) {
		// �V�F�[�_�̃����N���̃��O�̓��e���擾����
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		cerr << &infoLog[0] << endl;
	}
	return static_cast<GLboolean>(status);
}

// �v���O�����I�u�W�F�N�g���쐬����
//  vsrc: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�t�@�C����
//  fsrc: �t���O�����g�V�F�[�_�̃\�[�X�t�@�C����
GLuint createProgram(const char *vsrc, const char *fsrc) {

	// ��̃v���O�����I�u�W�F�N�g���쐬����
	const GLuint program(glCreateProgram());

	if (vsrc != NULL) {
		// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬����
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
		if (printShaderInfoLog(vobj, "vertex shader")) {
			glAttachShader(program, vobj);
		}
		glDeleteShader(vobj);
	}

	if (fsrc != NULL) {
		// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬����
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
		if (printShaderInfoLog(fobj, "fragment shader")) {
			glAttachShader(program, fobj);
		}
		glDeleteShader(fobj);
	}

	// �v���O�����I�u�W�F�N�g�������N����
	glBindAttribLocation(program, 0, "position");
	glBindAttribLocation(program, 1, "color");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	// �쐬�����v���O�����I�u�W�F�N�g��Ԃ�
	if (printProgramInfoLog(program)) {
		return program;
	}

	// �v���O�����I�u�W�F�N�g���쐬�ł��Ȃ���� 0 ��Ԃ�
	glDeleteProgram(program);
	return 0;
}

GLuint loadProgram(const char *vert, const char *frag) {
	// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
	vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// �����ʑ̂̒��_�̈ʒu
constexpr Object::Vertex octahedronVertex[] = {
	{ 0.0f, 1.0f, 0.0f },
	{ -1.0f, 0.0f, 0.0f },
	{ 0.0f, -1.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 0.0f, -1.0f, 0.0f },
	{ 0.0f, 0.0f, -1.0f },
	{ -1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, -1.0f }
};

// �Z�ʑ̂̒��_�̈ʒu
constexpr Object::Vertex cubeVertex[] =
{
	{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f }, // (0)
	{ -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.8f }, // (1)
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.8f, 0.0f }, // (2)
	{ -1.0f, 1.0f, -1.0f, 0.0f, 0.8f, 0.8f }, // (3)
	{ 1.0f, 1.0f, -1.0f, 0.8f, 0.0f, 0.0f }, // (4)
	{ 1.0f, -1.0f, -1.0f, 0.8f, 0.0f, 0.8f }, // (5)
	{ 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.0f }, // (6)
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f } // (7)
};
// �Z�ʑ̗̂Ő��̗��[�_�̃C���f�b�N�X
constexpr GLuint wireCubeIndex[] =
{
	1, 0, // (a)
	2, 7, // (b)
	3, 0, // (c)
	4, 7, // (d)
	5, 0, // (e)
	6, 7, // (f)
	1, 2, // (g)
	2, 3, // (h)
	3, 4, // (i)
	4, 5, // (j)
	5, 6, // (k)
	6, 1 // (l)
};
/*
// �ʂ��ƂɐF��ς����Z�ʑ̂̒��_����
constexpr Object::Vertex solidCubeVertex[] =
{
	// ��
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	// ��
	{ 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	// ��
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{ 1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{ 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
	{ -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
	// �E
	{ 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
	// ��
	{ -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
	{ 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	// �O
	{ -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f }
};

// �Z�ʑ̖̂ʂ�h��Ԃ��O�p�`�̒��_�̃C���f�b�N�X
constexpr GLuint solidCubeIndex[] =
{
	0, 1, 2, 0, 2, 3, // ��
	4, 5, 6, 4, 6, 7, // ��
	8, 9, 10, 8, 10, 11, // ��
	12, 13, 14, 12, 14, 15, // �E
	16, 17, 18, 16, 18, 19, // ��
	20, 21, 22, 20, 22, 23 // �O
};
*/

// �ʂ��ƂɐF��ς����Z�ʑ̂̒��_����
constexpr Object::Vertex solidCubeVertex[] =
{
	// ��
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	// ��
	{ 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	// ��
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{ 1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{ 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{ 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
	{ -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
	// �E
	{ 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{ 1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
	// ��
	{ -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
	{ -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
	{ 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	// �O
	{ -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
	{ -1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f }
};

int main() {
	// GLFW������������
	if (glfwInit() == GL_FALSE) {
		// �������Ɏ��s����
		cerr << "Can't initialize GLFW" << endl;
		return 1;
	}

	// �v���O�����I�����̏�����o�^����
	atexit(glfwTerminate);

	// OpenGL Version 3.2 Core Profile ��I������
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	// �E�B���h�E���쐬����
	Window window;

	// �w�i�F���w�肷��
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// �w�ʃJ�����O��L���ɂ���
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// �f�v�X�o�b�t�@��L���ɂ���
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// �v���O�����I�u�W�F�N�g���쐬����
	GLuint program(loadProgram("point.vert", "point.frag"));

	// uniform�ϐ��̏ꏊ���擾����
	const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
	const GLint projectionLoc(glGetUniformLocation(program, "projection"));

	// �}�`�f�[�^���쐬����
	unique_ptr<const Shape> shape(new SolidShape(3, 36, solidCubeVertex));

	// �^�C�}�[��0�ɃZ�b�g
	glfwSetTime(0.0);

	// �E�B���h�E���J���Ă���ԌJ��Ԃ�
	while (window.shouldClose() == GL_FALSE) {
		// �E�B���h�E����������
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// �V�F�[�_�v���O�����̎g�p�J�n
		glUseProgram(program);

		// �������e�ϊ��s������߂�
		const GLfloat * const size(window.getSize());
		const GLfloat fovy(window.getScale() * 0.01f);
		const GLfloat aspect(size[0] / size[1]);
		const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 10.0f));


		// ���f���ϊ��s������߂�
		const GLfloat *const location(window.getLocation());
		const Matrix r(Matrix::rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f));
		const Matrix model(Matrix::translate(location[0], location[1], 0.0f) * r);

		// �r���[�ϊ��s������߂�
		const Matrix view(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

		// ���f���r���[�ϊ��s������߂�
		const Matrix modelview(view * model);

		// uniform�ϐ��ɒl��ݒ肷��
		glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());

		// �}�`��`�悷��
		shape->draw();


		// ��ڂ̃��f���r���[�ϊ��s������߂�
		const Matrix modelview1(modelview * Matrix::translate(0.0f, 0.0f, 3.0f));
		glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview1.data());
		shape->draw();

		// �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
		window.swapBuffers();
	}

	return 0;
}