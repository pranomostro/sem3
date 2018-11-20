/* This makes pthread_yield available and indicates that this
* application needs a GNU build/runtime environment. See
* man feature_test_macros for details */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>

/* Important: Use correct types in function declaration */
void *thread_func(void *arg)
{
	printf("Hello my name is %s\n", (char*) arg);
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	size_t i;
	int ret;
	const char *names[5] = {"up", "you", "give", "gonna", "never"};
	pthread_t tid[sizeof(names) / sizeof(*names)];

	/* Run backwards through array and dispatch a thread
	 * for each name */
	for (i = sizeof(names) / sizeof(*names); i > 0; i--) {
	ret = pthread_create(&tid[i-1], NULL, thread_func, (void*) names[i-1]);
	if(ret!=0) {
			fprintf(stderr, "Fatal: Could not start thread %zu.\n",
					i - 1);
			exit(-1);
	}
	}

	/* It's okay, allow the workers to do their work,
	 * relinquishing the CPU */

	sched_yield();

	/* Join all threads */
	for (i = 0; i < sizeof(names) / sizeof(*names); i++) {
		ret = pthread_join(tid[i], NULL);
		/* Don't forget to check for errors */
		if (ret) {
			fprintf(stderr, "Fatal: Failed to join thread %zu.\n", i);
			exit(-1);
		}
	}

	return 0;
}
