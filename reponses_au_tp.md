# TP n°1

## Q.1
L'algorithme de bench est implémenté dans le fichier **bench.c** dans la fonction *benchRead* commençant ligne 111.

## Q.2
Le benchemarck "chauffe" le cache, c'est à dire qu'il lit un peu du buffer de telle sorte que le buffer soit chargé dans 
le cache. Ensuite une série de lecture du buffer est efféctuée afin de mesurer le temp de lecture du buffer dans le cache.

## Q.3
question répondu à la question précédente.

## Q.5
On observe une première montée lors du passage du cache L1 au cache L2, suivie d'une seconde montée lors de la transition du cache L2 au cache L3 (vers 200 000 doubles). Ensuite, le temps d'accès reste relativement stable jusqu'à ce que la capacité du TLB soit dépassée.

À partir de ce seuil, le temps de lecture augmente de manière linéaire en raison des TLB misses répétés. Lors d'un TLB miss, le processeur doit parcourir les tables de pages en RAM pour traduire l'adresse virtuelle en adresse physique, puis mettre à jour le TLB, ce qui génère une surcout significatif à chaque accès mémoire.

## Q.6
Une huge page est une page mémoire qui contient plus de donnée qu'une page mémoire classique.

## Q.8
Après l'activation des *huge pages*, moins de pages sont nécessaires pour stocker les mêmes données, ce qui réduit considérablement le nombre d'entrées utilisées dans le TLB. Dans le cadre de mon processeur **Ryzen 7 3800X**, la capacité du TLB combinée aux 32 Mo de cache L3 permet de couvrir l'ensemble du jeu de données sans provoquer de TLB misses significatifs, ce qui explique l'absence de remontée sur la courbe.



## Q.10
*hp_init* est applé avant le lancement du main à l'uverture de la librairie. *hp_init* ouvre un fichier en lecture par 
les flux noyau. le fichier ouvert est le fichier correspondant à la librairie via la variable *FILE_NAME*. Ensuite une 
certaine quantité de mémoire lui est alouée. *hp_finelize* est appelé après main et sert à fermer la librairie.

## Q.11
La variable <alloue> (alloué en langage naturel) correspond à la quantité de mémoire alloué par notre malloc.
Cette variable est déclarée **static** ce qui met cette variable en commun a tous les appel de cette fonction.
On peux ainsi tenir le compte d'une nombre de bloque alloué. Cette variable est stoquée dans le segment donnée
et non pas sur la pile ou le tas.


## Q.12
La fonstion *hp_malloc* est la plus naïve des allocation mémoire possible. Cette fonction vérifie qu'il reste assez
de place pour ajouter un bloque de mémoire et en ajoute un si possible.

## Q.13
``` c
static void *__hp_malloc(size_t taille)
{
    /*
     *      * Alloue un bloc de mémoire au moins de taille `taille`.
     *           */

    static size_t alloue = 0UL;
    void *ret;

    assert(taille > 0);
    //La fonction calcule_multiple_align() détermine le plus petit
    //multiple de l’alignement maximal égal ou supérieur à a.
    //Celle-ci nous permet d’« arrondir » la taille demandée de
    //sorte que les blocs alloués soit toujours correctement alignés.
    
    taille = calcule_multiple_align(taille);

    if (taille < MEM_SIZE - alloue)
    {
        fprintf(stderr, "Il n'y a plus assez de mémoire disponible.\n");
        return NULL;
    }

    ret = (void*) (alloue+1);
    alloue += taille;
    return ret;

}
```

## Q.14
Cette implementation soufre du défaut de ne pas pouvoir alloué de mémoire si nous avons attaint la fin de notre mémoire et ceci 
même si nous libérons des bloqued. Cela proviens du fait que chaque allocation a lieu à la fin de la mémoire.

## Q.15
La fonction parcour la liste chainée des blocs mémoire tant que la position où nous allons inserer notre nouveau bloc n'est pas 
encore determinée. Si le bloc courant est assez grand on le met dans <ret> que nous retournerons.
Lorsque qu'un bloc assez grand est trouvé il est retiré e la chaine pour ^tre renvoyé. dns le cas contraire la fonction revoie le pointeur NULL.


## Q.16
``` c
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
```


## Q.17 
On recherche un bloque libre de la bonne taille parmis les bloque libéré. Si nous n'en avons pas trouvé on peut en créer un nouveaux dans le cas ou la mémoire est suffisante.

## Q.18
```c
static void *__hp_malloc(size_t taille)
{
    /*
     *      * Alloue un bloc de mémoire au moins de taille `taille`.
     *           */

    static size_t alloue = 0UL; 
    void *ret;

    assert(taille > 0);

    //La fonction calcule_multiple_align() détermine le plus petit
    //multiple de l’alignement maximal égal ou supérieur à a.
    //Celle-ci nous permet d’« arrondir » la taille demandée de
    //sorte que les blocs alloués soit toujours correctement alignés.
    taille = calcule_multiple_align(taille); 

    ret = recherche_bloc_libre(taille); //recherche d'un bloc libre

    if (!ret)
    {
        if (MEM_SIZE - alloue < TAILLE_EN_TETE + taille)
        {
            fprintf(stderr, "Il n'y a plus assez de mémoire disponible.\n");
            return NULL;
        }

        ret = reserve.mem + alloue + TAILLE_EN_TETE;
        alloue += taille;
        bloc_init(ret, taille);
    }

    return ret;
}
```

## Q.19
__hp_free verifie que le pointeur entré en paraùètre n'est pas le pointeur null. ensuite il parcours la chaine des pointeur liberé jusq'à arrivé au bout. Il ajoute le nouveau bloque de mémoire libéré en fin de la chaine de méméoire libre.

## Q.20
```c
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
```