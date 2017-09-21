#ifndef _VECTOR
#define _VECTOR

#include <math.h>

class Vector {
	private:
		double x, y, z;

	public:
		//default constructor

		Vector(double X = 0, double Y = 0, double Z = 0) {
			x = X;
			y = Y;
			z = Z;
		}
		~Vector(){};

		//calculate and return the magnitude of this vector

		double GetMagnitude() {
			return sqrt(x * x + y * y + z * z);
		}

		//multiply this vector by a scalar

		Vector operator*(double num) const {
			return Vector(x * num, y * num, z * num);
		}

		//pass in a vector, pass in a scalar, return the product

		friend Vector operator*(double num, Vector const &vec) {
			return Vector(vec.x * num, vec.y * num, vec.z * num);
		}

		//add two vectors

		Vector operator+(const Vector &vec) const {
			return Vector(x + vec.x, y + vec.y, z + vec.z);
		}

		//subtract two vectors

		Vector operator-(const Vector &vec) const {
			return Vector(x - vec.x, y - vec.y, z - vec.z);
		}

		//normalize this vector

		void normalizeVector() {
			double magnitude = sqrt(x * x + y * y + z * z);
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}

		//calculate and return dot product

		double dot(const Vector &vec) const {
			return x * vec.x + y * vec.y + z * vec.z;
		}

		//calculate and return cross product

		Vector cross(const Vector &vec) const {
			return Vector(y * vec.z - z * vec.y,
					z * vec.x - x * vec.z,
					x * vec.y - y * vec.x);
		}
};

#endif

