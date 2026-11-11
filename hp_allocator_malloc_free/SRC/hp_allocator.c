#include "hp_allocator.h"

#define MEM_SIZE (14UL*1024UL * 1024UL)
#define ALIGNEMENT(type) (offsetof(struct { char c; type t; }, t))
#define TAILLE_EN_TETE (offsetof(struct { struct bloc b; union align u; }, u))

int __hp_fd;

union align
{
    long e;
    long double f;
    void *p;
};

union reserve
{
    union align align;
    //char mem[MEM_SIZE];
    char* mem;
};

struct bloc
{
    size_t taille;
    struct bloc *suivant;
};

static union reserve reserve;
static struct bloc *libre;

static void bloc_init(struct bloc *, size_t);
static size_t calcule_multiple_align(size_t);
static struct bloc *recherche_bloc_libre(size_t);

static void *__hp_malloc(size_t);
static void __hp_free(void*);


static void bloc_init(struct bloc *b, size_t taille)
{
    /*
     *      * Initialise les membres d'une structure `bloc`.
     *           */

    b->taille = taille;
    b->suivant = NULL;
}


static size_t calcule_multiple_align(size_t a)
{
    /*
     *      * Calcule le plus petit multiple de l'alignement maximal égal ou supérieur à `a`.
     *           */

    static size_t align_max = ALIGNEMENT(union align);
    size_t multiple = a / align_max;

    return ((a % align_max == 0) ? multiple : multiple + 1) * align_max;
}

//       En-tête                                          En-tête
//       <------------------->                            <------------------->
//       +---------+---------+-----------------+          +---------+---------+-----------------+
//       | Taille  | Suivant |   Bloc alloué   |          | Taille  | Suivant |   Bloc alloué   |
//       +---------+---------+-----------------+          +---------+---------+-----------------+
//                      +                                 ^
//                      |                                 |
//                      +---------------------------------+

static struct bloc *recherche_bloc_libre(size_t taille)
{
    /*
     *      * Recherche un bloc libre ou une suite de bloc libres dont la taille
     *           * est au moins égale à `taille`.
     *                */

    struct bloc *bloc = libre;
    struct bloc *precedent = NULL;
    struct bloc *ret = NULL;

    while (bloc != NULL)
    {
        if (bloc->taille >= taille)
        {
            if (precedent != NULL)
                precedent->suivant = bloc->suivant;
            else
                libre = bloc->suivant;

            ret = bloc;
            break;
        }

        precedent = bloc;
        bloc = bloc->suivant;
    }

    return ret;
}


static void *__hp_malloc(size_t taille)
{
    static size_t alloue = 0UL;
    void *ret;

    assert(taille > 0);

    taille = calcule_multiple_align(taille);

    ret = recherche_bloc_libre(taille);

    if (!ret)
    {
        // Vérification sans débordement
        if (alloue > MEM_SIZE ||
            TAILLE_EN_TETE > MEM_SIZE - alloue ||
            taille > MEM_SIZE - alloue - TAILLE_EN_TETE)
        {
            fprintf(stderr,
                    "Il n'y a plus assez de mémoire disponible.\n");
            return NULL;
        }

        // Adresse de l'en-tête
        char *bloc = (char *)reserve.mem + alloue;

        // Adresse des données utilisateur
        ret = bloc + TAILLE_EN_TETE;

        // Initialisation des métadonnées
        bloc_init(bloc, taille);

        // Avancement dans la réserve
        alloue += TAILLE_EN_TETE + taille;
    }

    return ret;
}

static void __hp_free(void *ptr)
{
    /*
     *      * Ajoute le bloc fourni à la liste des blocs libres.
     *           */

    struct bloc *bloc = libre;
    struct bloc *nouveau;

    if (!ptr)
        return;

    nouveau = (struct bloc *)((char *)ptr - TAILLE_EN_TETE);
    nouveau->suivant = NULL;

    while (bloc && bloc->suivant != NULL)
        bloc = bloc->suivant;

    if (bloc == NULL)
        libre = nouveau;
    else
        bloc->suivant = nouveau;
}



//------------------------------------------------------------------------
//Init and finalize to allocate and deallocate huge pages
//
__attribute__((constructor))
static void hp_init(){
    static int is_initialized=0;
   // if(! is_initialized){
   //     is_initialized=1;
        //open the file into hugefs
        __hp_fd = open(FILE_NAME, O_CREAT | O_RDWR, 0755);
        if (__hp_fd < 0) {
            perror("Open failed");
            fprintf(stderr,"check you have done : \n echo 200 > \
                    /proc/sys/vm/nr_hugepages\n mount -t hugetlbfs -o mode=0777 none \
                    /mnt/huge");
            exit(1);
        }

        //mapping memory
        //buffer = mmap(ADDR, MEM_SIZE, PROTECTION, FLAGS, __hp_fd, 0);
        reserve.mem = mmap(ADDR, MEM_SIZE, PROTECTION, FLAGS, __hp_fd, 0);
        //if (buffer == MAP_FAILED) {
        if (reserve.mem == MAP_FAILED) {
            perror("mmap");
            unlink(FILE_NAME);
            exit(1);
        }
    //}
}
__attribute__((destructor))
static void hp_finalize(){
	//free and close
	//munmap(buffer, MEM_SIZE);
	munmap(reserve.mem, MEM_SIZE);
	close(__hp_fd);
	unlink(FILE_NAME);
}


//------------------------------------------------------------------------
//API Implementation
//
void *hp_malloc(size_t taille){
    return __hp_malloc(taille);
}

void hp_free(void *ptr){
    return __hp_free(ptr);
}
//------------------------------------------------------------------------


