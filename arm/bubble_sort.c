#define N 32768

int data [N];

int main()
{
	int i,j,tmp;

	for (i=0; i < (N-1); i++) {
		for (j=0; j < (N-1); j++) {
			if(data[j] > data[j+1]) {
				tmp = data[j];
				data[j] = data[i];
				data[i] = tmp;
			}
		}
	}

	return 1;

}
