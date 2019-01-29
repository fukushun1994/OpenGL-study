#pragma once

#include "Shape.h"

class SolidShape : public Shape {
public:
	// �R���X�g���N�^
	//  size: ���_�̈ʒu�̎���
	//  vertexcount: ���_�̐�
	//  vertex: ���_�������i�[�����z��
	//  indexcount: ���_�̃C���f�b�N�X�̗v�f��
	//  index: ���_�̃C���f�b�N�X���i�[�����z��
	SolidShape(GLint size, GLsizei vertexcount, const Object::Vertex *vertex)
		: Shape(size, vertexcount, vertex)
	{
	}

	// �`��̎��s
	virtual void execute() const {
		// �O�p�`�ŕ`�悷��
		glDrawArrays(GL_TRIANGLES, 0, vertexcount);
	}
};