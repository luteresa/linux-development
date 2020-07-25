#include <iostream>
#include <queue>
#include <string>
#include <unistd.h>

using namespace std;

char *path_detect1= NULL;
char *path_detect0= NULL;

char *path_extraf1= NULL;
char *path_extraf0= NULL;

char *path_model= NULL;
struct data{
	string name;
	int score;
};
queue<struct data> myqueue;

void *extractFeature_thread(void *arg)
{
	char * path_testfile = (char *)arg;

	int c=50;
	while(c-- > 0) {
		cout << "===read data: name" << myqueue.front().name<<",score"<<myqueue.front().score<<endl;;
		myqueue.pop();
		usleep(200*1000);
	}

	return NULL;
}
void *detect_thread(void *arg)
{
	char * path_testfile = (char *)arg;
	int c=50;
	struct data d;

	while(c-- > 0) {
		printf("~~~write data:%d\n",c);
		
		d.name = "person"+to_string(c);
		d.score = c+50;
		myqueue.push(d);
		usleep(100*1000);
	}

	return NULL;
}


int main(int argc, char **argv)
{
	int ret=0;
	
	if(argc < 2){
		printf("input error.\n");
		printf("usage: ./a.out testfile0 testfile1\n\n");
		exit(1);
	}

	if (argc>=2) {
		path_detect0 = argv[1];
		path_extraf0 = argv[2];
	}


	pthread_t tidA, tidB; 
	pthread_t tidA1, tidB1; 
	pthread_t tidC, tidD; 
	pthread_t tidC1, tidD1; 

	pthread_create(&tidA, NULL, &detect_thread, path_detect0); 
	pthread_create(&tidC, NULL, &extractFeature_thread, path_extraf0); 
	
	pthread_join(tidA, NULL); 
	pthread_join(tidC, NULL); 


	return 0;
}

