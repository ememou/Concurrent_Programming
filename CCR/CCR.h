#define CCR_DECLARE(label) \
	pthread_mutex_t label ## mutex;\
	pthread_cond_t label ## wait;\
	int label ## cars;\
	int label ## waiting;\
	int label ## active;

#define CCR_INIT(label) \
	pthread_mutex_init(&label ## mutex, NULL);\
	pthread_cond_init(&label ## wait, NULL);\
	label ## cars = 0;\
	label ## waiting = 0;

#define CCR_EXEC(label, cond, body) \
	pthread_mutex_lock(&label ## mutex);\
	label ## cars++;\
	label ## active = 1;\
	while(!(cond)) {\
		if (!label ## active){\
			label ## waiting++;\
		}\
		label ## active = 0;\
		pthread_cond_wait(&label ## wait, &label ## mutex);\
	}\
	body\
	label ## cars += label ## waiting;\
	label ## waiting = 0; \
	while(label ## cars>0) {\
		label ## cars--;\
		pthread_cond_signal(&label ## wait);\
	}\
	pthread_mutex_unlock(&label ## mutex);
