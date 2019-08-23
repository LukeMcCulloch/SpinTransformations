// =============================================================================
// SpinXForm -- LinearSolver.cpp
// Keenan Crane
// August 16, 2011
//

#include "LinearSolver.h"

cm::Common cc;

void LinearSolver :: solve( Factor& A,
                     vector<Quaternion>& x,
                     const vector<Quaternion>& b )
// solves the linear system Ax = b where A is positive-semidefinite
{
   int n = x.size();
   Dense result( cc, n*4, 1 );
   Dense    rhs( cc, n*4, 1 );

   // convert right-hand side to real values
   toReal( b, rhs );

   // solve real linear system
   result = cholmod_l_solve( CHOLMOD_A, *A, *rhs, cc );

   // convert solution back to quaternions
   toQuat( result, x );
}

void LinearSolver :: toReal( const vector<Quaternion>& uQuat,
                             Dense& uReal )
// converts vector from quaternion- to real-valued entries
{
   for( size_t i = 0; i < uQuat.size(); i++ )
   {
      uReal(i*4+0) = uQuat[i].re();   // real
      uReal(i*4+1) = uQuat[i].im().x; // i
      uReal(i*4+2) = uQuat[i].im().y; // j
      uReal(i*4+3) = uQuat[i].im().z; // k
   }
}

void LinearSolver :: toQuat( const Dense& uReal,
                             vector<Quaternion>& uQuat )
// converts vector from real- to quaternion-valued entries
{
   for( size_t i = 0; i < uQuat.size(); i++ )
   {
      uQuat[i] = Quaternion( uReal(i*4+0),  // real
                             uReal(i*4+1),  // i
                             uReal(i*4+2),  // j
                             uReal(i*4+3)); // k
   }
}

