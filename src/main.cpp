// =============================================================================
// SpinXForm -- main.cpp
// Keenan Crane
// August 16, 2011
//

#include <iostream>
#include "Viewer.h"

using namespace std;

int main( int argc, char **argv )
{
   if( argc < 3 || argc > 4 )
   {
      cerr << "usage: " << argv[0] << " mesh.obj image.tga [result.obj]" << endl;
      return 1;
   }

   if( argc == 4 ) // batch mode
   {
      // load mesh
      Mesh mesh;
      mesh.read( argv[1] );

      // load image
      Image image;
      image.read( argv[2] );

      // apply transformation
      const double scale = 5.;
      mesh.setCurvatureChange( image, scale );
      mesh.updateDeformation();

      // write result
      mesh.write( argv[3] );
   }
   else // interactive mode
   {
      Viewer viewer;

      // load mesh
      viewer.mesh.read( argv[1] );

      // load image
      viewer.image.read( argv[2] );

      // start viewer
      viewer.init();
   }

   return 0;
}

