#pragma once
#include <cmath>
#include <algorithm>
#include <GL/glew.h>

// �ϊ��s��
class Matrix {
public:
	// �R���X�g���N�^
	Matrix() {}

	// �z��̓��e�ŏ���������R���X�g���N�^
	//  a: GLfloat�^��16�v�f�̔z��
	Matrix(const GLfloat *a) {
		std::copy(a, a + 16, matrix);
	}

	// ��Z
	Matrix operator*(const Matrix &m) const {
		Matrix t;
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				const int ji(j * 4 + i);

				t.matrix[ji] = 0.0f;
				for (int k = 0; k < 4; k++) {
					t.matrix[ji] += matrix[k * 4 + i] * m.matrix[j * 4 + k];
				}
			}
		}
		return t;
	}

	// �ϊ��s��̔z���Ԃ�
	const GLfloat *data() const {
		return matrix;
	}

	// �P�ʍs���ݒ肷��
	void loadIdentity() {
		std::fill(matrix, matrix + 16, 0.0f);
		matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
	}

	// �P�ʍs����쐬����
	static Matrix identity() {
		Matrix t;
		t.loadIdentity();
		return t;
	}

	// (x, y, z)�������s�ړ�����ϊ��s����쐬����
	static Matrix translate(GLfloat x, GLfloat y, GLfloat z) {
		Matrix t;

		t.loadIdentity();
		t.matrix[12] = x;
		t.matrix[13] = y;
		t.matrix[14] = z;
		return t;
	}

	// (x, y, z)�{�Ɋg��k������ϊ��s����쐬����
	static Matrix scale(GLfloat x, GLfloat y, GLfloat z) {
		Matrix t;

		t.loadIdentity();
		t.matrix[0] = x;
		t.matrix[5] = y;
		t.matrix[10] = z;
		return t;
	}

	// y���W��s�{�����l��x���W�ɑ����ϊ��s����쐬����
	static Matrix shearXY(GLfloat s) {
		return shear(4, s);
	}
	// z���W��s�{�����l��y���W�ɑ����ϊ��s����쐬����
	static Matrix shearYZ(GLfloat s) {
		return shear(9, s);
	}
	// x���W��s�{�����l��z���W�ɑ����ϊ��s����쐬����
	static Matrix shearZX(GLfloat s) {
		return shear(2, s);
	}
	// x���W��s�{�����l��y���W�ɑ����ϊ��s����쐬����
	static Matrix shearYX(GLfloat s) {
		return shear(1, s);
	}
	// y���W��s�{�����l��z���W�ɑ����ϊ��s����쐬����
	static Matrix shearZY(GLfloat s) {
		return shear(6, s);
	}
	// z���W��s�{�����l��x���W�ɑ����ϊ��s����쐬����
	static Matrix shearXZ(GLfloat s) {
		return shear(8, s);
	}

	// x������ɃƉ�]����ϊ��s����쐬����
	static Matrix rotateX(GLfloat theta) {
		Matrix t;

		t.loadIdentity();
		t.matrix[5] = cos(theta);
		t.matrix[6] = sin(theta);
		t.matrix[9] = -sin(theta);
		t.matrix[10] = cos(theta);
		return t;
	}
	// y������ɃƉ�]����ϊ��s����쐬����
	static Matrix rotateY(GLfloat theta) {
		Matrix t;

		t.loadIdentity();
		t.matrix[0] = cos(theta);
		t.matrix[2] = -sin(theta);
		t.matrix[8] = sin(theta);
		t.matrix[11] = cos(theta);
		return t;
	}
	// z�������theta��]����ϊ��s����쐬����
	static Matrix rotateZ(GLfloat theta) {
		Matrix t;

		t.loadIdentity();
		t.matrix[0] = cos(theta);
		t.matrix[1] = sin(theta);
		t.matrix[4] = -sin(theta);
		t.matrix[5] = cos(theta);
		return t;
	}
	// (x, y, z)������ theta��]����ϊ��s����쐬����
	static Matrix rotate(GLfloat theta, GLfloat x, GLfloat y, GLfloat z) {
		Matrix t;
		const GLfloat d(sqrt(x*x + y*y + z*z));

		if (d > 0.0f) {
			const GLfloat l(x / d), m(y / d), n(z / d);
			const GLfloat l2(l * l), m2(m * m), n2(n * n);
			const GLfloat lm(l * m), mn(m * n), nl(n * l);
			const GLfloat c(cos(theta)), c1(1.0f - c), s(sin(theta));

			t.loadIdentity();
			t.matrix[0] = (1.0f - l2) * c + l2;
			t.matrix[1] = lm * c1 + n * s;
			t.matrix[2] = nl * c1 - m * s;
			t.matrix[4] = lm * c1 - n * s;
			t.matrix[5] = (1.0f - m2) * c + m2;
			t.matrix[6] = mn * c1 + l * s;
			t.matrix[8] = nl * c1 + m * s;
			t.matrix[9] = mn * c1 - l * s;
			t.matrix[10] = (1.0f - n2) * c + n2;
		}
		return t;
	}

	// �r���[�ϊ��s����쐬����
	static Matrix lookat(
		GLfloat ex, GLfloat ey, GLfloat ez,		// ���_�̈ʒu
		GLfloat gx, GLfloat gy, GLfloat gz,		// �ڕW�_�̈ʒu
		GLfloat ux, GLfloat uy, GLfloat uz) {	// ������̃x�N�g��

		// ���s�ړ��̕ϊ��s��
		const Matrix tv(translate(-ex, -ey, -ez));

		// t�� = e - g
		const GLfloat tx(ex - gx);
		const GLfloat ty(ey - gy);
		const GLfloat tz(ez - gz);

		// r�� = u x t��
		const GLfloat rx(uy * tz - uz * ty);
		const GLfloat ry(uz * tx - ux * tz);
		const GLfloat rz(ux * ty - uy * tx);

		// s�� = t�� x r��
		const GLfloat sx(ty * rz - tz * ry);
		const GLfloat sy(tz * rx - tx * rz);
		const GLfloat sz(tx * ry - ty * rx);


		// s���̒������`�F�b�N
		const GLfloat s2(sx * sx + sy * sy + sz * sz);
		if (s2 == 0.0f) return tv;

		// ��]�̕ϊ��s��
		Matrix rv;
		rv.loadIdentity();

		// r���𐳋K�����Ĕz��ϐ��Ɋi�[
		const GLfloat r(sqrt(rx * rx + ry * ry + rz * rz));
		rv.matrix[0] = rx / r;
		rv.matrix[4] = ry / r;
		rv.matrix[8] = rz / r;

		// s���𐳋K�����Ĕz��ϐ��Ɋi�[
		const GLfloat s(sqrt(s2));
		rv.matrix[1] = sx / s;
		rv.matrix[5] = sy / s;
		rv.matrix[9] = sz / s;

		// t���𐳋K�����Ĕz��ϐ��Ɋi�[
		const GLfloat t(sqrt(tx * tx + ty * ty * tz * tz));
		rv.matrix[2] = tx / t;
		rv.matrix[6] = ty / t;
		rv.matrix[10] = tz / t;

		return rv * tv;
	}

	// ���s���e�ϊ��s����쐬����
	static Matrix orthogonal(
		GLfloat left, GLfloat right,
		GLfloat bottom, GLfloat top,
		GLfloat zNear, GLfloat zFar) {

		Matrix t;
		const GLfloat dx(right - left);
		const GLfloat dy(top - bottom);
		const GLfloat dz(zFar - zNear);

		if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
			t.loadIdentity();
			t.matrix[0] = 2.0f / dx;
			t.matrix[5] = 2.0f / dy;
			t.matrix[10] = -2.0f / dz;
			t.matrix[12] = -(right + left) / dx;
			t.matrix[13] = -(top + bottom) / dy;
			t.matrix[14] = -(zFar + zNear) / dz;
		}
		return t;
	}

	// �������e�ϊ��s����쐬����
	static Matrix frustum(
		GLfloat left, GLfloat right,
		GLfloat bottom, GLfloat top,
		GLfloat zNear, GLfloat zFar) {

		Matrix t;
		const GLfloat dx(right - left);
		const GLfloat dy(top - bottom);
		const GLfloat dz(zFar - zNear);

		if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
			t.loadIdentity();
			t.matrix[0] = 2.0f * zNear / dx;
			t.matrix[5] = 2.0f * zNear / dy;
			t.matrix[8] = (right + left) / dx;
			t.matrix[9] = (top + bottom) / dy;
			t.matrix[10] = -(zFar + zNear) / dz;
			t.matrix[11] = -1.0f;
			t.matrix[14] = -2.0f * zFar * zNear / dz;
			t.matrix[15] = 0.0f;
		}
		return t;
	}

	// ��p���w�肵�ē������e�ϊ��s����쐬����
	static Matrix perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
		Matrix t;
		const GLfloat dz(zFar - zNear);

		if (dz != 0.0f) {
			t.loadIdentity();
			t.matrix[5] = 1.0f / tan(fovy * 0.5f);
			t.matrix[0] = t.matrix[5] / aspect;
			t.matrix[10] = -(zFar + zNear) / dz;
			t.matrix[11] = -1.0f;
			t.matrix[14] = -2.0f * zFar * zNear / dz;
			t.matrix[15] = 0.0f;
		}

		return t;
	}

private:
	// �ϊ��s��̗v�f
	GLfloat matrix[16];

	static Matrix shear(int index, GLfloat s) {
		Matrix t;

		t.loadIdentity();
		t.matrix[index] = s;
		return t;
	}
};