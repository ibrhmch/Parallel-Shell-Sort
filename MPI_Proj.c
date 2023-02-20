#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <mpi.h> 
using namespace std;
const int MAX_STRING = 32;

int* init(int arr[])
{
	for (int i = 0; i < MAX_STRING; i++)
	{
		arr[i] = rand()%100;
	}
	return arr;
}


void shellSort(int arr[], int n)
{
	for (int gap = n / 2; gap > 0; gap /= 2)
	{
		for (int i = gap; i < n; i += 1)
		{
			int temp = arr[i];
			int j;
			for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
				arr[j] = arr[j - gap];
			arr[j] = temp;
		}
	}
}

int* merg(int arr1[], int arr2[], int n)
{
	int* arr3;
	arr3 = new int[n * 2];
	int i, j, k;
	i = j = k = 0;
	while (i < n && j < n)
	{
		if (arr1[i] < arr2[j])
			arr3[k++] = arr1[i++];
		else
			arr3[k++] = arr2[j++];
	}
	while (i < n)
		arr3[k++] = arr1[i++];
	while (j < n)
		arr3[k++] = arr2[j++];
	/*for (int i = 0; i < 2*n; i++)
	{
		printf("%d  ", arr3[i]);
	}
	printf("\n");*/
	return arr3;
}

void display(int arr[],int n,int rank)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d  ", arr[i]);
	}
	printf("\nMy rank is %d\n",rank);

}

int main(void) {
	int* arr, * local_arr, * local_a;
	arr = new int[MAX_STRING];
	int comm_sz;
	int my_rank;
	int local_sum=0;
	int sum=0;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if (my_rank == 0) {
		arr = init(arr);
		printf("Initial array\n");
		display(arr, MAX_STRING,my_rank);
	}
	local_arr = new int[MAX_STRING / comm_sz];
	MPI_Scatter(arr, MAX_STRING/comm_sz, MPI_INT, local_arr, MAX_STRING / comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	shellSort(local_arr, MAX_STRING / comm_sz);
	//display(local_arr, MAX_STRING / comm_sz,my_rank);
	//printf("After shell sort\n");
	int c = 1;
	local_a = new int;
	for (int j= comm_sz/2; j > 0; j /= 2) {
		for (int i = (j*2)-1; i >= 0; i--)
		{
			if (i > j-1)
			{
				if (my_rank == i) {
					MPI_Send(local_arr, (MAX_STRING / comm_sz)*c
						, MPI_INT, i - j, 0, MPI_COMM_WORLD);
				}
			}
			else
			{
				if (my_rank == i) {
					local_a = new int[(MAX_STRING / comm_sz) * c];
					MPI_Recv(local_a, (MAX_STRING / comm_sz) * c, MPI_INT, i + j, 0,
						MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
			}
		}
		for (int i = 0; i< j; i++) {
			if (my_rank==i)
			{
				local_arr = merg(local_a, local_arr, (MAX_STRING / comm_sz) * c);
				//display(local_arr, (MAX_STRING / comm_sz) * c, my_rank);
			}
		}
		c*=2;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (my_rank==0)
	{
		printf("After sorting\n");
		display(local_arr, MAX_STRING,my_rank);
	}
	MPI_Finalize();
	return 0;
}