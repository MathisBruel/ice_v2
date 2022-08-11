# PLUGINS

Les plugins sont des petits composants intégrés à la solution Premiere Pro d'Adobe pour aider la post-production à générer le contenu ICE. Il existe 3 plugins : 

- Exporter
- Transmitter
- MovingHeads

Tous ces composants sont développés en C/C++ et s'aide du SDK Premiere Pro ou After effect.

## Exporter 

Le plugin d'exporter permet d'exporter le projet Premiere Pro en tant que contenu ICE au format CIS.
Par rapport à l'ancienne version, plus besoin de générer la synchronisation à cette étape. Le plugin ne génère uniquement le fichier CIS. La synchronisation sera réalisée par la suite grâce à l'outil Syncer qui permet de merger différents CIS, d'effectuer les premières synchros en s'aidant visuellement de la CPL de référence et d'effectuer de nouvelle synchronisation tout en s'aidant visuellement. Pour cette dernière partie, des checks supplémentaires sont ajoutés pour valider que la synchronisation est cohérente et valide. Le temps de rendu est bien plus rapide qu ela version précédente car le multithread natif de rendu de Premiere Pro est activé.
On passe d'un ratio 8/1 à un ratio inférieur à 1/1.

## Transmitter

Le transmitter est un composant permettant de forwarder un message via TCP contenant l'image du template de la frame courante dans Premiere Pro. Rien de bien complexe en somme.

## MovingHead

Le plugin de movingHead permet d'ajouter un effet à un calque Premiere Pro et de piloter une movingHead avec tout un panel d'outils.
Chaque séquence où l'effet est ajouté représente le contrôle d'une movingHead. Un effet peut être enregistré sur disque et être réutilisé par la suite.

Toutes les données et méthodes permettant son contrôle de l'instance est représenté par une instance de classe MovingHandler.
Au démarrage de l'application, les preset sont loadés via une instance de classe PresetLoader.
Toutes les actions natives de Premiere Pro via l'interface de l'effet sont gérés en point d'entrée par une instance de MovingControler.  

L'outil, très complet, permet de : 

- changer la movingHead contrôlée
- rendre visible ou non le rendu graphique du contrôle de la movingHead
- de définir un point spatial de pointage de la moving
    - activation/désactivation
    - set de position
    - changer le type d'interpolation entre le point courant et le point suivant
        - linéaire
        - courbé (utiliser extra ou extra2 pour le réglage)
        - arquer (utiliser extra ou extra2 pour le réglage)
- de définir un point de couleur de la moving
    - activation/désactivation
    - set de la couleur (avec l'alpha pour le niveau d'intensité)
    - changer le type d'interpolation entre le point courant et le point suivant
        - linéaire
        - exponentiel (slow then fast : utiliser extra pour le réglage de la vitesse)
        - exponentiel (fast then slow : utiliser extra pour le réglage de la vitesse)
    - resizer le timeline : augmenter ou réduire le temps d'application de l'effet (tout l'effet est gardé)
    - translater l'effet : celui-ci coupe les points qui se trouveront en dehors de la timeline
- looper l'effet : l'effet est recopié X fois dans la timeline pour la remplir entièrement
    - en inverse : sens inverse de l'ffet recopié en alternance avec le sens droit
    - en droit : l'effet doit commencer et finir avec les mêmes positions et les mêmes couleurs pour que cela fonctionne
- inverser toutes les positions de l'effet horizontalement
- inverser toutes les positions de l'effet verticalement
- rescale toutes les positions de l'effet
- translater toutes les positions de l'effet
- faire tourner toutes les positions de l'effet selon un point paramétrable
- sauvegarder/charger un preset de positions
- sauvegarder/charger un preset de couleurs