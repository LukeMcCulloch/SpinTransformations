TARGET = spinxform
OBJS = CMWrapper.o EigenSolver.o Image.o LinearSolver.o Mesh.o Quaternion.o QuaternionMatrix.o Vector.o Viewer.o main.o

UNAME = $(shell uname)

ifeq ($(UNAME), Darwin)
   # Mac OS X
   CFLAGS  = -Wall -Werror -pedantic -ansi -O3 -Iinclude
   LDFLAGS = -Wall -Werror -pedantic -ansi -O3
   LIBS = -framework GLUT -framework OpenGL -framework Accelerate
else
   # Windows / Cygwin
   CFLAGS  = -Wall -Werror -pedantic -ansi -O3 -Iinclude -I/usr/include/opengl
   LDFLAGS = -Wall -Werror -pedantic -ansi -O3 -L/usr/lib/w32api
   LIBS = -lglut32 -lglu32 -lopengl32
endif

CHOLMOD_LIBS = -lm -lamd -lcamd -lcolamd -lccolamd -lcholmod -lspqr -lmetis


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

