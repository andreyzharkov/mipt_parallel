all: main clean
	
main: game_field.o barrier.o fast_parallel.o simple_parallel.o
	g++ life.cpp barrier.o fast_parallel.o simple_parallel.o game_field.o omp_parallel.h -o life -pthread -fopenmp

barrier.o: barrier.cpp
	g++ -c barrier.cpp -pthread

game_field.o: game_field.cpp
	g++ -c game_field.cpp
	
simple_parallel.o: simple_parallel.cpp
	g++ -c simple_parallel.cpp -pthread

fast_parallel.o: fast_parallel.cpp game_field.o
	g++ -c fast_parallel.cpp -pthread

clean:
	rm -rf *.o
