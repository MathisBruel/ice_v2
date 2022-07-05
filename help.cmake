function(ICE_link_library base_path library_name target)

    file(GLOB LIB_FILES
        "${base_path}/*.lib"
        "${base_path}/lib/*.lib"
        "${base_path}/*.a"
        "${base_path}/lib/*.a"
        "${base_path}/lib/x64/*.a"
    )

    foreach(lib_name ${LIB_FILES})
        target_link_libraries(${target} PUBLIC "${lib_name}")
    endforeach()

    target_include_directories(${target} PUBLIC "${base_path}/include/")
endfunction()

function(ICE_link_Poco_library base_path library_name target)

    file(GLOB LIB_FILES
        "${base_path}/*.lib"
        "${base_path}/lib/*.lib"
        "${base_path}/*.a"
        "${base_path}/lib/*.a"
    )

    list(APPEND LIBS PocoZip PocoDataSQLite PocoDataMySQL PocoData PocoNetSSL PocoRedis PocoNet PocoMongoDB PocoJWT PocoCrypto PocoUtil PocoXML PocoJSON PocoEncodings PocoFoundation CppUnit)

    foreach(component ${LIBS})
        foreach(lib_name ${LIB_FILES})
        string(FIND ${lib_name} ${component} FOUND)
        if(${FOUND} STREQUAL "-1")
        else ()
            target_link_libraries(${target} PUBLIC "${lib_name}")
        endif()
        endforeach()
    endforeach()

    target_include_directories(${target} PUBLIC "${base_path}/include/")
endfunction()

function(ICE_set_library base_path library_name library_variable)

    find_library(${library_variable} NAMES ${library_name} PATHS ${base_path} PATH_SUFFIXES lib)
    if (${${library_variable}} STREQUAL "library_variable-NOTFOUND")
        set(${library_name} "${${library_name}}d")
        find_library(${library_variable} NAMES ${library_name} PATHS ${base_path} PATH_SUFFIXES lib REQUIRED)
    endif()

    message ("Path library " ${library_variable} " : ${${library_variable}}")
endfunction()