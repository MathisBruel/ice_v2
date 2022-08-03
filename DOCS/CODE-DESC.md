# ICE-CORE CODE

Ce document a pour but de décrire les sous-modules du code de ICE.

## COMMON

Ce dossier regroupe les librairies développées communes à tous le code.

### LIB-BASE

Cette librairie permet de gérer des éléments de base : la gestion d'un tableau d'octets, etc.

### LIB-CONF

Cette librairie permet de charger tous les fichiers de configuration de ICE-V2.

### LIB-IMAGE

Cette librairie permet de gérer les images : load/save des images. Charger des scripts CIS, gérer les fichiers de calibration colorimétrique.

### LIB-IMS

Cette librairie permet de communiquer avec tous les types de serveurs de projection (Barco, Christie, Doremi) via une API commune.

### LIB-KINET

Cette librairie permet de communiquer avec tous les équipements ICE : les panneaux et les éclairages.

### LIB-CMD

Cette librairie permet de recevoir toutes les commandes via les systèmes.
Elle définit toutes les commandes, possède un receveur HTTP de commande, un UDP de commande (envoyé depuis les serveurs de projection) et un receveur de stream HTTP (envoyé depuis PremierePro).

### LIB-REPOSITORY

Cette librairie permet de gérer les contenus : savoir ce qui est présent sur disque et de télécharger de nouveaux scripts automatiquement sur le FTP.

### LIB-SYNCHRO

Cette librairie permet de gérer les formats des fichiers synchros (LVI, CPL, SYNC) et lancer des synchronisation.

### LIB-TEMPLATE

Cette librairie permet de définir le format du template dans le Cis. Selon l'architecture, plusieurs formats peuvent être définis.

### LIB-3D

Cette librairie permet d'effectuer les projections des images dans les panneaux, ainsi que le positionnement des moving-heads.

### LIB-CONTEXT

Cette librairie est simplement le contexte commun à une application. Elle stocke des données accessibles en multi-thread qui sont protégées par des mutexs.


## EXTERN

Ce dossier contient toutes les librairies externes mais nécessaire au projet.
Elles doivent être préalablement compilées.

Note : pour les plugins ADOBE, il faut les recompiler sans les flags MT.

## DOCS

Ce dossier regroupe toute la documentation.

## EXE

### PLUGIN

Ce dossier contient tous les plugins ADOBE ainsi que son SDK.

### R&D

Ce dossier contient le seam carving ainsi que sa librairie CUDA exécutée directement sur la carte graphique.

### SERVICES

Ce dossier contient toutes les applications finales du système. Le coeur de ICE et son interface nodeJS, le coeur de la gestion centrale et son interface. Le coeur du système HTTP de fournisseur de fichier.

### TOOLS

Ce dossier contient tous les outils de ICE : 

- les encodeurs/décodeurs base64
- Les outils de calibration colorimétrique
- les outils de correction de position de moving-heads
- les outils de synchronisation de script

### UNITY

Ce dossier contient tout le projet UNITY pour la VR.

