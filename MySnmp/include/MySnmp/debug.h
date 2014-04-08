#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef _WIN32

#ifdef _DEBUG
#define MYDEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define MYDEBUG_NEW
#endif // _DEBUG

#define _CRTDBG_MAP_ALLOC  

#include<stdlib.h>

#include<crtdbg.h>  


#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

#endif

#endif