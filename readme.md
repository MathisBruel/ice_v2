# COMPILATION WINDOWS

## PREREQUISITE

Installer Visual Studio 2017 (ou changer generator).
Installer InnoSetup (ajout dans le PATH)
Installer .NET Framework 3.0
Installer BOOST version : 
Installer TBB version : 

## BUILD

Se placer dans le répertoire CMAKE_OUT.

Lancer `cmake -G "Visual Studio 15 2017" -A x64 ..`
Une fois terminé, les projets Visual Studio sont générés

Lancer `cmake --build . --config Release`
Une fois terminé, les exécutables contiennent toutes les librairies (en statique) nécessaires à l'exécution dans ../BUILD.

Lancer `cmake --install . --config Release`
Une fois terminé, les exécutables contiennent toutes les librairies (en statique) nécessaires à l'exécution dans ../BUILD.

## SETUP (windows only)

Lancer `iscc -OBUILD setup.iss`
Puis lancer l'installation.

## TESTS

Se placer dans le répertoire de build de l'EXE.
Lancer `ctest -C Release .`

# COMPILATION LINUX

## PREREQUISITE

Installer avec `apt-get`:

- virtual-guest-additions.iso
- virtual-guest-utils
- libssl (version ?)
- openssl (version ?)
- cmake (version ?)
- pthread (`libpthread-stubs0-dev`)

## BUILD

Se placer dans le répertoire CMAKE_OUT_UNIX.

Lancer `cmake ..`
Une fois terminé, les projets Visual Studio sont générés

Lancer `cmake --build . --config Release`
Une fois terminé, les exécutables contiennent toutes les librairies (en statique) nécessaires à l'exécution dans ../BUILD.

## INSTALL (for unix ? )

## RUNNER

### INSTALLATION

Télécharger gitlab-runner : 
`curl -LJO "https://gitlab-runner-downloads.s3.amazonaws.com/latest/deb/gitlab-runner_${arch}.deb"`

Liste des ${arch} se trouve sur `https://gitlab-runner-downloads.s3.amazonaws.com/latest/index.html`
L'installer : `dpkg -i gitlab-runner_${arch}.deb`

### CONFIGURATION DU SERVICE

Générer le certificat : 

`mkdir /etc/gitlab-runner/certs`
`echo "HEAD / HTTP/1.0\n Host: gitlab.cgrcinemas.fr:443\n\n EOT\n" | openssl s_client -prexit -connect gitlab.cgrcinemas.fr:443 > /etc/gitlab-runner/certs/cgrcinemas.crt`

Enregister le runner : 

`gitlab-runner register --tls-ca-file=/etc/gitlab-runner/certs/cgrcinemas.crt`

Modifications avancées : 

Via `systemctl` et le nom du service `gitlab-runner.service`
Modification du fichier `/etc/gitlab-runner/config.toml`

Creation du script d'automatisation
Créer le fichier `.gitlab-ci.yaml`

Créer le fichier tasks.sh qui effectue le build et le lancement des tests



