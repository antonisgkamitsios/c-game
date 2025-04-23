#ifndef NOBUILD_H_
#define NOBUILD_H_

#include <assert.h>

#define da_append(da, item)                                                   \
  do                                                                          \
  {                                                                           \
    if ((da)->length >= (da)->capacity)                                       \
    {                                                                         \
      (da)->capacity = (da)->capacity == 0 ? 1 : (da)->capacity * 2;          \
      (da)->data = realloc((da)->data, (da)->capacity * sizeof(*(da)->data)); \
      assert((da)->data != NULL && "Bro where is your RAM");                  \
    }                                                                         \
    (da)->data[(da)->length++] = item;                                        \
  } while (0)

#define da_append_many(da, items, size)                                       \
  do                                                                          \
  {                                                                           \
    if ((da)->length + size > (da)->capacity)                                 \
    {                                                                         \
      if ((da)->capacity == 0)                                                \
      {                                                                       \
        (da)->capacity = 1;                                                   \
      }                                                                       \
      while ((da)->length + size > (da)->capacity)                            \
      {                                                                       \
        (da)->capacity *= 2;                                                  \
      }                                                                       \
      (da)->data = realloc((da)->data, (da)->capacity * sizeof(*(da)->data)); \
      assert((da)->data != NULL && "Bro where is your RAM");                  \
    }                                                                         \
    memcpy((da)->data + (da)->length, items, size * sizeof(*(da)->data));     \
    (da)->length += size;                                                     \
  } while (0)

#endif // NOBUILD_H