#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include "plug.h"

#ifdef HOT_RELOAD
#define PLUG(name, ...) name##_t *name;
PLUG_FUNCS
#undef PLUG

bool reload_plug(void);
#else
#define PLUG(name, ...) name##_t name;
PLUG_FUNCS
#undef PLUG

#define reload_plug(void) true
#endif // HOT_RELOAD

#endif // HOT_RELOAD_H