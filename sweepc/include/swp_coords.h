/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    Defines coordinate structures and transform functions.
*/

#ifndef SWEEP_COORDS_H
#define SWEEP_COORDS_H

#include "swp_params.h"
#include <cmath>
#include <vector>

namespace Sweep
{
namespace Coords
{
    // A 3D cartesian position vector.
    class Vector
    {
    private:
        real A[3]; // Vector data.
    public:
        // Constructor.
        Vector() {memset(&A[0], 0, sizeof(real)*3);}
        // Cartesian coordinates.
        inline real &X(void) {return A[0];}
        inline real &Y(void) {return A[1];}
        inline real &Z(void) {return A[2];}
        // Operators to return vector elements.
        real &operator[](int i) {return A[0];}
        const real &operator[](int i) const {return A[0];}

        // VECTOR TRANSLATION.

        // Translates the vector the given deviations.
        inline void Translate(real dx, real dy, real dz) {A[0]+=dx; A[1]+=dy; A[2]+=dz;}
        inline void Translate(Vector D) {A[0]+=D[0]; A[1]+=D[1]; A[2]+=D[2];}

    };

    // A 3D coordinate transform matrix.
    class Matrix 
    {
    private:
        real A[3][3]; // The matrix data [row][col].
    public:
        // Constructor.
        Matrix() {memset(&A[0][0], 0, sizeof(real)*9);}

        // OPERATORS.

        // Operators to return matrix elements.
        real* operator[](int i) {return &A[i][0];}
        const real *const operator[](int i) const {return &A[i][0];}

        // MATRIX MULTIPLICATION.

        // Performs matrix multiplication.  Returns
        // the resultant matrix.
        inline Matrix Mult(const Matrix &B) const
        {
            // C = A X B.
            Matrix C;
            C[0][0] = (A[0][0]*B[0][0]) + (A[0][1]*B[1][0]) + (A[0][2]*B[2][0]);
            C[0][1] = (A[0][0]*B[0][1]) + (A[0][1]*B[1][1]) + (A[0][2]*B[2][1]);
            C[0][2] = (A[0][0]*B[0][2]) + (A[0][1]*B[1][2]) + (A[0][2]*B[2][2]);
            C[1][0] = (A[1][0]*B[0][0]) + (A[1][1]*B[1][0]) + (A[1][2]*B[2][0]);
            C[1][1] = (A[1][0]*B[0][1]) + (A[1][1]*B[1][1]) + (A[1][2]*B[2][1]);
            C[1][2] = (A[1][0]*B[0][2]) + (A[1][1]*B[1][2]) + (A[1][2]*B[2][2]);
            C[2][0] = (A[2][0]*B[0][0]) + (A[2][1]*B[1][0]) + (A[2][2]*B[2][0]);
            C[2][1] = (A[2][0]*B[0][1]) + (A[2][1]*B[1][1]) + (A[2][2]*B[2][1]);
            C[2][2] = (A[2][0]*B[0][2]) + (A[2][1]*B[1][2]) + (A[2][2]*B[2][2]);
            return C;
        }

        // Performs matrix multiplication on a vector.
        inline Vector Mult(const Vector &B) const
        {
            // C = A X B.
            Vector C;
            C[0] = (A[0][0]*B[0]) + (A[0][1]*B[0]) + (A[0][2]*B[0]);
            C[1] = (A[1][0]*B[1]) + (A[1][1]*B[1]) + (A[1][2]*B[1]);
            C[2] = (A[2][0]*B[2]) + (A[2][1]*B[2]) + (A[2][2]*B[2]);
            return C;
        }
        
        // IDENTITY MATRIX.

        // Returns the identity matrix.
        static inline Matrix Identity(void)
        {
            Matrix I;
            I[0][0] = I[1][1] = I[2][2] = 1.0;
            return I;
        }

        // Sets this matrix to the identity matrix.
        inline void SetIdentity(void)
        {
            A[0][0] = A[1][1] = A[2][2] = 1.0;
            A[0][1] = A[0][2] = A[1][0] = 0.0;
            A[1][2] = A[2][0] = A[2][1] = 0.0;
        }

        // COORDINATE ROTATION.

        // Adds a rotation to the transform matrix around the Z-axis.
        inline void RotateZ(real phi)
        {
            // Precalculate cos and sin.
            real cosp = cos(phi);
            real sinp = sin(phi);

            // This function requires one temporary storage variable (a0)
            // to account for a change in the left column before it is used.

            // Top row.
            real a0 = A[0][0];
            A[0][0] = + (a0*cosp) + (A[0][1]*sinp);
            A[0][1] = - (a0*sinp) + (A[0][1]*cosp);
            //A[0][2] = A[0][2];
            // Middle row.
            a0 = A[1][0];
            A[1][0] = + (a0*cosp) + (A[1][1]*sinp);
            A[1][1] = - (a0*sinp) + (A[1][1]*cosp);
            //A[1][2] = A[1][2];
            // Bottom row.
            a0 = A[2][0];
            A[2][0] = + (a0*cosp) + (A[2][1]*sinp);
            A[2][1] = - (a0*sinp) + (A[2][1]*cosp);
            //A[2][2] = A[2][2];
        }

        // Sets the matrix to be a transform matrix for rotation around
        // the z-axis.
        inline void SetRotZ(real phi)
        {
            A[0][2] =   A[1][2] = A[2][0] = A[2][1] = 0.0;
            A[0][0] =   (A[1][1] = cos(phi));
            A[0][1] = - (A[1][0] = sin(phi));
            A[2][2] =   1.0;
        }

        // Adds a rotation to the transform matrix around the x-axis
        inline void RotateX(real theta)
        {
            // Precalculate cos and sin.
            real cosp = cos(theta);
            real sinp = sin(theta);

            // This function requires one temporary storage variable (a1)
            // to account for a change in the middle column before it is used.

            // Top row.
            real a1 = A[0][1];
            //A[0][0] = A[0][0];
            A[0][1] = + (a1*cosp) + (A[0][2]*sinp);
            A[0][2] = - (a1*sinp) + (A[0][2]*cosp);
            // Middle row.
            a1 = A[1][1];
            //A[1][0] = A[1][0];
            A[1][1] = + (a1*cosp) + (A[1][2]*sinp);
            A[1][2] = - (a1*sinp) + (A[1][2]*cosp);
            // Bottom row.
            a1 = A[2][1];
            //A[2][0] = A[2][0];
            A[2][1] = + (a1*cosp) + (A[2][2]*sinp);
            A[2][2] = - (a1*sinp) + (A[2][2]*cosp);
       }

        // Sets the matrix to be a transform matrix for rotation
        // about the x-axis.
        inline void SetRotX(real theta)
        {
            A[0][0] =   1.0;
            A[0][1] =   A[0][2] = A[1][0] = A[2][0] = 0.0;
            A[1][1] =   (A[2][2] = cos(theta));
            A[1][2] = - (A[2][1] = sin(theta));
        }

        // Sets the matrix to be a transform matrix for rotations
        // about the x-axis and the z-axis.
        inline void Rotate(
            real theta, // X-axis rotation (radians).
            real phi    // Z-axis rotation (radians).
            )
        {
            // M = Z x X.

            // Precalculate trig terms.
            real sinp = sin(phi);
            real cosp = cos(phi);
            real sint = sin(theta);
            real cost = cos(theta);
            // Set matrix.
            A[0][0] =   cosp;
            A[0][1] = - sinp * cost;
            A[0][2] =   sinp * sint;
            A[1][0] =   sinp;
            A[1][1] =   cosp * cost;
            A[1][2] = - cosp * sint;
            A[2][0] =   0.0;
            A[2][1] =   sint;
            A[2][2] =   cost;
        }
    };
};
};
#endif
