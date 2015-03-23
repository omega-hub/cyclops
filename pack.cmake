if(WIN32)
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE FILE
        FILES
            ${BIN_DIR}/cyclops.dll
            ${BIN_DIR}/cyclops.pyd
        )
    
    file(INSTALL DESTINATION ${PACKAGE_DIR}
        TYPE FILE
        FILES
            ${SOURCE_DIR}/modules/cyclops/physics.bat
            ${SOURCE_DIR}/modules/cyclops/spincube.bat
        )
endif()

file(INSTALL DESTINATION ${PACKAGE_DIR}/modules/cyclops
    TYPE DIRECTORY
    FILES
        ${SOURCE_DIR}/modules/cyclops/common
        ${SOURCE_DIR}/modules/cyclops/test
    )

file(INSTALL DESTINATION ${PACKAGE_DIR}/examples/cyclops
    TYPE DIRECTORY
    FILES
        ${SOURCE_DIR}/modules/cyclops/examples/python/
    )
    