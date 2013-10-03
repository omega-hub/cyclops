add_definitions( -D_CRT_SECURE_NO_WARNINGS -DCY_EXPORTING)
set(SRCS 
		Entity.cpp
		EffectNode.cpp
		AnimatedObject.cpp
		LineSet.cpp
		Light.cpp
		Material.cpp
		MaterialParser.cpp
		ModelLoader.cpp
		ModelGeometry.cpp
		Shapes.cpp
		Skybox.cpp
		SceneLoader.cpp
		SceneManager.cpp
		StaticObject.cpp
		Text3D.cpp
		Uniforms.cpp
		cyclopsPythonApi.cpp)
    
set(HEADERS 
		../cyclops/Entity.h
		../cyclops/EffectNode.h
		../cyclops/AnimatedObject.h
		../cyclops/LineSet.h
		../cyclops/Light.h
		../cyclops/Material.h
		../cyclops/MaterialParser.h
		../cyclops/ModelLoader.h
		../cyclops/ModelGeometry.h
		../cyclops/cyclopsConfig.h
		../cyclops/Shapes.h
		../cyclops/Text3D.h
		../cyclops/Skybox.h
		../cyclops/StaticObject.h
		../cyclops/SceneLoader.h
		../cyclops/SceneManager.h
		../cyclops/Uniforms.h)
		
###############################################################################
# Setup compile info
add_library( cyclops SHARED ${SRCS} ${HEADERS})

source_group(osg REGULAR_EXPRESSION osg/)		
target_link_libraries(cyclops
	omega 
	omegaToolkit
    omegaOsg)
