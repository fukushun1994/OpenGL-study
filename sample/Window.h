#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ウィンドウ関連の処理
class Window {
public:
	// コンストラクタ
	Window(int width = 640, int height = 480, const char *title = "Hello!")
		: window(glfwCreateWindow(width, height, title, NULL, NULL))
		, scale(100.0f), location{0, 0}, arrowKeyCount(0), wheelRotation(0.0)
	{
		if (window == NULL) {
			// ウィンドウが作成できなかった
			std::cerr << "Can't create GLFW window." << std::endl;
			exit(1);
		}

		// 現在のウィンドウを処理対象にする
		glfwMakeContextCurrent(window);

		// GLEWを初期化する
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			// GLEWの初期化に失敗した
			std::cerr << "Can't initialize GLEW" << std::endl;
			exit(1);
		}

		// 垂直同期のタイミングを待つ
		glfwSwapInterval(1);

		// ウィンドウのサイズ変更時に呼び出す処理の登録
		glfwSetWindowSizeCallback(window, resize);

		// マウスホイール操作時に呼び出す処理の登録
		glfwSetScrollCallback(window, wheel);

		// このインスタンスのthisポインタを記録しておく
		glfwSetWindowUserPointer(window, this);

		// ウィンドウのサイズ変更時に呼び出す処理の登録
		glfwSetKeyCallback(window, keyboad);

		// 開いたウィンドウの初期設定
		resize(window, width, height);
	}

	// デストラクタ
	virtual ~Window() {
		glfwDestroyWindow(window);
	}

	// ウィンドウを閉じるべきかを判定する
	int shouldClose() const {
		return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
	}

	// カラーバッファを入れ替えてイベントを取り出す
	void swapBuffers() {
		// カラーバッファを入れ替える
		glfwSwapBuffers(window);

		// イベントを取り出す
		if (arrowKeyCount == 0) {
			glfwWaitEvents();
		}
		else {
			glfwPollEvents();
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_RELEASE) {
			location[0] -= 2.0f / size[0];
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_RELEASE) {
			location[0] += 2.0f / size[0];
		}
		if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_RELEASE) {
			location[1] += 2.0f / size[1];
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_RELEASE) {
			location[1] -= 2.0f / size[1];
		}

		// マウスの左ボタンの状態を調べる
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
			// マウスカーソルの位置を取得する
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			// マウスカーソルの正規化デバイス座標系上での位置を求める
			location[0] = static_cast<GLfloat>(x) * 2.0f / size[0] - 1.0f;
			location[1] = 1.0f - static_cast<GLfloat>(y) * 2.0f / size[1];
		}
	}

	const GLfloat *getSize() const { return size; }

	// ワールド座標系に対するデバイス座標系の拡大率を取り出す
	GLfloat getScale() const { return scale; }

	// 位置を取り出す
	const GLfloat *getLocation() const { return location; }

	// ウィンドウのサイズ変更時の処理
	static void resize(GLFWwindow *const window, int width, int height) {
		// ウィンドウ全体をビューポートに設定する
		glViewport(0, 0, width, height);

		// このインスタンスのthisポインタを得る
		Window *const instance = getInstance(window);
		if (instance != NULL) {
			// 開いたウィンドウのサイズを保存する
			instance->size[0] = static_cast<GLfloat>(width);
			instance->size[1] = static_cast<GLfloat>(height);
		}
	}

	// マウスホイール操作時の処理
	static void wheel(GLFWwindow *const window, double x, double y) {
		// このインスタンスのthisポインタを得る
		Window *const instance = getInstance(window);
		if (instance != NULL) {
			// ワールド座標系に対するデバイス座標系の拡大率を更新する
			instance->scale += static_cast<GLfloat>(y);
			instance->wheelRotation += y;
		}
	}

	double getWheelRotation() {
		return wheelRotation;
	}

	// キーボード操作時の処理
	static void keyboad(GLFWwindow *const window, int key, int scancode, int action, int mods) {
		// このインスタンスのthisポインタを得る
		Window *const instance = getInstance(window);
		if (instance != NULL) {
			if (key != GLFW_KEY_LEFT && key != GLFW_KEY_RIGHT
				&& key != GLFW_KEY_UP && key != GLFW_KEY_DOWN) {
				return;
			}
			if (action == GLFW_RELEASE) {
				instance->arrowKeyCount -= 1;
			}
			else if (action == GLFW_PRESS) {
				instance->arrowKeyCount += 1;
			}
		}
	}

private:
	// ウィンドウのハンドル
	GLFWwindow *const window;

	// ウィンドウのサイズ
	GLfloat size[2];

	// ワールド座標系に対するデバイス座標系の拡大率
	GLfloat scale;

	// 図形の正規化デバイス座標系上での位置
	GLfloat location[2];

	// 矢印キーの押されている数
	int arrowKeyCount;

	// ホイールの回転量
	double wheelRotation;

	static Window *const getInstance(GLFWwindow *const window) {
		Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));
		return instance;
	}
};