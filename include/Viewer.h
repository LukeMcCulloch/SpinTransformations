// =============================================================================
// SpinXForm -- Viewer.h
// Keenan Crane
// August 16, 2011
//
// Viewer is used to visualize the surface and handle user input.  It is
// essentially a wrapper around GLUT.  Since GLUT callbacks are specified via
// function pointers, callbacks (and the subsequent members they access) are
// declared static.
//

#ifndef SPINXFORM_VIEWER_H
#define SPINXFORM_VIEWER_H

#include "Mesh.h"
#include "Image.h"
#include "Quaternion.h"
#include <GLUT/glut.h>

class Viewer
{
   public:
      static void init( void );
      static Mesh mesh;
      static Image image;

   protected:

      // GLUT callbacks
      static void display( void );
      static void idle( void );
      static void keyboard( unsigned char c, int x, int y );
      static void mouse( int button, int state, int x, int y );
      static void motion( int x, int y );
      static void menu( int value );
      static void view( int value );

      // menu functions
      static void mTransform( void );
      static void mResetMesh( void );
      static void mWriteMesh( void );
      static void mExit( void );
      static void mSmoothShaded( void );
      static void mTextured( void );
      static void mWireframe( void );
      static void mError( void );
      static void mRho( void );
      static void mUVScaleUp( void );
      static void mUVScaleDown( void );

      // draw routines
      static void initGLUT( void );
      static void initGL( void );
      static void initLighting( void );
      static void initTexture( void );
      static void drawSurface( void );
      static void drawMesh( void );
      static void setMeshMaterial( void );
      static Vector vertex( int faceIndex, int whichVertex );
      static Vector faceNormal( int faceIndex );
      static Vector barycenter( int faceIndex );
      static void computeVertexNormals( void );
      static double quasiConformalDistortion( int faceIndex );
      static Vector HSV( double h, double s, double v );
      static Vector qcColor( double qc );
      static void printQCDistortion( void );
      static void updateDisplayList( void );
      static void reloadImage( void );

      // unique identifiers for menus
      enum
      {
         menuTransform,
         menuResetMesh,
         menuWriteMesh,
         menuExit,
         menuSmoothShaded,
         menuTextured,
         menuWireframe,
         menuError,
         menuRho,
         menuUVScaleUp,
         menuUVScaleDown
      };

      // draw state
      enum RenderMode
      {
         renderShaded,
         renderTextured,
         renderWireframe,
         renderRho,
         renderError
      };

      static RenderMode mode;
      static double rhoMax;
      static vector<Vector> vertexNormals;
      static double uvScale;

      // OpenGL handles
      static GLuint surfaceDL;  // surface display list
      static GLuint texture;    // checkerboard texture

      // camera state
      static Quaternion clickToSphere( int x, int y );
      static Quaternion pClick;   // mouse coordinates of current click
      static Quaternion pDrag;    // mouse coordinates of current drag
      static Quaternion pLast;    // mouse coordinates of previous drag
      static Quaternion rLast;    // previous camera rotation
      static Quaternion momentum; // camera momentum
      static int tLast;           // time of previous drag
};

#endif
