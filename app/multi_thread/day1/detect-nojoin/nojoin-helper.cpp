#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>
#include <map>
#include <iostream>

static pthread_mutex_t mtx;
typedef std::pair < void *, void *>elem_t;
typedef std::map < pthread_t, elem_t > map_t;
static map_t thr_map;

extern "C" int pthread_create(pthread_t * tid, const pthread_attr_t * attr,
		       void *(*start_routine) (void *), void *arg)
{
	static __decltype(pthread_create) * real
	    = reinterpret_cast < __decltype(pthread_create) * >(dlsym(RTLD_NEXT,
								      "pthread_create"));
	int rc = (*real) (tid, attr, start_routine, arg);
	if (rc == 0) {
		pthread_mutex_lock(&mtx);
		thr_map[*tid] = std::make_pair((void *)start_routine, arg);
		printf("create\n");
		pthread_mutex_unlock(&mtx);
	}
	return rc;
}

extern "C" int pthread_join(pthread_t tid, void **arg)
{
	static __decltype(pthread_join) * real
	    = reinterpret_cast < __decltype(pthread_join) * >(dlsym(RTLD_NEXT,
								    "pthread_join"));
	int rc = (*real) (tid, arg);
	if (rc == 0) {
		pthread_mutex_lock(&mtx);
		const auto it = thr_map.find(tid);
		assert(it != thr_map.end());
		thr_map.erase(it);
		pthread_mutex_unlock(&mtx);
	}
	return rc;
}

extern "C" void print_nojoin()
{
	pthread_mutex_lock(&mtx);

        std::cout << "found " << thr_map.size() << " not joined threads" << std::endl;

	for (auto it = thr_map.begin(); it != thr_map.end(); it++)
		std::cout << "tid:" << it->first << " fnc:" << it->second.
		    first << " arg:" << it->second.second << std::endl;

	pthread_mutex_unlock(&mtx);
}
