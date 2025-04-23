#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>
#include <dlfcn.h>

#include "hot_reload.h"

void *libplug = NULL;
static const char *plug_file_name = "libplug.so";

#define PLUG(name, ...) name##_t *name = NULL;
PLUG_FUNCS
#undef PLUG

bool reload_plug(void)
{

  if (libplug != NULL)
    dlclose(libplug);

  libplug = dlopen(plug_file_name, RTLD_NOW);
  if (libplug == NULL)
  {
    TraceLog(LOG_ERROR, "Could not load %s: %s\n", plug_file_name, dlerror());
    return false;
  }

#define PLUG(name, ...)                                                                \
  name = dlsym(libplug, #name);                                                        \
  if (name == NULL)                                                                    \
  {                                                                                    \
    printf("Could not find symbol %s for %s: %s\n", #name, plug_file_name, dlerror()); \
    return false;                                                                      \
  }
  PLUG_FUNCS
#undef PLUG

  return true;
}