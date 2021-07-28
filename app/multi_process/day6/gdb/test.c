#include <stdio.h>
int main(void)
{

	int arr[5];
	for (int i=0; i < 5; i++) {
		printf("arr[%d]:%d,",i,arr[i]);
	}
	printf("\n");
	printf("arr size:%d,len=%d\n",sizeof(arr),sizeof(arr)/sizeof(int));
	memset(arr,0x0, sizeof(arr));
	for (int i=0; i < 5; i++) {
		printf("arr[%d]:%d,",i,arr[i]);
	}

	printf("\n");
	return 0;
}
