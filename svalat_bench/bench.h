#ifndef INCLUDED_BENCH_H
#define INCLUDED_BENCH_H

#include <stddef.h>

#define STEP_SIZE 16
#ifndef LOOP_NB0
#define LOOP_NB0 20000000UL*STEP_SIZE
#endif
#define LOOP_ALLOC_NB0 50
//MM = Memory Mode
//on utilise le même buffer pour tous les threads
#define MM_SAME_BUFFER 0x0
//on utilise le même buffer mais on décale d'une moitié les différents threads
//WO = With Offset
//attention vous devez allouer TAILLE*(1+nb_pth/2)
#define MM_SAME_BUFFER_WO 0x1
//on utilise des buffers différents pour chaque threads
//attantion vous devez allouer TAILLE*nb_pth
#define MM_NOT_SAME_BUFFER 0x2
#define KILO_OCTET(x) (x*1024/sizeof(double))


#define BUFFER_MIN_SIZE 14UL*1024*1024

void bench(unsigned int size,double * buffer,unsigned int nbThread);
void runBench(double * buffer,unsigned long bufferSize,unsigned int nbThread);
void setMode(unsigned int mode);
int getMode(void);
const char * getModeStr(void);
void setModeStr(const char * mode);
size_t getMinBufferSize(unsigned int nbPthread);
void print_common_header();
double benchAllocator(unsigned long bufferSize);
void benchAllocator_core(unsigned long bufferSize);

#endif
