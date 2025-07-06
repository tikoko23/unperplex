#ifndef UNPERPLEX_DEF_H
#define UNPERPLEX_DEF_H

#if defined(UNPERPLEX_UI_HOT_RELOADING) && defined(__unix__)
#  include <dlfcn.h>
#  undef UNPERPLEX_UI_HOT_RELOADING
#  define UNPERPLEX_UI_HOT_RELOADING 1
#else
#  undef UNUNPERPLEX_UI_HOT_RELOADING
#  define UNPERPLEX_UI_HOT_RELOADING 0
#endif

#endif
