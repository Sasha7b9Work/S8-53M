# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Panel)

include_directories(
    src
    src/Hardware/HAL
    ..
    ../common/_VS_KEIL/ARMCLANG
    ../common/_VS_KEIL/CMSIS
    ../STM32Cube_FW_L0/Drivers/STM32L0xx_HAL_Driver/Inc
    ../STM32Cube_FW_L0/Drivers/CMSIS/Device/ST/STM32L0xx/Include
    ../STM32Cube_FW_L0/Drivers/CMSIS/Include
if(WIN32)
else()
    /usr/include/x86_64-linux-gnu
endif()
)

add_definitions(-DSTM32L063xx -DUSE_HAL_DRIVER)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    
    add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
    add_compile_options(/wd4623)    # 'derived class' : default constructor was implicitly defined as deleted because
                                    # a base class default constructor is inaccessible or deleted
    add_compile_options(/wd4626)    # 'derived class' : assignment operator was implicitly defined as deleted because
                                    # a base class assignment operator is inaccessible or deleted
    add_compile_options(/wd4774)    # 'function' : format string expected in argument 3 is not a string literal
    add_compile_options(/wd4820)    # 'bytes' bytes padding added after construct 'member_name'
    add_compile_options(/wd5027)    # 'derived class' : move assignment operator was implicitly defined as deleted
    add_compile_options(/wd5039)    # 'function': pointer or reference to potentially throwing function passed to
                                    # 'extern "C"' function under -EHc. Undefined behavior may occur if this function
                                    # throws an exception
    add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch
                                    # specified
    add_compile_options(/wd5219)    # implicit conversion from 'type1' to 'type2', possible loss of data
    add_compile_options(/wd5220)
    add_compile_options(/wd26812)   # The enum type type-name is unscoped. Prefer 'enum class' over 'enum' (Enum.3)
else()

endif()

file(GLOB VS_KEIL_ARMCLANG ../common/vs_keil/ARMCLANG/*)
file(GLOB VS_KEIL_CMSIS    ../common/vs_keil/CMSIS/core_cm0plus.h)

file(GLOB LIBS_CMSIS       ../common/STM32Cube_L0/Drivers/CMSIS/Device/ST/STM32L0xx/Include/*
                           ../common/STM32Cube_L0/Drivers/CMSIS/Include/*)

file(GLOB LIBS_HAL         ../common/STM32Cube_L0/Drivers/STM32L0xx_HAL_Driver/Inc/*.h
                           ../common/STM32Cube_L0/Drivers/STM32L0xx_HAL_Driver/Src/*.c)

file(GLOB SRC                    src/*.h
                                 src/*.cpp
                           ../common/Panel/*.*)

set_source_files_properties(${LIBS_HAL} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(libs/vs_keil/ARMCLANG FILES ${VS_KEIL_ARMCLANG})
source_group(libs/vs_keil/CMSIS    FILES ${VS_KEIL_CMSIS})
source_group(src                   FILES ${SRC})

add_library(${PROJECT_NAME}
    ${VS_KEIL_ARMCLANG}
    ${VS_KEIL_CMSIS}
    ${SRC}
)
