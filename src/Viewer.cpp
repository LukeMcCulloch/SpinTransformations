// =============================================================================
// SpinXForm -- Viewer.cpp
// Keenan Crane
// August 16, 2011
//

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Utility.h"
#include "Viewer.h"

using namespace std;

// INITIALIZATION --------------------------------------------------------------

// declare static member variables
Mesh Viewer::mesh;
Image Viewer::image;
double Viewer::uvScale = 1.;
double Viewer::rhoMax;
Quaternion Viewer::rLast = 1.;
Quaternion Viewer::pClick = 1.;
Quaternion Viewer::pDrag = 1.;
Quaternion Viewer::pLast = 1.;
Quaternion Viewer::momentum = 1.;
int Viewer::tLast;
Viewer::RenderMode Viewer::mode;
GLuint Viewer::texture = 0;
GLuint Viewer::surfaceDL = 0;
vector<Vector> Viewer::vertexNormals;

void Viewer :: init( void )
{
   initGLUT();
   initGL();

   mode = renderShaded;
   computeVertexNormals();
   mesh.setCurvatureChange( image, 5. );

   updateDisplayList();

   glutMainLoop();
}

void Viewer :: initGLUT( void )
{
   int argc = 0;

   // initialize window
   glutInitWindowSize( 512, 512 );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInit( &argc, NULL );
   glutCreateWindow( "SpinXForm" );

   // specify callbacks
   glutDisplayFunc  ( Viewer::display  );
   glutIdleFunc     ( Viewer::idle     );
   glutKeyboardFunc ( Viewer::keyboard );
   glutMouseFunc    ( Viewer::mouse    );
   glutMotionFunc   ( Viewer::motion   );

   // initialize menus
   int viewMenu = glutCreateMenu( Viewer::view );
   glutAddMenuEntry( "[s] Smooth Shaded",  menuSmoothShaded );
   glutAddMenuEntry( "[f] Wireframe",      menuWireframe    );
   glutAddMenuEntry( "[e] QC Error",       menuError        );
   glutAddMenuEntry( "[p] Rho",            menuRho          );
   glutAddMenuEntry( "[t] Textured",       menuTextured     );
   glutAddMenuEntry( "[-] Shrink Texture", menuUVScaleDown  );
   glutAddMenuEntry( "[+] Grow Texture",   menuUVScaleUp    );

   int mainMenu = glutCreateMenu( Viewer::menu );
   glutSetMenu( mainMenu );
   glutAddMenuEntry( "[space] Transform", menuTransform   );
   glutAddMenuEntry( "[r] Reset Mesh",    menuResetMesh   );
   glutAddMenuEntry( "[w] Write Mesh",    menuWriteMesh   );
   glutAddMenuEntry( "[q] Exit",          menuExit        );
   glutAddSubMenu( "View", viewMenu );
   glutAttachMenu( GLUT_RIGHT_BUTTON );
}

void Viewer :: initGL( void )
{
   //glClearColor( .3, .3, .5, 1. );
   glClearColor( 1., 1., 1., 1. );

   initLighting();
   initTexture();
}

void Viewer :: initLighting( void )
{
   GLfloat position[4] = { 20., 30., 40., 0. };
   glLightfv( GL_LIGHT0, GL_POSITION, position );

   glEnable( GL_LIGHT0 );

   glEnable( GL_NORMALIZE );
}

void Viewer :: initTexture( void )
{
   const int N = 64;
   float texels[ N*N ];
   bool useCheckerboard = true;
   bool useDots = false;

   if( useCheckerboard )
   {
      for( int i = 0; i < N; i++ )
      for( int j = 0; j < N; j++ )
      {
         texels[ i*N + j ] = ((i+j)%2)*.25 + .75;
      }
   }

   if( useDots )
   {
      const double radius = 30.;

      for( int I = 0; I < N; I++ )
      for( int J = 0; J < N; J++ )
      {
         double x = (double)I / radius;
         double i = round( x/(sqrt(3.)/2.) );
         double y = (double)J / radius - .5*fmod(i,2.);
         double j = round( y );
         double u = i*sqrt(3.)/2.;
         double v = j;
         double d = sqrt( (x-u)*(x-u) + (y-v)*(y-v) );
         if( d < sqrt(3.)/4. ) texels[I*N+J] = .5;
         else                  texels[I*N+J] = 1.;
      }
   }

   glGenTextures( 1, &texture );

   glBindTexture( GL_TEXTURE_2D, texture );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, N, N, 0, GL_LUMINANCE, GL_FLOAT, (GLvoid*) texels );
}


// MENU CALLBACKS --------------------------------------------------------------

void Viewer :: menu( int value )
{
   switch( value )
   {
      case( menuTransform ):
         mTransform();
         break;
      case( menuResetMesh ):
         mResetMesh();
         break;
      case( menuWriteMesh ):
         mWriteMesh();
         break;
      case( menuExit ):
         mExit();
         break;
      default:
         break;
   }
}

void Viewer :: view( int value )
{
   switch( value )
   {
      case( menuSmoothShaded ):
         mSmoothShaded();
         break;
      case( menuTextured ):
         mTextured();
         break;
      case( menuWireframe ):
         mWireframe();
         break;
      case( menuError ):
         mError();
         break;
      case( menuRho ):
         mRho();
         break;
      case( menuUVScaleUp ):
         mUVScaleUp();
         break;
      case( menuUVScaleDown ):
         mUVScaleDown();
         break;
      default:
         break;
   }
}

void Viewer :: mTransform( void )
{
   reloadImage();
   mesh.updateDeformation();
   computeVertexNormals();
   updateDisplayList();
   printQCDistortion();
}

void Viewer :: mResetMesh( void )
{
   mesh.resetDeformation();
   computeVertexNormals();
   updateDisplayList();
}

void Viewer :: mWriteMesh( void )
{
   mesh.write( "result.obj" );
}

void Viewer :: mExit( void )
{
   exit( 0 );
}

void Viewer :: mSmoothShaded( void )
{
   mode = renderShaded;
   updateDisplayList();
}

void Viewer :: mTextured( void )
{
   mode = renderTextured;
   updateDisplayList();
}

void Viewer :: mWireframe( void )
{
   mode = renderWireframe;
   updateDisplayList();
}

void Viewer :: mError( void )
{
   mode = renderError;
   updateDisplayList();
}

void Viewer :: mRho( void )
{
   mode = renderRho;
   updateDisplayList();
}

void Viewer :: mUVScaleUp( void )
{
   uvScale *= 1.1;
   updateDisplayList();
}

void Viewer :: mUVScaleDown( void )
{
   uvScale /= 1.1;
   updateDisplayList();
}


// KEYBOARD CALLBACK -----------------------------------------------------------

void Viewer :: keyboard( unsigned char c, int x, int y )
{
   switch( c )
   {
      case 27:
         mExit();
         break;
      case 'e':
         mError();
         break;
      case 'p':
         mRho();
         break;
      case 's':
         mSmoothShaded();
         break;
      case 't':
         mTextured();
         break;
      case 'f':
         mWireframe();
         break;
      case 'w':
         mWriteMesh();
         break;
      case 'r':
         mResetMesh();
         break;
      case ' ':
         mTransform();
         break;
      case '_':
         mUVScaleDown();
         break;
      case '-':
         mUVScaleDown();
         break;
      case '=':
         mUVScaleUp();
         break;
      case '+':
         mUVScaleUp();
         break;
      case 'q':
         mExit();
         break;
      default:
         break;
   }
}


// DRAW ROUTINES ---------------------------------------------------------------

void Viewer :: display( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();

   int viewport[4];
   glGetIntegerv( GL_VIEWPORT, viewport );
   double aspect = (double) viewport[2] / (double) viewport[3];
   const double fovy = 50.;
   const double near = .01;
   const double far = 1000.;
   gluPerspective( fovy, aspect, near, far );

   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   gluLookAt( 0., 0., -2.5, 0., 0., 0., 0., 1., 0. );

   Quaternion r = ( pDrag * ( ~pClick )) * rLast;

   double w = r[0];
   double x = r[1];
   double y = r[2];
   double z = r[3];
   GLdouble M[16] = {
      1.-2.*y*y-2.*z*z, 2.*x*y+2.*w*z, 2.*x*z-2.*w*y, 0.,
      2.*x*y-2.*w*z, 1.-2.*x*x-2.*z*z, 2.*y*z+2.*w*x, 0.,
      2.*x*z+2.*w*y, 2.*y*z-2.*w*x, 1.-2.*x*x-2.*y*y, 0.,
      0., 0., 0., 1.
   };
   glMultMatrixd( M );

   drawSurface();

   glutSwapBuffers();
}

void Viewer :: setMeshMaterial( void )
{
   GLfloat     zero[4] = { 0., 0., 0., 1. };
   GLfloat  diffuse[4] = { .8, .5, .3, 1. };
   GLfloat specular[4] = { .3, .3, .3, 1. };
   GLfloat  ambient[4] = { .2, .2, .5, 1. };

   glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse  );
   glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specular );
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient  );
   glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, 16.      );

   if( mode == renderError )
   {
      glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, zero );
   }

   if( mode == renderRho || mode == renderError )
   {
      glEnable( GL_COLOR_MATERIAL );

      rhoMax = 0.;
      for( size_t i = 0; i < mesh.faces.size(); i++ )
      {
         rhoMax = max( rhoMax, abs( mesh.rho[i] ));
      }
   }
   else
   {
      glDisable( GL_COLOR_MATERIAL );
   }
}

void Viewer :: drawSurface( void )
{
   glPushAttrib( GL_ALL_ATTRIB_BITS );
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_LIGHTING );

   setMeshMaterial();

   if( mode == renderTextured )
   {
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, texture );
   }

   glCallList( surfaceDL );

   glPopAttrib();
}

void Viewer :: drawMesh( void )
{
   glEnable( GL_POLYGON_OFFSET_FILL );
   glPolygonOffset( 1., 1. );

   glBegin( GL_TRIANGLES );
   for( size_t i = 0; i < mesh.faces.size(); i++ )
   {
      if( mode == renderRho )
      {
         const double r = .85;
         const double g = .70;
         const double b = .61;
         double rho = mesh.rho[i] / rhoMax;

         if( rho < 0. )
         {
            // white->pink
            glColor3f( .2 + r * (  1.  ) * .6,
                       .2 + g * (1.+rho) * .6,
                       .2 + b * (  1.  ) * .6);
         }
         else
         {
            // white->green
            glColor3f( .2 + r * (1.-rho) * .6,
                       .2 + g * (  1.  ) * .6,
                       .2 + b * (1.-rho) * .6 );
         }
      }

      if( mode == renderError )
      {
         double error = quasiConformalDistortion( i );
         Vector c = qcColor( error );
         glColor3f( c.x, c.y, c.z );
      }

      if( mode == renderWireframe )
      {
         Vector N = faceNormal( i );
         glNormal3dv( &N[0] );
      }

      for( int j = 0; j < 3; j++ )
      {
         int k = mesh.faces[i].vertex[j];
         const Vector& N = vertexNormals[k];
         Vector uv = mesh.faces[i].uv[j] / uvScale;
         Vector p = vertex( i, j );

         glTexCoord2dv( &uv[0] );
         if( mode != renderWireframe )
         {
            glNormal3dv( &N[0] );
         }
         glVertex3dv( &p[0] );
      }
   }
   glEnd();

   glDisable( GL_POLYGON_OFFSET_FILL );

   if( mode == renderWireframe )
   {
      glDisable( GL_LIGHTING );
      glColor4f( 0., 0., 0., .25 );
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

      glBegin( GL_TRIANGLES );
      for( size_t i = 0; i < mesh.faces.size(); i++ )
      {
         for( int j = 0; j < 3; j++ )
         {
            Vector p = vertex( i, j );
            glVertex3dv( &p[0] );
         }
      }
      glEnd();
   }
}

Vector Viewer :: vertex( int faceIndex, int whichVertex )
{
   Face& face( mesh.faces[ faceIndex ] );
   int i = face.vertex[ whichVertex ];
   return mesh.newVertices[ i ].im();
}

Vector Viewer :: faceNormal( int faceIndex )
{
   Vector p[3] =
   {
      vertex( faceIndex, 0 ),
      vertex( faceIndex, 1 ),
      vertex( faceIndex, 2 )
   };

   return ((p[1]-p[0]) ^ (p[2]-p[0])).unit();
}

Vector Viewer :: barycenter( int faceIndex )
{
   Vector p[3] =
   {
      vertex( faceIndex, 0 ),
      vertex( faceIndex, 1 ),
      vertex( faceIndex, 2 )
   };

   return ( p[0] + p[1] + p[2] ) / 3.;
}

void Viewer :: computeVertexNormals( void )
{
   vertexNormals.resize( mesh.vertices.size());

   for( size_t i = 0; i < vertexNormals.size(); i++ )
   {
      vertexNormals[i] = Vector( 0., 0., 0. );
   }

   for( size_t i = 0; i < mesh.faces.size(); i++ )
   {
      Vector N = faceNormal( i );

      const Face& face( mesh.faces[i] );

      for( int i = 0; i < 3; i++ )
      {
         vertexNormals[ face.vertex[i] ] += N;
      }
   }

   for( size_t i = 0; i < mesh.vertices.size(); i++ )
   {
      vertexNormals[i].normalize();
   }
}

void Viewer :: updateDisplayList( void )
{
   if( surfaceDL )
   {
      glDeleteLists( surfaceDL, 1 );
      surfaceDL = 0;
   }

   surfaceDL = glGenLists( 1 );

   setMeshMaterial();

   glNewList( surfaceDL, GL_COMPILE );
   drawMesh();
   glEndList();
}


// QUASICONFORMAL ERROR --------------------------------------------------------

double Viewer :: quasiConformalDistortion( int faceIndex )
{
   // get original vertex positions
   Vector p1 = mesh.vertices[ mesh.faces[faceIndex].vertex[0] ].im();
   Vector p2 = mesh.vertices[ mesh.faces[faceIndex].vertex[1] ].im();
   Vector p3 = mesh.vertices[ mesh.faces[faceIndex].vertex[2] ].im();

   // get deformed vertex positions
   Vector q1 = mesh.newVertices[ mesh.faces[faceIndex].vertex[0] ].im();
   Vector q2 = mesh.newVertices[ mesh.faces[faceIndex].vertex[1] ].im();
   Vector q3 = mesh.newVertices[ mesh.faces[faceIndex].vertex[2] ].im();
   
   // compute edge vectors
   Vector u1 = p2 - p1;
   Vector u2 = p3 - p1;

   Vector v1 = q2 - q1;
   Vector v2 = q3 - q1;

   // compute orthonormal bases
   Vector e1 = u1 / u1.norm();
   Vector e2 = ( u2 - (u2*e1)*e1 );
   e2 = e2 / e2.norm();

   Vector f1 = v1 / v1.norm();
   Vector f2 = ( v2 - (v2*f1)*f1 );
   f2 = f2 / f2.norm();

   // project onto bases
   p1 = Vector( 0., 0., 0. );
   p2 = Vector( u1*e1, u1*e2, 0. );
   p3 = Vector( u2*e1, u2*e2, 0. );

   q1 = Vector( 0., 0., 0. );
   q2 = Vector( v1*f1, v1*f2, 0. );
   q3 = Vector( v2*f1, v2*f2, 0. );

   double A = ((p2-p1)^(p3-p1)).norm();

   Vector Ss = (q1*(p2.y-p3.y)+q2*(p3.y-p1.y)+q3*(p1.y-p2.y))/(2.*A);
   Vector St = (q1*(p3.x-p2.x)+q2*(p1.x-p3.x)+q3*(p2.x-p1.x))/(2.*A);
   double a = Ss*Ss;
   double b = Ss*St;
   double c = St*St;
   double Gamma = sqrt(.5*((a+c)+sqrt(sqr(a-c)+4.*b*b)));
   double gamma = sqrt(.5*((a+c)-sqrt(sqr(a-c)+4.*b*b)));

   if( Gamma < gamma )
   {
      swap( Gamma, gamma );
   }

   return Gamma/gamma;
}

Vector Viewer :: HSV( double h, double s, double v )
{
   double r = 0., g = 0., b = 0.;

   if (s == 0)
   {
      r = v;
      g = v;
      b = v;
   }
   else
   {
      h = (h == 1 ? 0 : h) * 6;

      int i = (int)floor(h);

      double f = h - i;
      double p = v * (1 - s);
      double q = v * (1 - (s * f));
      double t = v * (1 - s * (1 - f));

      switch (i)
      {
         case 0:
            r = v;
            g = t;
            b = p;
            break;

         case 1:
            r = q;
            g = v;
            b = p;
            break;

         case 2:
            r = p;
            g = v;
            b = t;
            break;

         case 3:
            r = p;
            g = q;
            b = v;
            break;

         case 4:
            r = t;
            g = p;
            b = v;
            break;

         case 5:
            r = v;
            g = p;
            b = q;
            break;

         default:
            break;
      }
   }

   return Vector(r, g, b);
}

Vector Viewer :: qcColor( double qc )
// standard color map for quasi-conformal distortion
{
   // clamp to range [1,1.5]
   qc = max( 1., min( 1.5, qc ));

   // compute color
   return HSV( (2.0-4.0*(qc-1.0))/3.0, .7, 0.65 );
}

void Viewer :: printQCDistortion( void )
// displays the max distortion and (area-weighted) mean distortion
{
   double totalArea = 0.;
   double avgQC = 0.;
   double maxQC = 0.;

   for( size_t i = 0; i < mesh.faces.size(); i++ )
   {
      double qc = quasiConformalDistortion( i );
      double A = mesh.area( i );

      totalArea += A;
      avgQC += A*qc;
      maxQC = max( qc, maxQC );
   }

   avgQC /= totalArea;

   cout << " max QC distortion: " << maxQC << endl;
   cout << "mean QC distortion: " << avgQC << endl;
   cout << endl;
}

// IMAGE MAP -------------------------------------------------------------------

void Viewer :: reloadImage( void )
{
   image.reload();
   mesh.setCurvatureChange( image, 5. );
}

// CAMERA CONTROL --------------------------------------------------------------

Quaternion Viewer :: clickToSphere( int x, int y )
{
   int viewport[4];
   glGetIntegerv( GL_VIEWPORT, viewport );
   int w = viewport[2];
   int h = viewport[3];

   Quaternion p( 0.,
                 2. * (double) x / (double) w - 1.,
                 2. * (double) y / (double) h - 1.,
                 0. );

   if( p.norm2() > 1. )
   {
      p.normalize();
      p.im().z = 0.;
   }
   else
   {
      p.im().z = sqrt( 1. - p.norm2() );
   }

   return p;
}

void Viewer :: mouse( int button, int state, int x, int y )
{
   if( state == GLUT_DOWN )
   {
      pClick = pDrag = pLast = clickToSphere( x, y );
      momentum = 1.;
   }
   if( state == GLUT_UP )
   {
      double timeSinceDrag = ( clock() - tLast ) / (double) CLOCKS_PER_SEC;

      if( timeSinceDrag < .1 )
      {
         momentum = pDrag * ( ~pLast );
         momentum = ( .03 * momentum + .97 ).unit();
      }
      else
      {
         momentum = 1.;
      }

      rLast = pDrag * ( ~pClick ) * rLast;
      pClick = pDrag = 1.;
   }
}

void Viewer :: motion( int x, int y )
{
   tLast = clock();
   pLast = pDrag;
   pDrag = clickToSphere( x, y );
}

void Viewer :: idle( void )
{
   // get time since last idle event
   static int t0 = clock();
   int t1 = clock();
   double dt = .5 * (t1-t0) / (double) CLOCKS_PER_SEC;

   rLast = momentum * rLast;
   momentum = ( (1.-dt) * momentum + dt ).unit();

   t0 = t1;

   glutPostRedisplay();
}

