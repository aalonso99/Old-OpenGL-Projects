OPTS= -O2 -fopenmp -I"irrKlang/include" `sdl-config --libs` -lGL -lGLU -lIL -lILU -L"irrKlang/lib" ./irrKlang/linux-gcc-64/libIrrKlang.so

all:
	g++ LTexture.cpp geometry.cpp objects.cpp robot.cpp vrg.cpp -o vrg $(OPTS)

clean:
	rm vrg
