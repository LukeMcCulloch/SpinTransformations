// ============================================================================
// SpinXForm -- CMWrapper.h
// Keenan Crane
// August 16, 2011
//
// CMWrapper provides an object-oriented interface for building C++
// applications that interface with CHOLMOD or other SuiteSparse packages using
// the CHOLMOD matrix format.  It includes wrapper classes for CHOLMOD dense
// and sparse matrices as well as cholmod_common objects.  Wrapper objects are
// mainly for convenience and may not always be your most efficient option.
// CHOLMOD has been tested with SuiteSparse 3.4.0 with METIS 4.0.1 and Intel
// TBB 2.2 on Max OS X 10.6.2.
// 
// In order to link against CHOLMOD you must:
// 
//    1. (optional:) download the Intel Threading Building Blocks library from
//       http://www.threadingbuildingblocks.org/
// 
//    2. download METIS from http://glaros.dtc.umn.edu/gkhome/metis/metis/overview
// 
//    3. download SuiteSparse from http://www.cise.ufl.edu/research/sparse/SuiteSparse/
// 
//    4. copy (and build, if necessary) the following Intel TBB libraries into
//       your library path:
//          libtbb.dylib
//          libtbb_debug.dylib
//          libtbbmalloc.dylib
//          libtbbmalloc_debug.dylib
// 
//    5. build and copy the following METIS libraries into your library path:
//          libmetis.a
// 
//    6. build and copy the following SuiteSparse libraries into your library path:
//          libamd.a
//          libcamd.a
//          libccolamd.a
//          libcholmod.a
//          libcolamd.a
//       Note: may be simpler to just type "make" in each of the appropriate
//       SuiteSparse/*/Lib subdirectories -- you do not have to build all of
//       SuiteSparse
//

#ifndef SPINXFORM_CMWRAPPER_H
#define SPINXFORM_CMWRAPPER_H

#include <cholmod.h>
#include <map>

// Object-oriented wrapper for CHOLMOD sparse matrix format.

namespace cm
{
   class Common
   {
      public:
         Common( void );
         ~Common( void );

         operator cholmod_common*( void );
         // allows cm::Common to be treated as a cholmod_common*

      protected:
         cholmod_common common;
   };

   class Dense
   {
      public:
         Dense( Common& common, int m = 0, int n = 0, int xtype = CHOLMOD_REAL );
         // initialize an mxn matrix of doubles
         // xtype is either CHOLMOD_REAL or CHOLMOD_ZOMPLEX
         
         Dense( const Dense& A );
         // copy constructor
         
         ~Dense( void );
         // destructor

         cholmod_dense* operator*( void );
         // dereference operator gets pointer to underlying cholmod_dense data structure

         const Dense& operator=( const Dense& A );
         // copies A

         const Dense& operator=( cholmod_dense* A );
         // gets pointer to A; will deallocate A upon destruction

         int size( int dim ) const;
         // returns the size of the dimension specified by scalar dim

         int length( void ) const;
         // returns the size of the largest dimension

         void zero( double rVal = 0., double iVal = 0. );
         // sets all elements to rVal+iVal*i

         double norm( void );
         // returns the infinity norm

         void horzcat( const Dense& A, const Dense& B );
         // replaces the current matrix with [ A, B ]

         void vertcat( const Dense& A, const Dense& B );
         // replaces the current matrix with [ A; B ]

         double& operator()( int row, int col );
         double  operator()( int row, int col ) const;
         double& r( int row, int col );
         double  r( int row, int col ) const;
         // access real part of element (row,col)
         // note: uses 0-based indexing

         double& i( int row, int col );
         double  i( int row, int col ) const;
         // access imaginary part of element (row,col)
         // note: uses 0-based indexing
         
         double& operator()( int index );
         double  operator()( int index ) const;
         // access real part of element ind of a vector
         // note: uses 0-based indexing
         
         double& r( int index );
         double  r( int index ) const;
         // access real part of element ind of a vector
         // note: uses 0-based indexing
         
         double& i( int index );
         double  i( int index ) const;
         // access imaginary part of element ind of a vector
         // note: uses 0-based indexing

      protected:
         void initializeFromCopy( void );

         Common& common;
         int m, n;
         int xtype;
         cholmod_dense* data;
         double* rData;
         double* iData;
   };

   class Sparse
   {
      public:
         Sparse( Common& common, int m = 0, int n = 0, int xtype = CHOLMOD_REAL );
         // initialize an mxn matrix of doubles
         // xtype is either CHOLMOD_REAL or CHOLMOD_ZOMPLEX

         ~Sparse( void );

         void resize( int m, int n );
         // clears and resizes to mxn matrix
         
         cholmod_sparse* operator*( void );
         // dereference operator gets pointer to underlying cholmod_sparse data structure

         int size( int dim ) const;
         // returns the size of the dimension specified by scalar dim

         int length( void ) const;
         // returns the size of the largest dimension

         void zero( double rVal = 0., double iVal = 0. );
         // sets all nonzero elements to rVal+iVal*i
         
         void transpose( void );
         // replaces this matrix with its transpose

         void horzcat( const Sparse& A, const Sparse& B );
         // replaces the current matrix with [ A, B ]

         void vertcat( const Sparse& A, const Sparse& B );
         // replaces the current matrix with [ A; B ]

         double& operator()( int row, int col );
         double  operator()( int row, int col ) const;
         double& r( int row, int col );
         double  r( int row, int col ) const;
         // access real part of element (row,col)
         // note: uses 0-based indexing

         double& i( int row, int col );
         double  i( int row, int col ) const;
         // access imaginary part of element (row,col)
         // note: uses 0-based indexing
         
         typedef std::pair<int,int> EntryIndex; // NOTE: column THEN row! (makes it easier to build compressed format)
         typedef std::pair<double,double> EntryValue;
         typedef std::map<EntryIndex,EntryValue> EntryMap;
         typedef EntryMap::iterator       iterator;
         typedef EntryMap::const_iterator const_iterator;

               iterator begin( void );
         const_iterator begin( void ) const;
               iterator   end( void );
         const_iterator   end( void ) const;

      protected:
         Common& common;
         int m, n;
         int xtype, stype;
         cholmod_sparse* A;
         EntryMap data;

         double& retrieveEntry( int row, int col, int c );
         double  retrieveEntry( int row, int col, int c ) const;
   };

   class Upper : public Sparse
   {
      public:
         Upper( Common& common, int m = 0, int n = 0, int xtype = CHOLMOD_REAL );
         // initialize an mxn matrix of doubles
         // xtype is either CHOLMOD_REAL or CHOLMOD_ZOMPLEX
   };

   class Factor
   {
      public:
         Factor( Common& common );
         ~Factor( void );

         void build( Upper& A );
         // factorizes positive-definite matrix A using CHOLMOD

         cholmod_factor* operator*( void );
         // dereference operator gets pointer to underlying cholmod_factor data structure

      protected:
         Common& common;
         cholmod_factor *L;
   };
}

#endif
