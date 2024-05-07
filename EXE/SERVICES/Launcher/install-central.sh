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
checkPackage "nodejs"
checkPackage "libssl3"
checkPackage "wget"
checkPackage "libmysqlclient-dev"

# -- master directory
printTitle "Check global directory"
createFolder "$DIRECTORY" 0

# -- extraction
printTitle "Extraction"
createFolder "$TMPDIR" 0
printAction "Extract."
tar xzf ICE-CENTRAL-$VERSION.tar.gz -C $TMPDIR

# -- configuration
printTitle "Configuration"
printInfo "Check directory containing logs."
createFolder "$DIRECTORY/LOG" 0

# -- ICE-CENTRAL
printTitle "ICE CENTRAL"
stopService "ICE-CENTRAL"

# -- ICE-CENTRAL exe
printInfo "Check ICE-CENTRAL executable."
if [[ -f $DIRECTORY/ICE-CENTRAL ]] ; then
    printAction "Delete old ICE-CENTRAL exe."
    rm $DIRECTORY/ICE-CENTRAL
fi
printAction "Copy new ICE-CENTRAL exe."
cp $TMPDIR/ICE-CENTRAL $DIRECTORY/

# -- ICE-MIGRATION
printTitle "ICE MIGRATION"
stopService "ICE-MIGRATION"

# -- ICE-MIGRATION exe
printInfo "Check ICE-MIGRATION executable."
if [[ -f $DIRECTORY/ICE-MIGRATION ]] ; then
    printAction "Delete old ICE-MIGRATION exe."
    rm $DIRECTORY/ICE-MIGRATION
fi
printAction "Copy new ICE-MIGRATION exe."
cp $TMPDIR/ICE-MIGRATION $DIRECTORY/

# -- ICE-CENTRAL service
printInfo "Check ICE-CENTRAL service."
if [[ -f $SERVICEDIR/ICE-CENTRAL.service ]] ; then
    printAction "Delete old ICE-CENTRAL service."
    rm $SERVICEDIR/ICE-CENTRAL.service
fi
printAction "Copy new ICE-CENTRAL service."
cp $TMPDIR/ICE-CENTRAL.service $SERVICEDIR/
printAction "Reload daemons and force to start when reboot."
systemctl daemon-reload
systemctl enable ICE-CENTRAL

# -- remove old webCentral
printInfo "Check web central directory."
createFolderOrEmpty "$DIRECTORY/WebCentral"
printAction "Copy web central for ICE-CORE."
cp -r $TMPDIR/WebCentral/* $DIRECTORY/WebCentral/

printTitle "Start Web interface"
systemctl start ICE-CENTRAL

printTitle "Delete temp files."
rm -Rf $TMPDIR