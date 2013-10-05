request_dependency(omegaOsg)

add_definitions(-DOSGWORKS_STATIC)
include_directories(.)
include_directories(${OSG_INCLUDES})

add_subdirectory(src)
add_subdirectory(examples/cyhello)
add_subdirectory(examples/cyhello2)

###############################################################################
# Install commands
install(DIRECTORY common DESTINATION omegalib/cyclops)
install(DIRECTORY test DESTINATION omegalib/cyclops)


