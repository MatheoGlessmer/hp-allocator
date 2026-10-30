#!/bin/bash

# 1. Configurer le nombre de Huge Pages (avec tee)
echo 200 | sudo tee /proc/sys/vm/nr_hugepages > /dev/null

# 2. Créer le point de montage si nécessaire
sudo mkdir -p /mnt/huge

# 3. Monter le système de fichiers hugetlbfs
sudo mount -t hugetlbfs -o mode=0777 none /mnt/huge