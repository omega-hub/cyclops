add_executable(helloEarth helloEarth.cpp)
set_target_properties(helloEarth PROPERTIES FOLDER examples)
target_link_libraries(helloEarth 
	omega 
	omegaToolkit
	omegaOsg
	cyclops)


