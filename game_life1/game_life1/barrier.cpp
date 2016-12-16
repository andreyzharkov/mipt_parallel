#include "barrier.h"
#include "cassert"

Barrier::Barrier(unsigned num_threads, void (*f)(void))
	:capacity(num_threads), entered(0), round(0){
	func = f;
	pthread_mutex_init(&mtx, NULL);
	pthread_cond_init(&wait_others, NULL);
	assert(num_threads > 1);
}

void Barrier::enter() {
	//std::unique_lock<std::mutex> lock(mtx);
	pthread_mutex_lock(&mtx);
	++entered;
	if (entered == capacity) {
		func();
		entered = 0;
		++round;
		//wait_others.notify_all();
		pthread_cond_broadcast(&wait_others);
	}
	else {
		unsigned my_round = round;
		while (my_round == round)
			pthread_cond_wait(&wait_others, &mtx);
			//wait_others.wait(lock);
	}
	pthread_mutex_unlock(&mtx);
}
