cd '\\wsl$\Ubuntu-22.04\iceV2\ice_v2\BUILD\PACKAGE\'
cp ICE-CORE-2.0.tar.gz C:\Users\martin.lagrange\Documents\ICE-CORE-2.0.tar.gz
cp install-core.sh C:\Users\martin.lagrange\Documents\install-core.sh
cp Y:\\iceV2\ice_v2\EXE\SERVICES\Launcher\CONFIG\KinetConfiguration.xml C:\Users\martin.lagrange\Documents\KinetConfiguration.xml
cp Y:\\iceV2\ice_v2\EXE\SERVICES\Launcher\CONFIG\IceConfiguration.xml C:\Users\martin.lagrange\Documents\IceConfiguration.xml
cp Y:\\iceV2\ice_v2\EXE\SERVICES\Launcher\CONFIG\IMSConfiguration.xml C:\Users\martin.lagrange\Documents\IMSConfiguration.xml
cd '\\wsl$\Ubuntu-22.04\iceV2\ice_v2\DOCKER\'
cp dockerfile C:\Users\martin.lagrange\Documents\dockerfile
cd C:\Users\martin.lagrange\Documents\
docker build -t icev2 . 