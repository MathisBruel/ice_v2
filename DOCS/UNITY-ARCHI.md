# UNITY Architecture

Le logiciel UNITY de la VR ICE est basé en C# sur le modèle de construction UNITY.
Tous les composants sont empaquetés dans des GameObject. On peut y associer des scripts de code C# ou des fonctionnalités natives de UNITY.

## Architecture dossier

Dans le dossier ASSET on retrouve : 

- Ressources/Materials : toutes les images et matériaux fixes qui permettront de créer la salle
- Ressources/Models : tous les modèles 3D au format .obj 
- Scripts : tous les scripts C# permettant la gestion de la camera, des scripts ICE, du load des fichiers de configuration, etc ...
- XR : librairie pour permettre le bon fonctionnement du casque VR

## Architecture GameObject

### Screen

Ce GameObject permet de gérer l'écran de projection via un composant natif de jeu de média vidéo, redéfinir sa taille selon la configuration, il contient un mesh surface définissant l'écran.

### Auditorium

Ce GameObject permet de gérer les murs et les escaliers de la salle. Le volume général est redimensionnée selon la configuration et les escaliers sont générés dynamiquement selon la configuration.

### Left/Right panels

Ce GameObject permet de générer les panneaux de la salle dynamiquement selon la configuration. Une probe de réflexion est générée de chaque côté pour permettre de refléter les lumières émises par l'écran et les panneaux dans la salle.

### PARS / Backlight / Movings

Ce GameObject permet de générer dynamiquement les lumières de scénographie ICE de la salle selon la configuration.

### Seats

Ce GameObject permet de générer dynamiquement les sièges de scénographie ICE de la salle selon la configuration.

### UI

Ce GameObject permet de générer l'interface 2D pour pouvoir contrôler la salle.

### XRRIG

Ce GameObject permet de gérer la caméra dans le monde 3D.

### Controller

Le controller est le centre de l'application. Ce controller va : 

- charger les fichiers de configuration
- générer dynamiquement la salle
- charger le liste de scripts ICE disponibles
- gérer le temps réel : 
    - démarrage/arrêt/pause de lecture d'un CIS
    - mouvements caméras
    - soit en récupérant les trames venant de Premiere Pro en TCP
    - soit en lisant les frames contenus dans le cis à jouer
    - traite les données pour mettre à jour les équipements et l'écran