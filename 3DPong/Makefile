OPTS= -I"irrKlang/include" `sdl-config --libs` -lGL -lGLU -L"irrKlang/lib" ./irrKlang/linux-gcc-64/libIrrKlang.so

all:
	g++ 3D_PONG1Player.cpp -o 3D_PONG1Player $(OPTS)
	g++ 3D_PONG2Players.cpp -o 3D_PONG2Players $(OPTS)

clean:
	rm 3D_PONG1Player 3D_PONG2Players
