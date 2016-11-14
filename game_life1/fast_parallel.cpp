#include <iostream>
#include <vector>
#include <cassert>
#include <stdlib.h>

#include "game_field.h"
#include "fast_parallel.h"
#include "shared_mutex.h"

int n_iter;

class History {
	static const int MAX_BUF_SIZE = 20;
	//static const int MAX_THREADS_NUM = 20;
public:
	History(){}
	History(game_field initial_field, int n_threads, int buf_size=MAX_BUF_SIZE)
	{
		fields.resize(buf_size, initial_field);
		threads_borders.resize(buf_size, std::vector<int>(n_threads, -1));
		threads_borders[0] = std::vector<int>(n_threads, 0);
		curr_first = 1;
		curr_first_iter = 1;
		std::vector<std::pair<int, int> > borders = split_field(initial_field, n_threads);
		
		for (int i = 0; i < n_threads; i++){
			left_borders.push_back((borders[i]).first);
			right_borders.push_back((borders[i]).second);
		}
	}
	int getIterIndex(int iter){
		//read lock guard
		//std::cout << iter << std::endl;
		//mutex_.read_lock();
		//int res = (curr_first + (iter - curr_first_iter - 1) + fields.size()) % fields.size();
		//mutex_.read_unlock();
		return (iter) % fields.size();
	}
	//returns result
	game_field getLastField(){
		//return fields[(curr_first - 1 + fields.size()) % fields.size()];
		return fields[n_iter % fields.size()];
	}
	inline bool isIterFinished(int thread_id, int iter){
		if (iter < curr_first_iter) return true;
		return threads_borders[getIterIndex(iter)][thread_id] == 0;
	}
	inline bool neighbours_ready(int thread_id, int iter){
		if (thread_id == 0)
			return isIterFinished(1, iter);
		if (thread_id == left_borders.size() - 1)
			return isIterFinished(thread_id - 1, iter);
		return isIterFinished(thread_id - 1, iter) && isIterFinished(thread_id + 1, iter);
	}
	bool construct_field(int thread_id, int iteration){
		if (neighbours_ready(thread_id, iteration - 1)){
			//std::cout << "neighbours ready" + thread_id << std::endl;	
			int iter_index = getIterIndex(iteration);
			int d = threads_borders[iter_index][thread_id];
			if (d == -1){
				next(fields[(iter_index - 1 + fields.size()) % fields.size()], fields[iter_index], left_borders[thread_id], right_borders[thread_id]);
			}
			else{
				next(fields[(iter_index - 1 + fields.size()) % fields.size()], fields[iter_index], 
					left_borders[thread_id], std::min(right_borders.back(), left_borders[thread_id] + d));
				next(fields[(iter_index - 1 + fields.size()) % fields.size()], fields[iter_index], 
					std::max(0, right_borders[thread_id] - d), right_borders[thread_id]);
			}
			threads_borders[iter_index][thread_id] = 0;
			
			//check if all threads finish that iteration - reset curr_first & curr_first_iter
			
			bool calculated_complitely = true;
			for (int i = 0; i < right_borders.size(); i++){
				if (threads_borders[iter_index][i] != 0){
					calculated_complitely = false;
					//std::cout << threads_borders[iter_index][i] << std::endl;
					break;
				}
			}
			if (calculated_complitely){
				int current_iteration = curr_first_iter;
				//std::cout << "before lock" << std::endl;
				//write lock
				mutex_.lock();
				
				if (iter_index == curr_first){
					//fields[curr_first].print();
					//std::cout << "before increase" << std::endl;
					//std::cout << curr_first_iter << std::endl;
					//std::cout << curr_first_iter << std::endl;
					for (int i = 0; i < threads_borders[curr_first].size(); i++){
						threads_borders[(curr_first - 1 + fields.size()) % fields.size()][i] = -1;
					}
					curr_first_iter++;
					curr_first = (curr_first + 1) % fields.size();
				}
				//unlock
				mutex_.unlock();
				//std::cout << "unlock done" << std::endl;
			}
			
			return true;
		}
		else {
			//read lock
			//mutex_.read_lock();

			int iter_index = getIterIndex(iteration);
			int curr_first_index = curr_first;
			
			//mutex_.read_unlock();
			//read unlock
			
			int d = 1;
			for (int i = iter_index; i != curr_first_index; i = (i + 1) % fields.size()){
				if (right_borders[i] - left_borders[i] <= 2*d){
					return false;
				}
				if ((threads_borders[i][thread_id] == -1)){
					next(fields[(i - 1 + fields.size()) % fields.size()], fields[i], 
						left_borders[thread_id] + d, right_borders[thread_id] - d);
					threads_borders[i][thread_id] = d;
					return false;
				}
				
				if ((threads_borders[i][thread_id] > d)){
					next(fields[(i - 1 + fields.size()) % fields.size()], fields[i], 
						left_borders[thread_id] + d, left_borders[thread_id] + threads_borders[i][thread_id]);
					next(fields[(i - 1 + fields.size()) % fields.size()], fields[i], 
						right_borders[thread_id] - threads_borders[i][thread_id], right_borders[thread_id] - d);
					threads_borders[i][thread_id] = d;
					return false;
				}
				d++;
			}
			return false;
		}
	}
private:
	//last fields was (or still is) calculated
	std::vector<game_field> fields;
	//how many columns (from one side) is not calculated yet in this field by this process
	std::vector<std::vector<int> > threads_borders;
	//left border of the process subfield (included)
	std::vector<int> left_borders;
	//right border of the process subfield (not included)
	std::vector<int> right_borders;
	
	rw_mutex mutex_;
	int curr_first;
	int curr_first_iter;
};

History* history;
/*
void thread_func(int thread_id){
	int curr_iter = 1;
	while (curr_iter <= n_iter + 1){
		if (history->construct_field(thread_id, curr_iter)){
			curr_iter++;
		}
	}
}*/

static void* thread_func(void* thread_id_p){
	int thread_id = *(int*)thread_id_p;
	//std::cout << thread_id << std::endl;
	int curr_iter = 1;
	while (curr_iter <= n_iter + 1){
		if (history->construct_field(thread_id, curr_iter)){
			curr_iter++;
		}
	}
}

game_field fast_parallel(game_field& initial_field, int n_iter_, int n_threads){
	n_iter = n_iter_;
	history = new History(initial_field, n_threads);
	/*
	std::vector<std::thread> threads;
	for (int i = 0; i < n_threads - 1; i++){
		threads.emplace_back(thread_func, i);
	}
	thread_func(n_threads - 1);
	for (auto& t: threads){
		t.join();
	}*/
	std::vector<pthread_t> threads(n_threads - 1);
	std::vector<int> ids(n_threads);
	
	for (int i = 0; i < n_threads - 1; i++){
		ids[i] = i;
		pthread_create(&threads[i], NULL, thread_func, &ids[i]);
	}
	ids[n_threads - 1] = n_threads - 1;
	//for (int i = 0; i < ids.size(); i++) std::cout << ids[i] << std::endl;
	thread_func(&ids[n_threads - 1]);
	int dum;
	for (int i = 0; i < n_threads - 1; i++){
		if(pthread_join(threads[i],(void**)&dum)) {
            std::cout << "ERROR: CAN'T JOIN THREAD" << std::endl;
            exit(1);
		}
	}
	game_field ans = history->getLastField();
	
	
	
	
	delete history;
	return ans;
}
