#ifndef PLUG_H_
#define PLUG_H_

#define PLUG_FUNCS                     \
  PLUG(plug_init, void, void)          \
  PLUG(plug_update, void, void)        \
  PLUG(plug_pre_reload, void *, void)  \
  PLUG(plug_post_reload, void, void *) \
  PLUG(plug_clear, void, void)

#define PLUG(name, ret, ...) typedef ret(name##_t)(__VA_ARGS__);
PLUG_FUNCS
#undef PLUG

#endif // PLUG_H_