#ifndef __HP_ALLOCATOR_H__
#define __HP_ALLOCATOR_H__
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
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


void *hp_malloc(size_t taille);

#endif
