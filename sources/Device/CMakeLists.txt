# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

set(CMAKE_UNITY_BUILD OFF)

include_directories(
    ../common/_VS_KEIL/ARMCLANG
    ../common/_VS_KEIL/CMSIS
    ../common/_VS_KEIL/FatFS
    ../STM32Cube_FW_F4/Drivers/STM32F4xx_HAL_Driver/Inc
    ../STM32Cube_FW_F4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc
    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ../STM32Cube_FW_F4/Middlewares/Third_Party/FatFs/src
    ../STM32Cube_FW_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc
    ../STM32Cube_FW_F4/Middlewares/Third_Party/LwIP/src/include
    ../STM32Cube_FW_F4/Middlewares/Third_Party/LwIP/system
    src
    src/Hardware/HAL
    src/Hardware/FDrive
   ..
)

add_definitions(-DSTM32F429xx -DUSE_HAL_DRIVER -DDEVICE -DUSE_FULL_LL_DRIVER)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

    add_compile_options(
        /wd4061     # enumerator 'xxx' in switch of enum 'enum' is not explicitly handled by a case label
        /wd4062
        /wd4201
        /wd4211
        /wd4244
        /wd4302     # 'type cast': truncation from 'x **' to 'x'
        /wd4310     # cast truncates constant value
        /wd4514     # 'function' : unreferenced inline function has been removed
        /wd4623     # 'derived class' : default constructor was implicitly defined as deleted because a base
                    # class default constructor is inaccessible or deleted
        /wd4626     # 'derived class' : assignment operator was implicitly defined as deleted because a base
                    # class assignment operator is inaccessible or deleted
        /wd4668     # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
        /wd4774     # 'function' : format string expected in argument 3 is not a string literal
        /wd4820     # 'bytes' bytes padding added after construct 'member_name'
        /wd5027     # 'derived class' : move assignment operator was implicitly defined as deleted
        /wd5039     # 'function': pointer or reference to potentially throwing function passed to
                    # 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws
                    # an exception
        /wd5045     # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5219     # implicit conversion from 'type1' to 'type2', possible loss of data
        /wd5220     # '<unnamed-tag>::SR': a non-static data member with a volatile qualified type no longer
                    # implies that compiler generated copy/move constructors and copy/move assignment
                    # operators are not trivia
        /wd26812
        /wd33010
    )

else()


    add_definitions(-Wno-builtin-declaration-mismatch
                    -Wno-write-strings
                    -Wno-overflow
                    -Wno-int-to-pointer-cast
    )

endif()

file(GLOB SRC                      src/*.cpp src/*.h)
file(GLOB DATA                     src/Data/*.*)
file(GLOB DISPLAY                  src/Display/*.*)
file(GLOB DISPLAY_FONT             src/Display/Font/*.*)
file(GLOB DISPLAY_SCREEN           src/Display/Screen/*.*)
file(GLOB FPGA                     src/FPGA/*.*)
file(GLOB HARDWARE                 src/Hardware/*.*)
file(GLOB HARDWARE_FDRIVE          src/Hardware/FDrive/*.*)
file(GLOB HARDWARE_HAL             src/Hardware/HAL/*.*)
file(GLOB HARDWARE_LAN             src/Hardware/LAN/*.*)
file(GLOB HARDWARE_VCP             src/Hardware/VCP/*.*)
file(GLOB MENU                     src/Menu/*.*)
file(GLOB MENU_PAGES               src/Menu/Pages/*.*)
file(GLOB MENU_PAGES_MEMORY        src/Menu/Pages/PageMemory/*.*)
file(GLOB PANEL                    src/Panel/*.*
                             ../common/Panel/*.*)
file(GLOB SETTINGS                 src/Settings/*.*)
file(GLOB UTILS                    src/Utils/*.*)
file(GLOB UTILS_CONTAINERS         src/Utils/Containers/*.*)
file(GLOB UTILS_TEXT               src/Utils/Text/*.*)
file(GLOB SCPI                     src/SCPI/*.*)
file(GLOB LIBS_HAL                 ../STM32Cube_FW_F4/Drivers/STM32F4xx_HAL_Driver/Src/*.*)

source_group(src                   FILES ${SRC})
source_group(Data                  FILES ${DATA})
source_group(Display               FILES ${DISPLAY})
source_group(Display/Font          FILES ${DISPLAY_FONT})
source_group(Display/Screen        FILES ${DISPLAY_SCREEN})
source_group(FPGA                  FILES ${FPGA})
source_group(Hardware              FILES ${HARDWARE})
source_group(Hardware/FDrive       FILES ${HARDWARE_FDRIVE})
source_group(Hardware/HAL          FILES ${HARDWARE_HAL})
source_group(Hardware/LAN          FILES ${HARDWARE_LAN})
source_group(Hardware/VCP          FILES ${HARDWARE_VCP})
source_group(Menu                  FILES ${MENU})
source_group(Menu/Pages            FILES ${MENU_PAGES})
source_group(Menu/Pages/PageMemory FILES ${MENU_PAGES_MEMORY})
source_group(Panel                 FILES ${PANEL})
source_group(Settings              FILES ${SETTINGS})
source_group(Utils                 FILES ${UTILS})
source_group(Utils/Containers      FILES ${UTILS_CONTAINERS})
source_group(Utils/Text            FILES ${UTILS_TEXT})
source_group(SCPI                  FILES ${SCPI})
source_group(libs/HAL              FILES $${LIBS_HAL})

set_source_files_properties(
    /Hardware/LAN/ethernetif.cpp
    src/Menu/Pages/PageHelp.cpp
    src/Panel/PanelFunctions.cpp
    ${LIBS_HAL}
    PROPERTIES HEADER_FILE_ONLY TRUE
)

add_library(${PROJECT_NAME}
    ${SRC} ${DISPLAY}
    ${DISPLAY_FONT} ${DISPLAY_SCREEN}
    ${HARDWARE_LAN}
    ${DATA}
    ${HARDWARE_FDRIVE} ${FPGA} ${HARDWARE}
    ${HARDWARE_HAL} ${HARDWARE_VCP}
    ${MENU} ${MENU_PAGES} ${MENU_PAGES_MEMORY}
    ${PANEL} ${SETTINGS}
    ${UTILS} ${UTILS_CONTAINERS} ${UTILS_TEXT}
    ${SCPI}
    ${LIBS_HAL}
)
