#! /usr/bin/env sh

build_all_external() {
    for sts in 1 2 3 4 5 8; do
        stage=$sts
        build_external
    done
}

build_external() {

    # Prepare and clean repository
    if [ -d "EXTERN/CMAKE" ] ; then
        rm -rf "EXTERN/CMAKE"
    fi
    mkdir -p "EXTERN/CMAKE"
    cd "EXTERN/CMAKE"

    # Build and execute generation command
    cmdGen="cmake"
    if [ -n "$generator" ] ; then
        cmdGen="$cmdGen -G $generator"
    fi
    if [ -n "$archi" ] ; then
        cmdGen="$cmdGen -A $archi"
    fi
    cmdGen="$cmdGen -DSTAGE_COMPILATION=$stage .."
    echo $cmdGen
    eval $cmdGen

    # Build and execute build command
    cmdBuild="cmake --build ."
    if [ -n "$config" ] ; then
        cmdBuild="$cmdBuild --config $config"
    fi
    echo $cmdBuild
    eval $cmdBuild

    # Build and execute install command
    cmdInstall="cmake --install ."
    echo $cmdInstall
    eval $cmdInstall
    
    cd ../..
}

build_lib_exe() {
    
    if [ -d "CMAKE" ] ; then
        rm -rf "CMAKE"
    fi
    mkdir -p "CMAKE"
	cd "CMAKE"

    # Build and execute generation command
    cmdGen="cmake"

    if [ -n "$buildtype" ] ; then
        cmdGen="$cmdGen -DCMAKE_BUILD_TYPE=$buildtype"
    fi
    if [ -n "$generator" ] ; then
        cmdGen="$cmdGen -G $generator"
    fi
    if [ -n "$archi" ] ; then
        cmdGen="$cmdGen -A $archi"
    fi
    cmdGen="$cmdGen -DSTEP=$step .."
    echo $cmdGen
    eval $cmdGen
 
    cmake --build . --config $config

    # Build and execute install command
    cmdInstall="cmake --install ."
    echo $cmdInstall
    eval $cmdInstall
}

build_package() {

    VERSION="2.0"
    if [ "$step" = "CENTRAL" ] ; then
        if [ ! -d "BUILD/PACKAGE/CENTRAL" ] ; then
            mkdir -p "BUILD/PACKAGE/CENTRAL"
        fi
        mkdir "BUILD/PACKAGE/CENTRAL/TEMP"
        cp EXE/SERVICES/Launcher/install-central.sh BUILD/PACKAGE/CENTRAL
        cp -r BUILD/BIN/ICE-CENTRAL BUILD/BIN/ICE-MIGRATION EXE/SERVICES/WebCentral BUILD/PACKAGE/CENTRAL/TEMP
        cp -r EXE/SERVICES/Launcher/ICE-CENTRAL.service BUILD/PACKAGE/CENTRAL/TEMP

        cd BUILD/PACKAGE/CENTRAL/TEMP
        tar -czf ICE-CENTRAL-$VERSION.tar.gz *
        mv ICE-CENTRAL-$VERSION.tar.gz ../
        cd ../
        rm -Rf "TEMP"
    fi

    if [ "$step" = "CORE" ] ; then
        if [ ! -d "BUILD/PACKAGE/CORE" ] ; then
            mkdir -p "BUILD/PACKAGE/CORE"
        fi
        mkdir "BUILD/PACKAGE/CORE/TEMP"
        cp EXE/SERVICES/Launcher/install-core.sh BUILD/PACKAGE/CORE
        cp -r BUILD/BIN/ICE-SERVICE EXE/SERVICES/WebInterface BUILD/PACKAGE/CORE/TEMP
        cp -r EXE/SERVICES/Launcher/CONFIG BUILD/PACKAGE/CORE/TEMP
        cp -r EXE/SERVICES/Launcher/LUTS BUILD/PACKAGE/CORE/TEMP
        cp -r EXE/SERVICES/Launcher/ICE-CORE.service BUILD/PACKAGE/CORE/TEMP
        cp -r EXE/SERVICES/Launcher/ICE-WEB.service BUILD/PACKAGE/CORE/TEMP


        cd BUILD/PACKAGE/CORE/TEMP
        tar -czf ICE-CORE-$VERSION.tar.gz *
        mv ICE-CORE-$VERSION.tar.gz ../
        cd ../
        rm -Rf "TEMP"
    fi
}

# parse command line
while getopts "f:s:g:a:c:t:b:" OPTION; do
    case "$OPTION" in

        f)
            function="${OPTARG}"
            echo "Option function : $function"
                ;;

        s)
            stage="${OPTARG}"
            echo "Option stage : $stage"
                ;;

        g)
            generator="\"${OPTARG}\""
            echo "Option generator : $generator"
                ;;

        a)
            archi="\"${OPTARG}\""
            echo "Option archi : $archi"
                ;;
        
        b)
            buildtype="${OPTARG}"
            echo "Option build type : $buildtype"
                ;;
        
        c)
            config="${OPTARG}"
            echo "Option config : $config"
                ;;

        t)
            step="${OPTARG}"
            echo "Step : $step"
                ;;
            
    esac
done

# default values
if [ -z $config ] ; then
    config="Release"
fi

# call functions
$function
