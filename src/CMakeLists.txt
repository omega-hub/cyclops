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
        AnimatedObject.cpp
        Compositor.cpp
        CompositorXML.cpp
        CompositingLayer.cpp
        EffectNode.cpp
        Entity.cpp
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
        ShadowMapGenerator.cpp
        StaticObject.cpp
        Text3D.cpp
        Uniforms.cpp
        cyclopsPythonApi.cpp)
    
set(HEADERS 
        ../cyclops/cyclopsConfig.h
        ../cyclops/AnimatedObject.h
        ../cyclops/Compositor.h
        ../cyclops/CompositingLayer.h
        ../cyclops/Entity.h
        ../cyclops/EffectNode.h
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
        ../cyclops/ShadowMap.h
        ../cyclops/ShadowMapGenerator.h
        ../cyclops/StaticObject.h
        ../cyclops/SceneLoader.h
        ../cyclops/SceneManager.h
        ../cyclops/ShadowMap.h
        ../cyclops/Uniforms.h)
        
###############################################################################
# Setup compile info
add_library( cyclops SHARED ${SRCS} ${HEADERS})

source_group(cyclops REGULAR_EXPRESSION cyclops/)		
target_link_libraries(cyclops
    omega 
    omegaToolkit
    omegaOsg
    ${OSG_LIBS}
    ${OSGBULLET_LIBS})

if(MODULES_omegaOsgEarth)
    target_link_libraries(cyclops ${OSGEARTH_LIBS})
endif()

add_dependencies(cyclops omegaOsg)

###############################################################################
# Create the cyclops loader python module, to manually load cyclops
# See 'reduce orun memory usage omegalib issue: https://github.com/uic-evl/omegalib/issues/109
add_library(cyclops_loader SHARED cyclops_loader.cpp)
target_link_libraries(cyclops_loader cyclops)
# import module and lib will be called cyclops.dll/cyclops.pyd and 
# libcyclops.so/cyclops.so on windows and linux/OSX respectively
# we add the module_ prefix to the import library to avoid conflicts with the main
# cyclops.lib import library in windows.
# NOTE: we would like to completely disable import library generation but I did
# not find a way to do it.
set_target_properties(cyclops_loader PROPERTIES OUTPUT_NAME "cyclops" IMPORT_PREFIX "module_" PREFIX "")
if(WIN32)
    set_target_properties(cyclops_loader PROPERTIES FOLDER modules SUFFIX ".pyd" PDB_NAME "cyclops_loader")
endif()
