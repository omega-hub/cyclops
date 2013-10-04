if(MODULES_omegaOsg)
	add_definitions(-DOSGWORKS_STATIC)
	include_directories(.)
	include_directories(${OSG_INCLUDES})
	
	add_subdirectory(src)
	add_subdirectory(examples/cyhello)
	add_subdirectory(examples/cyhello2)
else()
	# cyclops depends on OSG
	set(MODULES_omegaOsg true CACHE BOOL " " FORCE)
	# create this variable to trigger a CMake re-generate step
	set(cyclops_REGENERATE_TRIGGER true CACHE INTERNAL)
endif()

###############################################################################
# Install commands
install(DIRECTORY common DESTINATION omegalib/cyclops)
install(DIRECTORY test DESTINATION omegalib/cyclops)


