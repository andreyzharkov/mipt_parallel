#pragma once

#include <pthread.h>
#include <iostream>

class Barrier {
public:
	Barrier(unsigned num_threads, void(*f)(void));
	//thread will wait until count of waiting threads reach surtain number
	void enter();
private:
	pthread_mutex_t mtx;
	pthread_cond_t wait_others;
	//number of waiting threads
	int entered;
	//era
	size_t round;
	//number of threads required to break through
	const size_t capacity;
	void (*func)(void);
};
