MINIFMP4
========

This is a small, single-header library for writing fragmented
MP4/ISO Base Media File Format files. It's designed to be
fairly simple to use, and not necessarily know all the inner
workings of how MP4 files work.

Usage
-----

In exactly one source file, define MINIFMP4_IMPLEMENTATION before
including the header:

```c
#define MINIFMP4_IMPLEMENTATION
#include "minifmp4.h"
```

The general workflow is:

1. Allocate and initialize 1 fmp4_mux object.
2. Configure your file-level options on the fmp4_mux object,
   things like the MP4 brand, minor version, and compatible
   brands.
3. Allocate and initialize 1+ fmp4_track objects and add them to the fmp4_mux
   object.
4. Configure your track-level options on your fmp4_track
   objects, things like the type of media, default flags,
   decoder-specific information, and so on.
5. Write out your initialization segment.
6. Add samples to the fmp4_track objects.
7. Write out media segments.
8. Close and free your objects.

The library allows a variety of memory-management options. You
can use the default malloc/realloc/free allocators, or supply a
custom set. You can allocate the needed structs on your own
and initialize them, or have the library allocate them for you.

All structs have getter/setter functions, a goal is to be able to use
this library without accessing a single struct field. Some fields
require using the getter/setter functions. Generally-speaking,
if a field's type is fmp4_membuf, you'll need to use a getter/setter function.

My main focus is on audio, so that's all the library supports
right now. It has features like, the ability to add loudness
metadata and send EventMessage objects.

Take a look at the demos folder for a few examples.

License
------

BSD Zero Clause (see the `LICENSE` file).
