#include <stdlib.h>
#include <stdio.h>
#include "bench.h"

#include "hp_allocator.h"


void benchAllocator_core(unsigned long bufferSize)
{
	double * buffer = NULL;
	unsigned int i;
	unsigned long j;

	if (bufferSize == 0)
		return;

	for (i=0;i<LOOP_ALLOC_NB0;i++)
	{
		buffer = (double*)malloc(bufferSize*sizeof(double));
		for (j=0;j<bufferSize;j++)
			buffer[j]=0.0;
		free(buffer);
	}
}

int main(int argc, char * argv[])
{
	double * buffer = NULL;
	unsigned int pth = 1;
	if (argc>=2)
		pth = atoi(argv[1]);
	if (argc>=3)
		setModeStr(argv[2]);

	printf("#mem alloc : malloc\n");
	printf("#using %i threads\n",pth);
	print_common_header();
	printf("#mem_size   nb_double   time\n");
	//buffer = (double*)malloc(getMinBufferSize(pth));
	buffer = hp_malloc(getMinBufferSize(pth));
	runBench(buffer,getMinBufferSize(pth),pth);
	//free(buffer);
	hp_free(buffer);

	return EXIT_SUCCESS;
}

