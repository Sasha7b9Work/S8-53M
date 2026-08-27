# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Loader)

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

add_definitions(-DSTM32F429xx -DUSE_HAL_DRIVER -DLOADER)

if(WIN32)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    
    add_compile_options(/wd4201)    # nonstandard extension used : nameless struct/union
    add_compile_options(/wd4505)    # 'function': unreferenced function with internal linkage has been removed
    add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
    add_compile_options(/wd4623)    # 'derived class' : default constructor was implicitly defined as deleted because a base
                                    # class default constructor is inaccessible or deleted
    add_compile_options(/wd4626)    # 'derived class' : assignment operator was implicitly defined as deleted because a base
                                    # class assignment operator is inaccessible or deleted
    add_compile_options(/wd4774)    # 'function' : format string expected in argument 3 is not a string literal
    add_compile_options(/wd4820)    # 'bytes' bytes padding added after construct 'member_name'
    add_compile_options(/wd5027)    # 'derived class' : move assignment operator was implicitly defined as deleted
    add_compile_options(/wd5039)    # 'function': pointer or reference to potentially throwing function passed to 'extern
                                    # "C"' function under -EHc. Undefined behavior may occur if this function throws an
                                    # exception
    add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    add_compile_options(/wd5219)    # implicit conversion from 'type1' to 'type2', possible loss of data
    add_compile_options(/wd5220)    #
    add_compile_options(/wd26812)

else()

    add_definitions(-Wno-builtin-declaration-mismatch
                    -Wno-write-strings
    )

endif()

file(GLOB VS_KEIL_ARMCLANG ../common/vs_keil/ARMCLANG/*)
file(GLOB VS_KEIL_CMSIS    ../common/vs_keil/CMSIS/core_cm4.h)
file(GLOB VS_KEIL_FATFS    ../common/vs_keil/FatFS/*)

file(GLOB LIBS_FATFS       ../common/STM32Cube_F4/Middlewares/Third_Party/FatFS/src/ff_gen_drv.*
                           ../common/STM32Cube_F4/Middlewares/Third_Party/FatFS/src/ff.*
                           ../common/STM32Cube_F4/Middlewares/Third_Party/FatFS/src/diskio.*
                           ../common/STM32Cube_F4/Middlewares/Third_Party/FatFS/src/option/ccsbcs.*
                           ../common/STM32Cube_F4/Middlewares/Third_Party/FatFS/src/option/syscall.*)

file(GLOB LIBS_CMSIS       ../common/STM32Cube_F4/Drivers/CMSIS/Device/ST/STM32F2xx/Include/*
                           ../common/STM32Cube_F4/Drivers/CMSIS/Include/*)

file(GLOB LIBS_HAL         ../common/STM32Cube_F2/Drivers/STM32F4xx_HAL_Driver/Inc/*.h
                           ../common/STM32Cube_F2/Drivers/STM32F4xx_HAL_Driver/Src/*.c)

file(GLOB LIBS_USBH        ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_core.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_ctlreq.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_ioreq.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_pipes.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc/usbh_msc.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc/usbh_msc_bot.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc/usbh_msc_scsi.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.*
                           ../common/STM32Cube_F4/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.*)

file(GLOB HARDWARE               src/Hardware/*.*)
file(GLOB HARDWARE_CMSIS         src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_FATFS         src/Hardware/FatFS/*.*)
file(GLOB HARDWARE_HAL           src/Hardware/HAL/*.*
                           ../common/Hardware/HAL/HAL_PINS_.*
                           ../common/Hardware/HAL/HAL_.*
                           ../common/Hardware/HAL/HAL_DAC1_.cpp
                           ../common/Hardware/HAL/HAL_DAC2_.cpp
                           ../common/Hardware/HAL/HAL_LTDC_.cpp
                           ../common/Hardware/HAL/HAL_ROM_.cpp
                           ../common/Hardware/HAL/HAL_TIM2_.cpp
                           ../common/Hardware/HAL/HAL_TIM6_.cpp
                           ../common/Hardware/HAL/HAL_TIM7_.cpp)
file(GLOB HARDWARE_FDRIVE        src/Hardware/FDrive/*.*)
file(GLOB SRC              ../common/defines_.h
                           ../common/Log_.*
                                 src/*.*)
file(GLOB DISPLAY                src/Display/*.*
                           ../common/Display/*.*)
file(GLOB DISPLAY_PAINTER  ../common/Display/Painter/Animated_.*
                           ../common/Display/Painter/Primitives_.*
                           ../common/Display/Painter/Text_.*)
file(GLOB DISPLAY_FONT     ../common/Display/Font/*.*)
file(GLOB PANEL                  src/Panel/*.*
                           ../common/Keyboard/*.*)
file(GLOB SETTINGS               src/Settings/*.*
                           ../common/Settings/cSettingsTypes.h)
file(GLOB UTILS                  src/Utils/*.*
                           ../common/Utils/*.*)
file(GLOB UTILS_CONTAINERS ../common/Utils/Containers/Buffer_.*)

set_source_files_properties(${LIBS_FATFS} ${LIBS_HAL} ${LIBS_USBH}
                            src/Hardware/HAL/HAL_ROM_stm.cpp
                            PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(libs/vs_keil/ARMCLANG FILES ${VS_KEIL_ARMCLANG})
source_group(libs/vs_keil/CMSIS    FILES ${VS_KEIL_CMSIS})
source_group(libs/vs_keil/FatFS    FILES ${VS_KEIL_FATFS})
source_group(libs/CMSIS            FILES ${LIBS_CMSIS})
source_group(libs/FatFS            FILES ${LIBS_FATFS})
source_group(libs/HAL              FILES ${LIBS_HAL})
source_group(libs/USBH             FILES ${LIBS_USBH})
source_group(Hardware              FILES ${HARDWARE})
source_group(Hardware/CMSIS        FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL          FILES ${HARDWARE_HAL})
source_group(Hardware/FDrive       FILES ${HARDWARE_FDRIVE})
source_group(src                   FILES ${SRC} ../co)
source_group(Display               FILES ${DISPLAY})
source_group(Display/Font          FILES ${DISPLAY_FONT})
source_group(Display/Painter       FILES ${DISPLAY_PAINTER})
source_group(Panel                 FILES ${PANEL})
source_group(Settings              FILES ${SETTINGS})
source_group(Utils                 FILES ${UTILS})
source_group(Utils/Containers      FILES ${UTILS_CONTAINERS})

                   
add_library(${PROJECT_NAME}
    ${VS_KEIL_CMSIS} ${VS_KEIL_ARMCLANG} ${VS_KEIL_FATFS} ${LIBS_CMSIS}
    ${LIBS_FATFS} ${LIBS_HAL} ${LIBS_USBH}
    ${HARDWARE} ${HARDWARE_CMSIS} ${HARDWARE_HAL} ${HARDWARE_FDRIVE}
    ${SRC} ${DISPLAY} ${DISPLAY_FONT} ${DISPLAY_PAINTER} ${PANEL} ${SETTINGS}
    ${UTILS} ${UTILS_CONTAINERS}
)
