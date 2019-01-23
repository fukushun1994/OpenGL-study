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
#include "Window.h"

using namespace std;

bool readShaderSource(const char *name, vector<GLchar> &buffer) {
	// ファイル名がNULLだった
	if (name == NULL) {
		return false;
	}

	// ソースファイルを開く
	ifstream file(name, ios::binary);
	if (file.fail()) {
		// 開けなかった
		cerr << "Error: Can't open source file: " << name << endl;
		return false;
	}

	// ファイルの末尾に移動し現在位置（＝ファイルサイズ）を得る
	file.seekg(0L, ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	// ファイルサイズのメモリを確保
	buffer.resize(length + 1);

	// ファイルを先頭から読み込む
	file.seekg(0L, ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail()) {
		// 読み込めなかった
		cerr << "Error: Could not read source file:" << name << endl;
		file.close();
		return false;
	}

	// 読み込み成功
	file.close();
	return true;
}

// シェーダオブジェクトのコンパイル結果を表示する
//  shader: シェーダオブジェクト名
//  str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str) {
	// コンパイル結果を取得する
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "Compile Error in " << str << endl;
	}

	// シェーダのコンパイル時のログの長さを取得する
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// シェーダのコンパイル時のログの内容を取得する
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		cerr << &infoLog[0] << endl;
	}
	return static_cast<GLboolean>(status);
}

// プログラムオブジェクトのリンク結果を表示する
//  program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program) {
	// リンク結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "Link Error." << endl;
	}

	// シェーダのリンク時のログの長さを取得する
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1) {
		// シェーダのリンク時のログの内容を取得する
		vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		cerr << &infoLog[0] << endl;
	}
	return static_cast<GLboolean>(status);
}

// プログラムオブジェクトを作成する
//  vsrc: バーテックスシェーダのソースファイル名
//  fsrc: フラグメントシェーダのソースファイル名
GLuint createProgram(const char *vsrc, const char *fsrc) {

	// 空のプログラムオブジェクトを作成する
	const GLuint program(glCreateProgram());

	if (vsrc != NULL) {
		// バーテックスシェーダのシェーダオブジェクトを作成する
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(vobj, "vertex shader")) {
			glAttachShader(program, vobj);
		}
		glDeleteShader(vobj);
	}

	if (fsrc != NULL) {
		// フラグメントシェーダのシェーダオブジェクトを作成する
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(fobj, "fragment shader")) {
			glAttachShader(program, fobj);
		}
		glDeleteShader(fobj);
	}

	// プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	// 作成したプログラムオブジェクトを返す
	if (printProgramInfoLog(program)) {
		return program;
	}

	// プログラムオブジェクトが作成できなければ 0 を返す
	glDeleteProgram(program);
	return 0;
}

GLuint loadProgram(const char *vert, const char *frag) {
	// シェーダのソースファイルを読み込む
	vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

// 矩形の頂点の位置
constexpr Object::Vertex rectangleVertex[] = {
	{ -0.5f, -0.5f },
	{  0.5f, -0.5f },
	{  0.5f,  0.5f },
	{ -0.5f,  0.5f }
};

int main() {
	// GLFWを初期化する
	if (glfwInit() == GL_FALSE) {
		// 初期化に失敗した
		cerr << "Can't initialize GLFW" << endl;
		return 1;
	}

	// プログラム終了時の処理を登録する
	atexit(glfwTerminate);

	// OpenGL Version 3.2 Core Profile を選択する
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	// ウィンドウを作成する
	Window window;

	// 背景色を指定する
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


	// プログラムオブジェクトを作成する
	GLuint program(loadProgram("point.vert", "point.frag"));

	// uniform変数の場所を取得する
	const GLint sizeLoc(glGetUniformLocation(program, "size"));
	const GLint scaleLoc(glGetUniformLocation(program, "scale"));
	const GLint locationLoc(glGetUniformLocation(program, "location"));

	// 図形データを作成する
	unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));


	// ウィンドウが開いている間繰り返す
	while (window.shouldClose() == GL_FALSE) {
		// ウィンドウを消去する
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダプログラムの使用開始
		glUseProgram(program);

		// uniform変数に値を設定する
		glUniform2fv(sizeLoc, 1, window.getSize());
		glUniform1f(scaleLoc, window.getScale());
		glUniform2fv(locationLoc, 1, window.getLocation());

		// 図形を描画する
		shape->draw();

		// カラーバッファを入れ替えてイベントを取り出す
		window.swapBuffers();
	}

	return 0;
}