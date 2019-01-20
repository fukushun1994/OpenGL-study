#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

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
	GLFWwindow *const window(glfwCreateWindow(640, 480, "Hello!", NULL, NULL));
	if (window == NULL) {
		// �E�B���h�E���쐬�ł��Ȃ�����
		cerr << "Can't create GLFW window." << endl;
		return 1;
	}

	// �쐬�����E�B���h�E�� OpenGL �̏����Ώۂɂ���
	glfwMakeContextCurrent(window);

	// GLEW ������������
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		// GLEW �̏������Ɏ��s����
		cerr << "Can't initialize GLEW" << endl;
		return 1;
	}

	// ���������̃^�C�~���O��҂�
	glfwSwapInterval(1);

	// �w�i�F���w�肷��
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// �E�B���h�E���J���Ă���ԌJ��Ԃ�
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// �E�B���h�E����������
		glClear(GL_COLOR_BUFFER_BIT);

		//
		// �����ŕ`�揈�����s��
		//

		// �J���[�o�b�t�@�����ւ���
		glfwSwapBuffers(window);

		// �C�x���g�����o��
		glfwWaitEvents();
	}

	//

	return 0;
}