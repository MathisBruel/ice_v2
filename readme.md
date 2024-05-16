# COMPILATION WINDOWS

## PREREQUISITE

- Visual Studio 15 2017 (ou plus récent).
- Qt 5.0 ou supérieur
- CUDA v11.6 minimum (pour le seam carving)
- OpenSSL-Win64

## BUILD

Se placer à la racine du répertoire de développement.
**Compilation des librairies externes** 
Lancer `./tasks -f build_all_external -g "Visual Studio 15 2017" -a x64`
Router le chemin de 'OpenSSL' dans le fichier CMakeLists.txt de la librairie LIB-IMS.

**Compilation des Plugins** 
Lancer `./tasks -f build_lib_exe -g "Visual Studio 15 2017" -a x64 t PLUGIN`
**Compilation des outils de calibration**
Router les chemins de 'QT' et 'OpenSSL' dans les fichiers CMakeLists.txt présents à la racine de chaque outil concerné.
Router le chemin de la librairie 'iphlpapi.lib' avec ses includes dans les fichiers CMakeLists.txt présents à la racine de chaque outil concerné.
Lancer `./tasks -f build_lib_exe -g "Visual Studio 15 2017" -a x64 t CALIB`
**Compilation des outils de synchronisation**
Router les chemins de QT et OpenSSL dans les fichiers CMakeLists.txt présents à la racine de chaque outil concerné.
Router le chemin de la librairie 'iphlpapi.lib' avec ses includes dans les fichiers CMakeLists.txt présents à la racine de chaque outil concerné.
Lancer `./tasks -f build_lib_exe -g "Visual Studio 15 2017" -a x64 t TOOLS`
**Compilation du seam carving**
Router les chemins de 'CUDA' dans la LIB-CUDA dans le fichier CMakeLists.txt.*
Router le chemin de la librairie 'iphlpapi.lib' avec ses includes dans le fichier CMakeLists.txt de 'PROCESSOR'.
Lancer `./tasks -f build_lib_exe -g "Visual Studio 15 2017" -a x64 t RaD`

Attention : pour les plugins ADOBE, les librairies externes compilées avec l'étape "build_all_external" doivent être compilées sans les option MT de windows (voir le fichier CMakeLists.txt du dossier "EXTERN").

## INSTALL

Les outils sont des exécutables directement copiables dans un répertoire pour une exécution directe.

# COMPILATION LINUX

## PREREQUISITE

Installer avec `apt-get`:

- libssl
- openssl
- cmake
- pthread
- mysqlclient
- crypto
- nodejs
- qtbase5-dev qtbase5-dev-tools libqt5sql5 libqt5sql5-sqlite libqt5sql5-mysql
- curl zip unzip tar
- librange-v3-dev

Intaller la lib `tabulate` :
```
cd && sudo git clone https://github.com/p-ranav/tabulate.git
cd tabulate && sudo mkdir BUILD && cd BUILD 
sudo cmake -G "Unix Makefiles" ..
sudo make install
cd && sudo rm -r tabulate
```

## BUILD

Se placer à la racine du répertoire de développement.

**Compilation des librairies externes** 
Lancer `./tasks.sh -f build_all_external`
**Compilation des exécutables ICE CORE et gestion centralisée (non terminée)**
Lancer `./tasks.sh -f build_lib_exe -t CORE`
**Package installer ICE CORE**
Lancer `./tasks.sh -f build_package -t CORE`
**Compilation des exécutables ICE CENTRAL et ICE MIGRATION**
Lancer `./tasks.sh -f build_lib_exe -t CENTRAL`
**Package installer ICE CENTRAL**
Lancer `./tasks.sh -f build_package -t CENTRAL`

## INSTALL

Pour installer ICE-CORE, il faut lancer l'installation du package via le script install-core.sh contenu dans l'archive 'ICE-CORE-$VERSION.tar.gz'.
L'interface s'installe en copiant tout le contenu du répertoire WebInterface et en lançant les commandes suivantes : 

```
cd WebInterface
nodejs javascripts/Main_API.js javascripts/config.json
```