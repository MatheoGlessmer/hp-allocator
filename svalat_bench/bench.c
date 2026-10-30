#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "bench.h"

#define KILO_OCTET(x) (x*1024/sizeof(double))

double tmp;

/**
 * Structure utilisé pour passer les arguments aux méthodes d'émorce de thread
 * du benchmark.
**/
struct ThreadMainArgs
{
	/** Pointe vers le buffer à lire  pendant le benchmark. **/
	double * buffer;
	/** Définit la taille du buffer à lire. **/
	unsigned int size;
	/** utilisé par la méthode pour placer le résultat final du benchmark. **/
	double timeResult;
};

/**
 * Variable globale au fichier permettant de sélectionné le mode mémoire
 * utilisé. Por plus de détail voir MM_SAME_BUFFER, MM_SAME_BUFFER_WO et
 * MM_NOT_SAME_BUFFER.
**/
unsigned int _benchMode = MM_SAME_BUFFER;

/**
 * Calcule la taille de buffer nécessaire et la renvoie.
**/
size_t getMinBufferSize(unsigned int nbPthread)
{
	switch (_benchMode)
	{
		case MM_SAME_BUFFER:
			return BUFFER_MIN_SIZE;
		case MM_SAME_BUFFER_WO:
			return BUFFER_MIN_SIZE+(BUFFER_MIN_SIZE*nbPthread)/2;
		case MM_NOT_SAME_BUFFER:
			return BUFFER_MIN_SIZE*nbPthread;
		default:
			fprintf(stderr,"_benchMode as an unknown value : %i\n",_benchMode);
			exit(1);
	};
	return 0;
}

/**
 * Permet de mettre à jour la variable globale de sélection de mode mémoire. **/
void setMode(unsigned int mode)
{
	_benchMode = mode;
}

/**
 * Permet de mettre à jour la variable globale de sélection de mode mémoire
 * depuis un string C.
**/
void setModeStr(const char * mode)
{
	if (strcmp(mode,"SAME_BUFFER")==0) {
		_benchMode = MM_SAME_BUFFER;
	} else if (strcmp(mode,"SAME_BUFFER_WO")==0) {
		_benchMode = MM_SAME_BUFFER_WO;
	} else if (strcmp(mode,"NOT_SAME_BUFFER")==0) {
		_benchMode = MM_NOT_SAME_BUFFER;
	} else {
		fprintf(stderr,"Unknown mode %s\n",mode);
		exit(1);
	}
}

/**
 * @return Permet de récupérer le mode mémoire en cours d'utilisation. **/
int getMode(void)
{
	return _benchMode;
}

/**
 * @return Permet de récupérer le mode mémoire en cours d'utilisation
 * sous forme d'une chaine de caractère C.
**/
const char * getModeStr(void)
{
	switch (_benchMode)
	{
		case MM_SAME_BUFFER:
			return "SAME_BUFFER";
		case MM_SAME_BUFFER_WO:
			return "SAME_BUFFER_WO";
		case MM_NOT_SAME_BUFFER:
			return "NOT_SAME_BUFFER";
		default:
			return "UNKNOWN";
	}
}

/**
 * Coeur du benchmark, cette méthode effectue la boucle principale et chronomètre
 * son temps d'exécution. Le méthode maintient constant le nombre d'accès mémoire.
 * @param size Définit la taille à lire dans le buffer.
 * @param buffer Définit le buffer sur lequel appliquer la lecture séquentielle.
 * @return Renvoie le temps en seconde.
**/
double benchRead(unsigned int size,double * buffer)
{
	struct timeval t0,t1;
	unsigned long i,max_loop;
	unsigned int j;
	//calc max loop to do
	max_loop = LOOP_NB0;
	//on calcule de sorte à avancer en temps constant en parcourant
	//toujours le même nombre de case mémoire au total.
	//on compense donc en réduisant la taille de la boucle externe de répétition
	//de parcour, de sorte que nb_parcour * taille_parcour reste constant.
	if (size>1)
		max_loop = max_loop/size;

	//on fait 5 boucles de chauffe.
	for(i=0;i<5;i++)
		for (j=0;j<size;j++)
			tmp = buffer[j];

	//strart timer
	gettimeofday(&t0,NULL);

	for(i=0;i<max_loop;i++)
		for (j=0;j<size;j+=STEP_SIZE)
			tmp = buffer[j];

	//end timer
	gettimeofday(&t1,NULL);

	//calc time
	return (t1.tv_sec-t0.tv_sec)+((double)t1.tv_usec-t0.tv_usec)/1000000.0;
}

double benchAllocator(unsigned long bufferSize)
{
	struct timeval t0,t1;

	//strart timer
	gettimeofday(&t0,NULL);

	benchAllocator_core(bufferSize);

	//end timer
	gettimeofday(&t1,NULL);

	return (t1.tv_sec-t0.tv_sec)+((double)t1.tv_usec-t0.tv_usec)/1000000.0;
}

/**
 * Méthode d'amorce du thread de mesure de temps.
**/
void * threadBenchMain(void * args)
{
	struct ThreadMainArgs * arg = args;
	arg->timeResult = benchRead(arg->size,arg->buffer);
	return NULL;
}

/**
 * Lance le benchmark sous forme multithreadé.
 * @param size Définit la taille à lire sur le buffer
 * @param buffer Définit le buffer à parcourir.
 * @param nbThread Définit le nombre de thread à lancer.
**/
void threadedBench(unsigned int size,double * buffer,unsigned int nbThread)
{
	pthread_t threadInfo[32];
	struct ThreadMainArgs args[32];
	unsigned int i;
	unsigned int rsize;
	double a;

	//check limit
	if (nbThread>32)
	{
		fprintf(stderr,"Thread number is too big, limit is set to 32\n");
		exit(1);
	}

	//init thread info
	for (i=0;i<nbThread;i++)
	{
		args[i].timeResult=0.0;
		args[i].size = size;
		switch (_benchMode)
		{
			case MM_SAME_BUFFER:
				args[i].buffer = buffer;
				break;
			case MM_SAME_BUFFER_WO:
				args[i].buffer = buffer+(i*size/2);
				break;
			case MM_NOT_SAME_BUFFER:
				args[i].buffer = buffer+size*i;
				break;
			default:
				fprintf(stderr,"_benchMode as an unknown value : %i\n",
					_benchMode);
				exit(1);
		}

	}

	//start threads
	for (i=0;i<nbThread;i++)
		if (pthread_create(threadInfo+i, NULL,threadBenchMain, args+i))
			fprintf(stderr,"Fail to start thread %i\n",i);

	//wait all
	void * retval;
	for (i=0;i<nbThread;i++)
		pthread_join(threadInfo[i],&retval);

	//test allocator
	a = benchAllocator(size);

	//display result
	rsize = sizeof(double)*size;
	printf("%i ko\t%i",rsize/1024,size);
	for (i=0;i<nbThread;i++)
		printf("\t%.06f",args[i].timeResult);
	printf("\t%g\n",a);
}

/**
 * Méthode principale utilisée pour lancer le benchmark en mode monothread ou
 * multithread pour une taille donnée. La méthode prend en charge l'affichage
 * du résultat.
 * @param size Définit la taille à lire sur le buffer
 * @param buffer Définit le buffer à parcourir.
 * @param nbThread Définit le nombre de thread à lancer.
**/
void bench(unsigned int size,double * buffer,unsigned int nbThread)
{
	double t,a;
	unsigned int rsize;

	//check buffer
	if (buffer==NULL)
	{
		fprintf(stderr,"Can't run on NULL buffer\n");
		exit(1);
	}

	if (nbThread > 1)
	{
		threadedBench(size,buffer,nbThread);
	} else {
		t = benchRead(size,buffer);
		//a = benchAllocator(size);
		a=0;
		rsize = sizeof(double)*size;
		printf("%i ko\t%i\t%g\t%g\n",rsize/1024,size,t,a);
	}
}

/**
 * Méthode principale de lancement du benchmark.
 * @param size Définit la taille à lire sur le buffer
 * @param buffer Définit le buffer à parcourir.
 * @param nbThread Définit le nombre de thread à lancer.
**/
void runBench(double * buffer,unsigned long bufferSize,unsigned int nbThread)
{
	unsigned int i=0;

	if (bufferSize < KILO_OCTET(4096))
	{
		fprintf(stderr,"Error, buffer is too short\n");
		exit(1);
	}

	//init buffer with 0
	for (i=0;i<BUFFER_MIN_SIZE/sizeof(double);i++)
		buffer[i]=0.0;

	for (i=0;i<=KILO_OCTET(156);i+=128)
		bench(i,buffer,nbThread);
	fprintf(stderr,"____PASS 128K_____\n");
	for (;i<=KILO_OCTET(256);i+=256)
		bench(i,buffer,nbThread);
	fprintf(stderr,"____PASS 256K_____\n");
	for (;i<=KILO_OCTET(512);i+=512)
		bench(i,buffer,nbThread);
	fprintf(stderr,"____PASS 512K_____\n");
	for (;i<=KILO_OCTET(4096);i+=1024)
		bench(i,buffer,nbThread);
	fprintf(stderr,"____PASS 4096K_____\n");
	for (;i<=BUFFER_MIN_SIZE/sizeof(double);i+=4096)
			bench(i,buffer,nbThread);
	fprintf(stderr,"____PASS %ldK_____\n",BUFFER_MIN_SIZE/1024);
}

void print_common_header()
{
	printf("#using memory mode : %s\n",getModeStr());
	printf("#STEP_SIZE = %d\n",STEP_SIZE);
	printf("#LOOP_NB0 = %ld\n",LOOP_NB0);
	printf("#buffer init with 0.0 = true\n");
	system("printf '#arch : ';uname -m");
	system("printf '#CPU : ';uname -p");
	system("printf '#OS : ';uname -s");
	system("printf '#version : ';uname -r");
}
