# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (S8-53M2)

set(CMAKE_UNITY_BUILD OFF)

include_directories(
    ../../common/_VS_GUI/HAL
    ../../common/_VS_GUI/CMSIS
    ../../STM32Cube_FW_F4/Drivers/STM32F4xx_HAL_Driver/Inc
    ../../STM32Cube_FW_F4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ../../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc
    ../../STM32Cube_FW_F4/Middlewares/Third_Party/FatFs/src

    ../../VS/ThirdParty/wxWidgets/include 
    ../../generated/ThirdParty/lib/vc_dll/mswud

    ../../Device/src
    ../../Device/src/libs
    ../../Device/src/Hardware/HAL
    ../../Device/src/Hardware/FDrive
    ../..
)

link_directories(../../generated/ThirdParty/lib/vc_dll)

add_definitions(-DSTM32F429xx -DWXUSINGDLL -DGUI -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

    add_compile_options(
        /wd4061     # enumerator 'xxx' in switch of enum 'yyy' is not explicitly handled by a case label
        /wd4062
        /wd4201     # nonstandard extension used : nameless struct/union
        /wd4127     # conditional expression is constant
        /wd4211     # nonstandard extension used : redefined extern to static
        /wd4244 
        /wd4255     # 'function' : no function prototype given: converting '()' to '(void)'
        /wd4310     # cast truncates constant value
        /wd4355     # 'this' : used in base member initializer list
        /wd4464     # relative include path contains '..'
        /wd4505     # 'function': unreferenced function with internal linkage has been removed
        /wd4514     # 'function' : unreferenced inline function has been removed
        /wd4623     # 'derived class' : default constructor was implicitly defined as deleted because a base
                    # class default constructor is inaccessible or deleted
        /wd4625     # 'derived class' : copy constructor was implicitly defined as deleted because a base
                    # class copy constructor is inaccessible or deleted
        /wd4626     # 'xxx': assignment operator was implicitly defined as deleted
        /wd4668     # 'xxx' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
        /wd4710     # 'xxx': function not inlined
        /wd4774     # 'string' : format string expected in argument number is not a string literal
        /wd4820     # 'xxx': '4' bytes padding added after data member 'yyy'
        /wd4996     # 'xxx': This function or variable may be unsafe.
        /wd5026     # 'type': move assignment operator was implicitly defined as deleted
        /wd5027     # 'type': move assignment operator was implicitly defined as deleted
        /wd5039     # 'xxx': pointer or reference to potentially throwing function passed to extern C
                    # function under -EHc.
        /wd5045     # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5204
        /wd5219
        /wd5220     # a non-static data member with a volatile qualified type no longer implies that
                    # compiler generated copy/move constructors and copy/move assignment operators are not
                    # trivial
        /wd5264
        /wd26812    # the enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum'
    )

endif()

file(GLOB SRC                             ../../Device/src/main.cpp
                                          ../../Device/src/defines.*
                                          ../../Device/src/Device.*
                                          ../../Device/src/Globals.*
                                          ../../Device/src/Tables.*
                                          ../../Device/src/Log.*)
file(GLOB HAL                             ../../common/_VS_GUI/HAL/*.*)
file(GLOB DATA                            ../../Device/src/Data/*.*)
file(GLOB DISPLAY                         ../../Device/src/Display/*.*)
file(GLOB DISPLAY_FONT                    ../../Device/src/Display/font/*.*)
file(GLOB DISPLAY_SCREEN                  ../../Device/src/Display/Screen/*.*)
file(GLOB FPGA                            ../../Device/src/FPGA/*.*)
file(GLOB HARDWARE                        ../../Device/src/Hardware/*.*)
file(GLOB HARDWARE_FDRIVE                 ../../Device/src/Hardware/FDrive/FDrive.h
                                                     ./src/Hardware/FDrive/*.*)
file(GLOB HARDWARE_HAL                    ../../Device/src/Hardware/HAL/*.*)
file(GLOB HARDWARE_LAN                    ../../Device/src/Hardware/LAN/LAN.h
                                                     ./src/Hardware/LAN/*.*)
file(GLOB HARDWARE_VCP                    ../../Device/src/Hardware/VCP/VCP.h
                                                     ./src/Hardware/VCP/*.*)
file(GLOB MENU                            ../../Device/src/Menu/*.*)
file(GLOB MENU_PAGES                      ../../Device/src/Menu/Pages/*.*)
file(GLOB MENU_PAGES_MEMORY               ../../Device/src/Menu/Pages/PageMemory/*.*)
file(GLOB PANEL                           ../../Device/src/Panel/*.*)
file(GLOB SCPI                            ../../Device/src/SCPI/*.*)
file(GLOB SETTINGS                        ../../Device/src/Settings/*.*)
file(GLOB UTILS                           ../../Device/src/Utils/*.*)
file(GLOB UTILS_CONTAINERS                ../../Device/src/Utils/Containers/*.*)
file(GLOB UTILS_TEXT                      ../../Device/src/Utils/Text/*.*)

file(GLOB RESOURCES                       resources/*)

set_source_files_properties(
    resources/*
    PROPERTIES LANGUAGE RC
)

set_source_files_properties(
    ../../Device/src/FPGA/FPGA_old.cpp
    ../../Device/src/FPGA/CalibrationFPGA_old.cpp
    ../../Device/src/Hardware/HAL/HAL_IT.cpp
    ../../Device/src/Menu/Pages/PageHelp.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE
)

source_group(src                        FILES ${SRC})
source_group(HAL                        FILES ${HAL})
source_group(Data                       FILES ${DATA})
source_group(Display                    FILES ${DISPLAY})
source_group(Display/font               FILES ${DISPLAY_FONT})
source_group(Display/Screen             FILES ${DISPLAY_SCREEN})
source_group(FPGA                       FILES ${FPGA})
source_group(Hardware                   FILES ${HARDWARE})
source_group(Hardware/FDrive            FILES ${HARDWARE_FDRIVE})
source_group(Hardware/HAL               FILES ${HARDWARE_HAL})
source_group(Hardware/LAN               FILES ${HARDWARE_LAN})
source_group(Hardware/VCP               FILES ${HARDWARE_VCP})
source_group(Menu                       FILES ${MENU})
source_group(Menu/Pages                 FILES ${MENU_PAGES})
source_group(Menu/Pages/PageMemory      FILES ${MENU_PAGES_MEMORY})
source_group(Panel                      FILES ${PANEL})
source_group(SCPI                       FILES ${SCPI})
source_group(Settings                   FILES ${SETTINGS})
source_group(Utils                      FILES ${UTILS})
source_group(Utils/Containers           FILES ${UTILS_CONTAINERS})
source_group(Utils/Text                 FILES ${UTILS_TEXT})
source_group(Resources                  FILES ${RESOURCES})

add_executable(
    ${PROJECT_NAME}
    ${SRC}
    ${HAL}
    ${DATA}
    ${DISPLAY}
    ${DISPLAY_FONT}
    ${DISPLAY_SCREEN}
    ${FPGA}
    ${HARDWARE}
    ${HARDWARE_FDRIVE}
    ${HARDWARE_HAL}
    ${HARDWARE_LAN}
    ${HARDWARE_VCP}
    ${MENU}
    ${MENU_PAGES}
    ${MENU_PAGES_MEMORY}
    ${PANEL}
    ${SCPI}
    ${SETTINGS}
    ${UTILS}
    ${UTILS_CONTAINERS}
    ${UTILS_TEXT}
    ${RESOURCES}
)

target_link_libraries(${PROJECT_NAME} wxbase33ud wxmsw33ud_core)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxbase33ud_vc_custom.dll $(OutputPath) /Y)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxmsw33ud_core_vc_custom.dll $(OutputPath) /Y)
