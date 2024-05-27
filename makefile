all: main

drawingObjects.o: drawingObjects.cpp drawingObjects.h
	g++ -c drawingObjects.cpp

main.o: main.cpp drawingObjects.h
	g++ -c main.cpp

main: main.o drawingObjects.o
	g++ -o main main.o drawingObjects.o -lSDL2 -lglu32 -lopengl32

clean:
	rm -fv *.o
	rm -fv main.exe

