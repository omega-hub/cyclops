add_definitions( -D_CRT_SECURE_NO_WARNINGS -DCY_EXPORTING)
if(WIN32)
	# Add the DOSGBULLET_STATIC to following projects, so we tell the header files 
	# we are not importing dll symbols.
	add_definitions(-DOSGBULLET_STATIC)
	if(MSVC)
		# Exclude libcmt when linking in visual studio
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
		set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
		add_definitions(-D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4018)
	endif(MSVC)
endif()

set(SRCS 
		Entity.cpp
		EffectNode.cpp
		AnimatedObject.cpp
		LineSet.cpp
		Light.cpp
		LightingLayer.cpp
		Material.cpp
		MaterialParser.cpp
		ModelLoader.cpp
		ModelGeometry.cpp
		RigidBody.cpp
		SceneLayer.cpp
		Shapes.cpp
		Skybox.cpp
		ShaderManager.cpp
		SceneLoader.cpp
		SceneManager.cpp
		ShadowMap.cpp
		StaticObject.cpp
		Text3D.cpp
		Uniforms.cpp
		cyclopsPythonApi.cpp)
    
set(HEADERS 
		../cyclops/cyclopsConfig.h
		../cyclops/Entity.h
		../cyclops/EffectNode.h
		../cyclops/AnimatedObject.h
		../cyclops/LineSet.h
		../cyclops/Light.h
		../cyclops/LightingLayer.h
		../cyclops/Material.h
		../cyclops/MaterialParser.h
		../cyclops/ModelLoader.h
		../cyclops/ModelGeometry.h
		../cyclops/RigidBody.h
		../cyclops/SceneLayer.h
		../cyclops/Shapes.h
		../cyclops/Text3D.h
		../cyclops/Skybox.h
		../cyclops/ShaderManager.h
		../cyclops/StaticObject.h
		../cyclops/SceneLoader.h
		../cyclops/SceneManager.h
		../cyclops/ShadowMap.h
		../cyclops/Uniforms.h)
		
###############################################################################
# Setup compile info
add_library( cyclops SHARED ${SRCS} ${HEADERS})

source_group(osg REGULAR_EXPRESSION osg/)		
target_link_libraries(cyclops
	omega 
	omegaToolkit
    omegaOsg
	${OSGBULLET_LIBS})

add_dependencies(cyclops bullet osgBullet)
