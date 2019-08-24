##########################################################################################

# Specify library locations here (add or remove "#" marks to comment/uncomment lines for your platform)

# Mac OS X
# DDG_INCLUDE_PATH      =
# DDG_LIBRARY_PATH      =
# DDG_BLAS_LIBS         = -framework Accelerate
# DDG_SUITESPARSE_LIBS  = -lspqr -lumfpack -lcholmod -lmetis -lcolamd -lccolamd -lcamd -lamd -ltbb -lm -lsuitesparseconfig
# DDG_OPENGL_LIBS       = -framework OpenGL -framework GLUT

# # Linux 
# modifying includes to /usr/local/include .. suitesparse, etc.
DDG_INCLUDE_PATH      = 
DDG_LIBRARY_PATH      = 
DDG_BLAS_LIBS         = -llapack -lblas -lgfortran 
DDG_SUITESPARSE_LIBS  = -lspqr -lcholmod -lcolamd -lccolamd -lcamd -lamd -lm -lumfpack -lamd #-lmetis 
DDG_OPENGL_LIBS       = -lGL -lGLU -lglut -lGLEW -lX11

# # Windows / Cygwin
# DDG_INCLUDE_PATH      = -I/usr/include/opengl -I/usr/include/suitesparse
# DDG_LIBRARY_PATH      = -L/usr/lib/w32api -L/usr/lib/suitesparse
# DDG_BLAS_LIBS         = -llapack -lblas
# DDG_SUITESPARSE_LIBS  = -lspqr -lcholmod -lcolamd -lccolamd -lcamd -lamd -lm
# DDG_OPENGL_LIBS       = -lglut32 -lglu32 -lopengl32

########################################################################################



TARGET = spinxform
OBJS = CMWrapper.o EigenSolver.o Image.o LinearSolver.o Mesh.o Quaternion.o QuaternionMatrix.o Vector.o Viewer.o main.o

# UNAME = $(shell uname)
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
   $(info ************  Darwin ************)
   # Mac OS X
   CFLAGS  = -Wall -Werror -pedantic -ansi -O3 -Iinclude
   LDFLAGS = -Wall -Werror -pedantic -ansi -O3
   LIBS = -framework GLUT -framework OpenGL -framework Accelerate
else
   ifeq ($(UNAME),Linux)
      $(info ************  Linux ************)
      # Linux
      CFLAGS = -O3 -Wall -Werror -ansi -pedantic  $(DDG_INCLUDE_PATH) -I./include -I./src
      LFLAGS = -O3 -Wall -Werror -ansi -pedantic $(DDG_LIBRARY_PATH)
      # CFLAGS = -O3 -Wall -Werror -ansi -pedantic  -I./include -I./src
      # LFLAGS = -O3 -Wall -Werror -ansi -pedantic 
      LIBS = $(DDG_OPENGL_LIBS) $(DDG_SUITESPARSE_LIBS) $(DDG_BLAS_LIBS)
   else
      # Windows / Cygwin
      $(info ************  windows ************)
      CFLAGS  = -Wall -Werror -pedantic -ansi -O3 -Iinclude -I/usr/include/opengl
      LDFLAGS = -Wall -Werror -pedantic -ansi -O3 -L/usr/lib/w32api
      LIBS = -lglut32 -lglu32 -lopengl32
   endif
endif
CHOLMOD_LIBS = -lm -lamd -lcamd -lcolamd -lccolamd -lcholmod -lspqr # -lmetis


all: $(TARGET)

test:
	@echo "testing example 'bumpy'...";
	./spinxform examples/bumpy/sphere.obj examples/bumpy/bumpy.tga examples/bumpy/solution.obj
	@echo ""
	-diff examples/bumpy/solution.obj examples/bumpy/reference_solution.obj
	@echo ""
	@echo "testing example 'spacemonkey'...";
	./spinxform examples/spacemonkey/capsule.obj examples/spacemonkey/spacemonkey.tga examples/spacemonkey/solution.obj
	@echo ""
	-diff examples/spacemonkey/solution.obj examples/spacemonkey/reference_solution.obj

$(TARGET): $(OBJS)
	g++ $(OBJS) $(LDFLAGS) $(LIBS) $(CHOLMOD_LIBS) -o $(TARGET)

CMWrapper.o: src/CMWrapper.cpp include/CMWrapper.h
	g++ $(CFLAGS) -c src/CMWrapper.cpp
        
EigenSolver.o: src/EigenSolver.cpp include/EigenSolver.h include/QuaternionMatrix.h include/Quaternion.h include/Vector.h include/LinearSolver.h
	g++ $(CFLAGS) -c src/EigenSolver.cpp
        
Image.o: src/Image.cpp include/Image.h
	g++ $(CFLAGS) -c src/Image.cpp
        
LinearSolver.o: src/LinearSolver.cpp include/LinearSolver.h include/QuaternionMatrix.h include/Quaternion.h include/Vector.h
	g++ $(CFLAGS) -c src/LinearSolver.cpp
        
Mesh.o: src/Mesh.cpp include/Mesh.h include/Quaternion.h include/Vector.h include/QuaternionMatrix.h include/Image.h include/LinearSolver.h include/EigenSolver.h include/Utility.h
	g++ $(CFLAGS) -c src/Mesh.cpp
        
Quaternion.o: src/Quaternion.cpp include/Quaternion.h include/Vector.h
	g++ $(CFLAGS) -c src/Quaternion.cpp
        
QuaternionMatrix.o: src/QuaternionMatrix.cpp include/QuaternionMatrix.h include/Quaternion.h include/Vector.h
	g++ $(CFLAGS) -c src/QuaternionMatrix.cpp
        
Vector.o: src/Vector.cpp include/Vector.h
	g++ $(CFLAGS) -c src/Vector.cpp
        
Viewer.o: src/Viewer.cpp include/Utility.h include/Viewer.h include/Mesh.h include/Quaternion.h include/Vector.h include/QuaternionMatrix.h include/Image.h
	g++ $(CFLAGS) -c src/Viewer.cpp
        
main.o: src/main.cpp include/Viewer.h include/Mesh.h include/Quaternion.h include/Vector.h include/QuaternionMatrix.h include/Image.h
	g++ $(CFLAGS) -c src/main.cpp
	

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f examples/bumpy/solution.obj
	rm -f examples/spacemonkey/solution.obj

