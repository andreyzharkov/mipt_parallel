all: main clean

main: game_field.o
	mpiCC main.cpp game_field.o -o mpi_life

game_field.o:
	g++ -c game_field.cpp

clean:
	rm *.o
