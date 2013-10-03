# cyclops depends on OSG
if(OMEGA_BUILD_OSG_LIB)
	add_definitions(-DOSGWORKS_STATIC)
	include_directories(./)
	include_directories(${OSG_INCLUDES})
	
	add_subdirectory(src)
	add_subdirectory(examples/cyhello)
	add_subdirectory(examples/cyhello2)
endif()

###############################################################################
# Install commands
install(DIRECTORY common DESTINATION omegalib/cyclops)
install(DIRECTORY test DESTINATION omegalib/cyclops)


