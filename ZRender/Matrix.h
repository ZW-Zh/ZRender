#ifndef _MATRIX_
#define _MATRIX_
#include <iostream>
#include "global.h"
#include "Vector3f.h"
class Matrix4f
{
public:
	float m[4][4];
	//返回单位矩阵
	Matrix4f();
	
	friend std::ostream& operator<< (std::ostream& os, const Matrix4f& matrix);
	friend Matrix4f& operator>> (Matrix4f& matrix, float(&arr)[16]);

	Matrix4f operator* (const Matrix4f& matrix) const;

	Matrix4f& operator=(const Matrix4f& matrix);

	Matrix4f operator+ (const Matrix4f& matrix) const;
	Matrix4f operator- (const Matrix4f& matrix) const;
};

inline Matrix4f::Matrix4f()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j) m[i][j] = 1;
			else m[i][j] = 0;
		}
	}
}

inline Matrix4f Matrix4f::operator*(const Matrix4f& matrix) const
{
	// TODO: 在此处插入 return 语句
	Matrix4f m;
	//m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0] + m[0][3] * matrix.m[3][0];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				m.m[i][j] += m.m[i][k] * matrix.m[k][j];
			}
		}
	}
	return m;
}

inline Matrix4f& Matrix4f::operator=(const Matrix4f& matrix) 
{
	// TODO: 在此处插入 return 语句
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = matrix.m[i][j];
		}
	}
	return *this;
}

inline Matrix4f Matrix4f::operator+(const Matrix4f& matrix) const
{
	Matrix4f m;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m.m[i][j] = matrix.m[i][j]+this->m[i][j];
		}
	}
	return m;
}

inline Matrix4f Matrix4f::operator-(const Matrix4f& matrix) const
{
	Matrix4f m;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m.m[i][j] = matrix.m[i][j] - this->m[i][j];
		}
	}
	return m;
}

inline std::ostream& operator<< (std::ostream& os, const Matrix4f& matrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (j == 3) os << matrix.m[i][j] << std::endl;
			else os << matrix.m[i][j] << ",";
		}
	}
	return os;
}

inline Matrix4f& operator>> (Matrix4f& matrix, float(&arr)[16])
{
	int n = getArrayLen(arr);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if ((i * 4 + j) >= n)
			{
				matrix.m[i][j] = 0;
			}
			else
			{
				matrix.m[i][j] = arr[i * 4 + j];
			}
		}
	}
	return matrix;
}

class Matrix3f
{
public:
	float m[3][3];
	//返回单位矩阵
	Matrix3f();

	friend std::ostream& operator<< (std::ostream& os, const Matrix3f& matrix);
	friend Matrix3f& operator>> (Matrix3f& matrix, float(&arr)[9]);

	Vector3f operator* (const Vector3f& vector) const;
	Matrix3f operator* (const Matrix3f& matrix) const;
	Matrix3f& operator=(const Matrix3f& matrix);

	Matrix3f operator+ (const Matrix3f& matrix) const;
	Matrix3f operator- (const Matrix3f& matrix) const;

};
inline Matrix3f::Matrix3f()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == j) m[i][j] = 1;
			else m[i][j] = 0;
		}
	}
}


inline Vector3f Matrix3f::operator*(const Vector3f& vector) const
{
	float x = m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z;
	float y = m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z;
	float z = m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z;
	return Vector3f(x,y,z);
}

inline Matrix3f Matrix3f::operator*(const Matrix3f& matrix) const
{
	// TODO: 在此处插入 return 语句
	Matrix3f m;
	//m[0][0] = m[0][0] * matrix.m[0][0] + m[0][1] * matrix.m[1][0] + m[0][2] * matrix.m[2][0] + m[0][3] * matrix.m[3][0];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				m.m[i][j] += m.m[i][k] * matrix.m[k][j];
			}
		}
	}
	return m;
}

inline Matrix3f& Matrix3f::operator=(const Matrix3f& matrix)
{
	// TODO: 在此处插入 return 语句
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m[i][j] = matrix.m[i][j];
		}
	}
	return *this;
}

inline Matrix3f Matrix3f::operator+(const Matrix3f& matrix) const
{
	Matrix3f m;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m.m[i][j] = matrix.m[i][j] + this->m[i][j];
		}
	}
	return m;
}

inline Matrix3f Matrix3f::operator-(const Matrix3f& matrix) const
{
	Matrix3f m;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m.m[i][j] = matrix.m[i][j] - this->m[i][j];
		}
	}
	return m;
}

inline std::ostream& operator<< (std::ostream& os, const Matrix3f& matrix)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (j == 2) os << matrix.m[i][j] << std::endl;
			else os << matrix.m[i][j] << ",";
		}
	}
	return os;
}

inline Matrix3f& operator>> (Matrix3f& matrix, float(&arr)[9])
{
	int n = getArrayLen(arr);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if ((i * 3 + j) >= n)
			{
				matrix.m[i][j] = 0;
			}
			else
			{
				matrix.m[i][j] = arr[i * 3 + j];
			}
		}
	}
	return matrix;
}
#endif // !_MATRIX4F_


