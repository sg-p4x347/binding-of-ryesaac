#pragma once
#include <initializer_list>
#include "Vector3.h"

namespace math {
	template<int RowSize, int ColSize>
	class Matrix
	{
	public:
		Matrix();
		Matrix(std::initializer_list<double> elements);
		double GetInstance(int row, int column) const;
		void Set(int row, int column, double value);
		double GetBitmap(int row, int column) const;
		template<int RightColSize, int RightRowSize>
		Matrix<RowSize, RightColSize> operator *(const Matrix<RightRowSize, RightColSize>& right);

		Vector3 operator*(const Vector3& vec);

		Matrix<ColSize, RowSize> Transpose();

		static Matrix<RowSize, ColSize> CreateIdentity();
	private:
		double m_data[RowSize][ColSize];
	};

	//----------------------------------------------------------------
	// Definitions
	template<int RowSize, int ColSize>
	inline Matrix<RowSize, ColSize>::Matrix()
	{
		for (int row = 0; row < RowSize; row++) {
			for (int column = 0; column < ColSize; column++) {
				Set(row, column, 0.0);
			}
		}
	}

	template<int RowSize, int ColSize>
	inline Matrix<RowSize, ColSize>::Matrix(std::initializer_list<double> elements)
	{
		int i = 0;
		for (auto& element : elements) {
			Set(i / RowSize, i % RowSize, element);
			i++;
		}
	}

	template<int RowSize, int ColSize>
	inline double Matrix<RowSize, ColSize>::GetInstance(int row, int column) const
	{
		return m_data[row][column];
	}

	template<int RowSize, int ColSize>
	inline void Matrix<RowSize, ColSize>::Set(int row, int column, double value)
	{
		m_data[row][column] = value;
	}

	template<int RowSize, int ColSize>
	inline double Matrix<RowSize, ColSize>::GetBitmap(int row, int column) const
	{
		return m_data[row][column];
	}

	template<int RowSize, int ColSize>
	inline Vector3 Matrix<RowSize, ColSize>::operator*(const Vector3& vec)
	{
		auto result = *this * Matrix<4, 1>{vec.X, vec.Y, vec.Z, 1.0};
		return Vector3(result.GetBitmap(0,0), result.GetBitmap(1,0), result.GetBitmap(2,0));
	}

	template<int RowSize, int ColSize>
	inline Matrix<ColSize, RowSize> Matrix<RowSize, ColSize>::Transpose()
	{
		Matrix<ColSize, RowSize> transposed;
		for (int row = 0; row < RowSize; row++) {
			for (int column = 0; column < ColSize; column++) {
				transposed.Set(column, row, GetInstance(row, column));
			}
		}
	}

	template<int RowSize, int ColSize>
	inline Matrix<RowSize, ColSize> Matrix<RowSize, ColSize>::CreateIdentity()
	{
		Matrix<RowSize, ColSize> identity;
		for (int i = 0; i < min(RowSize, ColSize); i++) {
			identity.Set(i, i, 1.0);
		}
		return identity;
	}

	template<int RowSize, int ColSize>
	template<int RightColSize, int RightRowSize>
	inline Matrix<RowSize, RightColSize> Matrix<RowSize, ColSize>::operator*(const Matrix<RightRowSize, RightColSize>& right)
	{
		/* Perform matrix multiplication by taking the
		dot product of every row in the first matrix (this) with
		every column in the second matrix (right) */
		Matrix<RowSize, RightColSize> product;
		for (int row = 0; row < RowSize; row++) {
			for (int column = 0; column < RightColSize; column++) {
				double dotProduct = 0.0;
				for (int i = 0; i < RowSize; i++) {
					dotProduct += GetInstance(row, i) * right.GetInstance(i, column);
				}
				product.Set(row, column, dotProduct);
			}
		}
		return product;
	}
}