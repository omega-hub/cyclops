request_dependency(omegaOsg)
module_version(1.9)

add_definitions(-DOSGWORKS_STATIC)
include_directories(.)
include_directories(${OSG_INCLUDES})
include_directories(${OSGBULLET_INCLUDES})

add_subdirectory(src)
if(OMEGA_BUILD_EXAMPLES)
    add_subdirectory(examples/cyhello2)
    if(MODULES_omegaOsgEarth)
        add_subdirectory(examples/helloEarth)
    endif()
endif()

#-------------------------------------------------------------------------------
# Install commands
install(DIRECTORY common DESTINATION omegalib/cyclops)
install(DIRECTORY test DESTINATION omegalib/cyclops)
install(DIRECTORY examples/python/ DESTINATION omegalib/examples)

if(WIN32)
    install(FILES orun.bat DESTINATION omegalib)
    install(FILES physics.bat DESTINATION omegalib)
    install(FILES spincube.bat DESTINATION omegalib)
endif()

set_target_properties(cyclops PROPERTIES FOLDER modules)
