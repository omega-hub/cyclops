if(WIN32)
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE FILE
        FILES
            ${BIN_DIR}/cyclops.dll
        )

elseif(APPLE)
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE FILE
        FILES
            ${BIN_DIR}/libcyclops.dylib
        )
endif()

pack_native_module(cyclops)

pack_dir(examples)
pack_dir(common)
pack_dir(test)

create_launcher(CyclopsExample_SpinCube examples/python/spincube)
create_launcher(CyclopsExample_Physics examples/python/physics)