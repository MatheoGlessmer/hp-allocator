# hp-allocator

Projet ENSIIE (TD Architecture des systèmes d'exploitation) : un allocateur
mémoire en C, en espace utilisateur, qui réserve une zone de mémoire
physiquement contiguë grâce aux huge pages, et une mesure de l'effet des
huge pages sur les performances (TLB et caches).

## Contexte

Sous Linux, les pages physiques sont attribuées de façon quasi aléatoire.
Une application peut recevoir plusieurs pages qui tombent sur les mêmes voies
du cache, et un grand jeu de données sature le TLB (chaque défaut de TLB
coûte un parcours des tables de pages). Les huge pages (2 Mo) réduisent le
nombre d'entrées TLB nécessaires.

## Contenu

- Un allocateur en deux étapes : version naïve (curseur qui avance
  uniquement), puis version avec liste chaînée de blocs libres réutilisés.
  `[préciser les dossiers : hp_allocator_malloc, hp_allocator_malloc_free, ...]`
- Les mesures du benchmark de lecture avec pages standards et avec huge pages
  (`bench_malloc.txt`, `bench_huge.txt`).
- Le rapport : `rapport.pdf`.

## Résultats

Mesures sur un Ryzen 7 3800X, temps de lecture en fonction de la taille du
buffer :

| Taille du buffer | Pages standards | Huge pages |
|---|---|---|
| < ~1,4 M de doubles (~11 Mo) | ≈ 0,016 s | ≈ 0,02 s (surcoût fixe de 2 à 3 ms) |
| > ~1,4 M de doubles | monte jusqu'à ≈ 0,035 s (défauts de TLB) | stable ≈ 0,02 s |

Les huge pages ne sont donc avantageuses que pour de grands jeux de données.
`[ajouter la figure de comparaison : ![comparaison](figures/comparaison.png)]`

## Limites

- Pas de fusion des blocs libres adjacents, premier bloc suffisant choisi :
  fragmentation à long terme.
- `__hp_free` parcourt la liste pour insérer en fin : coût linéaire.
- La bibliothèque dynamique et le remplacement de `malloc`/`free` n'ont pas
  été terminés.
- Les huge pages doivent être réservées au niveau du système.
- Pistes d'amélioration : détection de double free, protection des en-têtes
  de blocs. `[à vérifier par rapport à ton code actuel]`

## Compilation et utilisation

`[commandes make, réservation des huge pages, exemple d'exécution]`

## Crédits

Le sujet et le benchmark du TD sont fournis par l'enseignant et ne sont pas
redistribués ici. L'allocateur (`__hp_malloc`, `__hp_free`, liste de blocs
libres) est de moi. `[outils d'IA éventuels : usage de Claude pour realiser le readme à partir du rapport]`