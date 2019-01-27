#pragma once
#include <memory>
#include "Object.h"


class Shape {

public:
	// コンストラクタ
	//  size: 頂点の位置の次元
	//  vertexcount: 頂点の数
	//  vertex: 頂点属性を格納した配列
	//  indexcount: 頂点のインデックスの要素数
	//  index: 頂点のインデックスを格納した配列
	Shape(GLint size, GLsizei vertexcount, const Object::Vertex *vertex,
		GLsizei indexcount = 0, const GLuint *index = NULL)
		: object(new Object(size, vertexcount, vertex, indexcount, index))
		, vertexcount(vertexcount)
	{
	}

	// 描画
	void draw() const {
		// 頂点配列オブジェクトを結合する
		object->bind();
		// 描画を実行する
		execute();
	}

	// 描画の実行
	virtual void execute() const {
		// 折れ線で描画する
		glDrawArrays(GL_LINE_LOOP, 0, vertexcount);
	}

protected:
	// 描画に使う頂点の数
	const GLsizei vertexcount;

private:
	// 図形データ
	std::shared_ptr<const Object> object;

};