request_dependency(omegaOsg)

add_definitions(-DOSGWORKS_STATIC)
include_directories(.)
include_directories(${OSG_INCLUDES})
include_directories(${OSGBULLET_INCLUDES})

if(MODULES_omegaOsgEarth)
    include_directories(${OSGEARTH_INCLUDES})
endif()

add_subdirectory(src)
add_subdirectory(examples/cyhello2)

###############################################################################
# Install commands
install(DIRECTORY common DESTINATION omegalib/cyclops)
install(DIRECTORY test DESTINATION omegalib/cyclops)
install(DIRECTORY examples/python/ DESTINATION omegalib/examples)

install(FILES orun.bat DESTINATION omegalib)
install(FILES physics.bat DESTINATION omegalib)
install(FILES spincube.bat DESTINATION omegalib)

