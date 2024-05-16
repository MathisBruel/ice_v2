!/bin/bash

TMPDIR="/tmp/ICE2"
DIRECTORY="/data/ICE2"
SERVICEDIR="/etc/systemd/system"
VERSION="2.0"

# -- TEMP DATAS
OVERWRITE=0
RAWCOPY=0

function printTitle {
    tput setaf 6
    echo -e "\n\n################################"
    echo -e $1
    echo -e "################################\n"
    tput setaf 7
}

function printInfo {
    tput setaf 5
    echo -e $1
    tput setaf 7
}

function printAction {
    tput setaf 4
    echo -e $1
    tput setaf 7
}

function printWarning {
    tput setaf 3
    echo -e $1
    tput setaf 7
}

function printError {
    tput setaf 1
    echo -e $1
    tput setaf 7
}

function printValid {
    tput setaf 2
    echo -e $1
    tput setaf 7
}

function checkPackage {
    name=$1
    HASPACKAGE=`apt list --installed | grep '$name' -c`
    if [[ $HASPACKAGE == 0 ]]
    then
        printWarning "$name not installed : install it."
        apt-get install $name
    else
        printValid "$name is installed."
    fi
}

function createFolder {
    path=$1
    checkOverwrite=$2

    OVERWRITE=0
    RAWCOPY=0

    if [[ -d "$path" ]]
    then

        if [[ $checkOverwrite == 1 ]]
        then
            printWarning "$path already exists on your filesystem."
            read -p "Do you want to overwrite files ? : " temp
            if [[ $temp == "yes" ]] ; then OVERWRITE=1 ; fi
        else 
            printValid "$path already exists on your filesystem."
        fi
    else
        RAWCOPY=1
        printAction "Create directory $path on your filesystem."
        mkdir $path
        printAction "Create rights on directory $path."
        chmod -R 775 $path
        chown -R root:sudo $path
    fi
}

function createFolderOrEmpty {
    pathEmpty=$1

    if [[ ! -d "$pathEmpty" ]]
    then
        printAction "Create directory for old files."
        mkdir $pathEmpty
    else
        tput setaf 3
        printWarning "Remove old saved files."
        rm $pathEmpty/*
    fi
}

function copyFilesToFolder {
    SRC=$1
    DST=$2

    if [[ $RAWCOPY == 1 ]]
    then
        printAction "Copy default config files."
        cp $SRC/* $DST
    elif [[ $OVERWRITE == 1 ]]
    then
        createFolderOrEmpty "$DST"_OLD
        printAction "Back up files."
        mv $DST/* "$DST"_OLD
        printAction "Overwrite default config files."
        cp $SRC/* $DST
    fi
}

function stopService {

    service=$1
    printInfo "Check if $service service is running."
    SERVICEACTIVE=`systemctl status $service | grep 'active (running)' -c`
    if [[ $SERVICEACTIVE == 1 ]]
    then
        printWarning "$service service is running : stopping it."
        systemctl stop $service
        printInfo "Waiting 5 seconds to stop correctly."
        sleep 5
    fi
}

# -- check packages
printTitle "Check needed packages"
checkPackage "openssl"
checkPackage "libcrypto"
checkPackage "nodejs"
# --end check packages

# -- master directory
printTitle "Check global directory"
createFolder "$DIRECTORY" 0
# --end master directory

# -- extraction
printTitle "Extraction"
createFolder "$TMPDIR" 0
printAction "Extract."
tar xzf ICE-CORE-$VERSION.tar.gz -C $TMPDIR
# --end extraction

# -- configuration
printTitle "Configuration"
printInfo "Check directory containing scripts."
createFolder "$DIRECTORY/SCRIPTS" 0
printInfo "Check directory containing logs."
createFolder "$DIRECTORY/LOG" 0
printInfo "Check directory containing configuration files."
createFolder "$DIRECTORY/CONFIG" 1
printInfo "Apply changes on config files."
copyFilesToFolder "$TMPDIR/CONFIG" "$DIRECTORY/CONFIG"
printInfo "Check directory containing calibration files."
createFolder "$DIRECTORY/LUTS" 1
printInfo "Apply changes on calibration files."
copyFilesToFolder "$TMPDIR/LUTS" "$DIRECTORY/LUTS"
# --end configuration

# -- ICE-CORE
printTitle "ICE CORE"
stopService "ICE-CORE"
# --end ICE-CORE

# -- ICE-CORE exe
printInfo "Check ICE-CORE executable."
if [[ -f $DIRECTORY/ICE-SERVICE ]] ; then
    printAction "Delete old ICE-CORE exe."
    rm $DIRECTORY/ICE-SERVICE
fi
printAction "Copy new ICE-CORE exe."
cp $TMPDIR/ICE-SERVICE $DIRECTORY/
# --end ICE-CORE exe

# -- ICE-CORE service
printInfo "Check ICE-CORE service."
if [[ -f $SERVICEDIR/ICE-CORE.service ]] ; then
    printAction "Delete old ICE-CORE service."
    rm $SERVICEDIR/ICE-CORE.service
fi
printAction "Copy new ICE-CORE service."
cp $TMPDIR/ICE-CORE.service $SERVICEDIR/
printAction "Reload daemons and force to start when reboot."
systemctl daemon-reload
systemctl enable ICE-CORE
# --end ICE-CORE service

# -- ICE-WEB service
printTitle "ICE WEB"
stopService "ICE-WEB"
# --end ICE-WEB service

# -- remove old WebInterface
printInfo "Check web interface directory."
createFolderOrEmpty "$DIRECTORY/WebInterface"
printAction "Copy web interface for ICE-CORE."
cp -r $TMPDIR/WebInterface/* $DIRECTORY/WebInterface/
# --end remove old WebInterface

printInfo "Check ICE-WEB service."
if [[ -f $SERVICEDIR/ICE-WEB.service ]] ; then
    printAction "Delete old ICE-WEB service."
    rm $SERVICEDIR/ICE-WEB.service
fi
printAction "Copy new ICE-WEB service."
cp $TMPDIR/ICE-WEB.service $SERVICEDIR/
printAction "Reload daemons and force to start when always."
systemctl daemon-reload
systemctl enable ICE-WEB

printTitle "Start Web interface"
systemctl start ICE-WEB

printTitle "Delete temp files."
rm -Rf $TMPDIR