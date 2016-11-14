#pragma once

#include <pthread.h>

//Реализуем readers-writers mutex
class rw_mutex {
public:
	//Изначально читателей 0, писателей тоже
	rw_mutex() {
		//pthread_rwlockattr_t* attr;
		//pthread_rwlockattr_init(attr);
		//pthread_rwlockattr_setpshared(attr, 1);
		pthread_rwlock_init(&lock_, NULL);
	};
	
	~rw_mutex(){
		pthread_rwlock_destroy(&lock_);
	}


	//Читатели ожидают, пока текущий писатель не завершит работу
	//После этого зайти в критическую секцию могут неограниченно много читателей
	void read_lock() {
		pthread_rwlock_rdlock(&lock_);
	}
	
	//Если все читатели вышли из критической секции, в нее попадает один писатель
	void read_unlock(){
		pthread_rwlock_unlock(&lock_);
	}

	//Писатель ждет, пока из критической секции не выйдут все остальные потоки
	void lock(){
		pthread_rwlock_wrlock(&lock_);
	}

	void unlock(){
		pthread_rwlock_unlock(&lock_);
	}


private:
	pthread_rwlock_t lock_;
};
