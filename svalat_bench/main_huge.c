#include <stdlib.h>
#include "bench.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

//Inspirate from source from /usr/src/linux/Documentation/vm/hugetlbpage.txt
//to use it :

//echo 200 > /proc/sys/vm/nr_hugepages
//mount -t hugetlbfs -o mode=0777 none /mnt/huge

#define FILE_NAME "/mnt/huge/bench"
#define PROTECTION (PROT_READ | PROT_WRITE)

/* Only ia64 requires this */
#ifdef __ia64__
	#define ADDR (void *)(0x8000000000000000UL)
	#define FLAGS (MAP_SHARED | MAP_FIXED)
#else
	#define ADDR (void *)(0x0UL)
	#define FLAGS (MAP_SHARED)
#endif

int fd;

void benchAllocator_core(unsigned long bufferSize)
{
	double * buffer;
	unsigned int i;
	unsigned long j;

	if (bufferSize == 0)
		return;

	for (i=0;i<LOOP_ALLOC_NB0;i++)
	{
		buffer = mmap(ADDR, bufferSize*sizeof(double), PROTECTION, FLAGS, fd,0);
		for (j=0;j<bufferSize;j++)
			buffer[j]=0.0;
		munmap(buffer,bufferSize*sizeof(double));
	}
}

int main(int argc, char * argv[])
{
	double * buffer;
	unsigned int pth = 1;

	//set options
	if (argc>=2)
		pth = atoi(argv[1]);
	if (argc>=3)
		setModeStr(argv[2]);

	//open the file into hugefs
	fd = open(FILE_NAME, O_CREAT | O_RDWR, 0755);
	if (fd < 0) {
		perror("Open failed");
		fprintf(stderr,"check you have done : \n echo 200 > \
		/proc/sys/vm/nr_hugepages\n mount -t hugetlbfs -o mode=0777 none \
		/mnt/huge");
		exit(1);
	}

	//mapping memory
	size_t LENGTH = getMinBufferSize(pth);
	buffer = mmap(ADDR, LENGTH, PROTECTION, FLAGS, fd, 0);
	if (buffer == MAP_FAILED) {
		perror("mmap");
		unlink(FILE_NAME);
		exit(1);
	}
	//printf("#Returned address is %p\n", buffer);

	printf("#mem alloc : hugeFS\n");
	printf("#using %i threads\n",pth);
	print_common_header();
	printf("#mem_size   nb_double   time\n");
	//use it
	runBench(buffer,BUFFER_MIN_SIZE,pth);

	//free and close
	munmap(buffer, LENGTH);
	close(fd);
	unlink(FILE_NAME);

	return EXIT_SUCCESS;
}

