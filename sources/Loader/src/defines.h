// (c) Aleksandr Shevchenko e-mail : Sasha7b9@gmail.com
#pragma once
#include "defines_.h"


#ifndef WIN32
#pragma clang diagnostic ignored "-Winvalid-source-encoding"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wwritable-strings"
#pragma clang diagnostic ignored "-Wunused-function"
#endif


#ifndef __cplusplus
	typedef unsigned char bool;
	#define false   0
	#define true    (!false)
#endif


#ifdef WIN32
	#define __STATIC_INLINE inline
#else
	#define nullptr 0 //-V1059
#endif


#define LANG_RU true

#define WR_START 0
#define WR_UPR   0
#define WR_RAZV  0


#include "Log.h"
