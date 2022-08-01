# Composable Category Declarations

## Introduction

[_Working demo here_](#working-demo)

Proposing a practical, easy and clean way of composing perfetto categories
declared at multiple places/files across multiple independent third-party
libraries.

A TU (translation unit) of main-app or a third-party library
could be including headers of multiple other third-party libraries, and hence
they could end up including the multiple perfetto-category declaration
files. With this proposed model, all of it will works SEAMLESSLY, requiring NO
restriction at all on how headers should be included.

With this model, independence and individuality of third-party libraries is NOT
impacted.

In addition, with this new method, we can get rid of:

1). Multiple TrackEventDataSource (controlled via namespace).

2). Mapping multiple TrackEventDataSource to a common TLS and a common
    trace-writer.


## Background Context

As we know that we use compile time string interning for perfetto categories in
order to avoid runtime overhead of dealing with string. All the categories used
in `TRACE_EVENT` like macros, needs to be declared in advance at a common
place/file. This file is being included (directly or transitively) in all the
TUs (translation units) where `TRACE_EVENT` like macros are used.

We declare a list of strings (category names), and when one of those strings
are referred in any `TRACE_EVENT` like macro, we replace (at compile time) that
referred string by `category_id`. Generally `category_id` is just the index of
that particular `category_name` among the list of all category names.

As a result, we only deal with `category_id` integer at runtime instead of
`category_name` string.

## The problem

As discussed above, the entire application needs to refer to categories
declared at one place, to compute the category_id (index in the list)
correctly. We cannot easily fragment the category declaration list, because if
we do that, we cannot compute the category_id (index) easily at compile time.

because of that, we cannot easily use perfetto-instrumented third-party
libraries seamlessly, because those third-party libraries would be
declaring their own set of `category_names` in one of their files, and the
category_names used in their trace point would be resolving to
category_id (index) in their own category list. As a result two different
category_name would end up resolving to same category_id, causing a big
problem because the perfetto session produces a single trace file, covering the
execution of the entire application (in which that third-party library is
also imported). Hence we cannot correctly map those `category_id` to the
correct category names while processing the trace offline.

To solve this problem, In past we introduced namespace dependent category
declaration and different instantiation of templated class TrackEventDataSource
in different namespaces. With that, `category_names` are resolved w.r.t. to an
specific instantiation of TrackEventDataSource, and almost everything is kept
separated for different instantiation of TrackEventDataSource except a few
shared things like thread local storage (TLS) and TLS-trace-witters.
Different instantiation of TrackEventDataSource needs to be registered
differently in each third-party library before tracing session starts.

These solutions works to an extent but cannot support a translation unit which
transitively `#includes` two different category declaration files, which is
very important when using headers of third-party libraries, specially when those
third-party libraries use `TRACE_EVENT` macros in their header.

One of the recent solution proposes to looking up TrackEvent with relative
instead of absolute namespace paths. This solution improves the flexibility
of `#including` multiple category declaration files (via headers of third-party
libraries). However it still require manual intervention in namespace-resolution
at times when `TRACE_EVENT` like macros are not used in correct namespaces.

Because of this, while `#include`'ing a new third-party header, we might end up
in a situation where 10s of `TRACE_EVENT` macros began to fail requiring a
manual namespace-resolution (eg: `namespace perfetto = v8::perfetto;`).
Which is already too much pain, but even worse - what if we are not allowed to
change the code in that file, because the failing `TRACE_EVENT` macros are in
an existing third_party library, after including a new third_party before the
existing one.

Before:

```C++
#include "third_party_libB.h"
```

After:

```C++
#include "third_party_libA.h"  // newly added.
#include "third_party_libB.h"
```

but now we are seeing a lot of compilation errors (`ambiguous name resolution`)
coming from `third_party_libB.h` and we cannot even change the
`third_party_libB.h` file.


## The solution:

```C++
PERFETTO_DECLARE_CATEGORIES(
  PERFETTO_CATEGORY_DECLARATION_SLOT_NUMBER(2),
  perfetto::Category{"Cat1"},
  perfetto::Category{"Cat2"},
  perfetto::Category{"Cat3"},
);
```

Each category declaration file (aka. each third-party library) should use a
different `slot_number`. A third-party library can ask its clients to supply
this number as build-time variable
(such as `libA_perfetto_category_slot_number`), and the top level application
can set this variable in their BUILD script in a way, a different value is
provided to each of its third-party library.

**That's all**. That's the ONLY thing one need to setup in the build env once in
an eternity and forget about it. There is no need to revisit it ever again.


## How does it works internally ?

we are expending `PERFETTO_DECLARE_CATEGORIES` to:

```C++
template <> struct CategoryDeclarationSlot<slot_number> {
  constexpr static perfetto::Category kCategories[] = {__VA_ARGS__};
};
```

`slot_number` can be one of {0, 1, 2, ..., 15}.

At any point of time in a translation unit, `CategoryDeclarationSlot` could be
instantiated for some of the slot_numbers.

For example: In a translation unit, `CategoryDeclarationSlot<2>` and
`CategoryDeclarationSlot<5>` could be instantiated and rest of the slots may
not. We can just search over the range `{0, 1, ...15}` using C++ SFINAE to see
if categories are declared with that slot.
If declared, we can search for `category_name` in each of the
`CategoryDeclarationSlot`. If we find a match in `slot_number` at `index`, the
returned `category_id` would be `((index << 4) | slot_number)`.

Note: If `slot_number` `3` is not used for category declaration in a TU, the
      `sizeof(CategoryDeclarationSlot<3>)` will fail because template is not
      defined for `3` as of yet. Hence we can use it in SFINAE.

Thus, we got a unique `category_id` for a `category_name` at compile time.
This `category_id` is unique across all categories in all category declaration
slots, because of the underlying assumption that every two category
declarations will use a different `slot_number`.

## `category_id` to Category calculation at Runtime

In perfetto we need to make runtime decisions based on runtime state associated
with a category (For example: `Is category enabled for a session`).

We can continue to access the runtime state, very much similar to how it was
accessed before, with just one extra array lookup.

Before:

```C++
registered_categories[category_id]
```

After:

```C++
registered_slots[category_id & 0xF].categories[category_id >> 4]
```

Find more details in the implementation of `GetCategoryAtRunTime` and
`GetCategoryAtCompileTime` methods.

Note: The array `registered_slots` have external linkage, whose values
are populated one by one during static initialization step of different
translation units where `PERFETTO_DEFINE_CATEGORIES` are placed.


## Why we should adopt this solution

1). Setup ONLY ONCE in an eternity, and enjoy absolute freedom in including
    headers of third-party libraries. The trace macros will NEVER fail to
    compile because of a newly added header. No manual intervention required
    ever. A TU will continue to work if any number of category declaring
    headers are included in it. A category_name will be correctly
    resolved to where it matches with a declared category string of one of
    the `included` category declaration file. i.e. it will be automatically
    resolved to correct category declaration file as well.

2). Get rid of the complexity involved in maintaining different instantiation
    of TrackEventDataSource, and special treatment of `TrackEventDataSource`
    data source.

3). Get rid of mapping different instance of `TrackEventDataSource` into a
    common trace writer and common TLS. Now when we are adding support for
    startup tracing, we will need to introduce the same complexity in startup
    tracing as well.

4). No need to enforce each of the `perfetto-instrumented` third-party library
    to register their data source correctly, before the first tracing session
    starts, i.e. even before startup trace starts. What if a third-party library
    is loaded sometime later (possibly via `dlopen`) after the startup tracing
    is started, we need to register it later. Since we don't have support for
    adopting startup trace automatically even in late registeration, we will
    never be able to collect startup trace in such third-party libraries,
    unless we support such unnecessary complexity in startup trace to adopt
    the late arriving data sources. Which we don't indent to support that in
    startup trace.


## Working Demo

[This Repo](https://github.com/tinytrashbin/composable_category_declarations)

1). Go to `main_app` directory and run `./build.sh` (Tested on macOS).

2). Go to `third_party_libC` directory and run `./build.sh`.

3). Go to `third_party_libB` directory and run `./build.sh`.

4). Go to `third_party_libA` directory and run `./build.sh`.


Each third-party lib declare their own category names. These depndencies are
highly mixed.


![Picture](https://i.imgur.com/0gSCEkz.png)


Sample output of `libC_test_main`:

```
Trace = 
packet[0]: {Mapping: category_id 1 => 'libA_Cat1'}
packet[1]: {Mapping: category_id 17 => 'libA_Cat2'}
packet[2]: {Mapping: category_id 33 => 'libA_Cat3'}
packet[3]: {Mapping: category_id 2 => 'libB_Cat1'}
packet[4]: {Mapping: category_id 18 => 'libB_Cat2'}
packet[5]: {Mapping: category_id 34 => 'libB_Cat3'}
packet[6]: {Mapping: category_id 3 => 'libC_Cat1'}
packet[7]: {Mapping: category_id 19 => 'libC_Cat2'}
packet[8]: {Mapping: category_id 35 => 'libC_Cat3'}
packet[9]: {Event: category_id=19, event_name=LibC_Func1}
packet[10]: {Event: category_id=3, event_name=LibC_Func2}
packet[11]: {Event: category_id=2, event_name=LibB_Func2}
packet[12]: {Event: category_id=1, event_name=LibA_Func4}
packet[13]: {Event: category_id=1, event_name=LibA_Func3}
packet[14]: {Event: category_id=19, event_name=LibC_Func3}
packet[15]: {Event: category_id=35, event_name=LibC_Func4}
packet[16]: {Event: category_id=19, event_name=LibC_Func3}
packet[17]: {Event: category_id=18, event_name=LibB_Being_Called_From_LibC}
packet[18]: {Event: category_id=2, event_name=LibB_Func3}
packet[19]: {Event: category_id=33, event_name=LibA_Being_Called_From_LibC}
packet[20]: {Event: category_id=1, event_name=LibA_Func3}
packet[21]: {Event: category_id=3, event_name=LibC_Func2}
packet[22]: {Event: category_id=2, event_name=LibB_Func2}
packet[23]: {Event: category_id=1, event_name=LibA_Func4}
packet[24]: {Event: category_id=1, event_name=LibA_Func3}
---------
After TraceProcessor = 
packet[0]: {Mapping: category_id 1 => 'libA_Cat1'}
packet[1]: {Mapping: category_id 17 => 'libA_Cat2'}
packet[2]: {Mapping: category_id 33 => 'libA_Cat3'}
packet[3]: {Mapping: category_id 2 => 'libB_Cat1'}
packet[4]: {Mapping: category_id 18 => 'libB_Cat2'}
packet[5]: {Mapping: category_id 34 => 'libB_Cat3'}
packet[6]: {Mapping: category_id 3 => 'libC_Cat1'}
packet[7]: {Mapping: category_id 19 => 'libC_Cat2'}
packet[8]: {Mapping: category_id 35 => 'libC_Cat3'}
packet[9]: {Event: category_name = libC_Cat2, category_id=19, event_name=LibC_Func1}
packet[10]: {Event: category_name = libC_Cat1, category_id=3, event_name=LibC_Func2}
packet[11]: {Event: category_name = libB_Cat1, category_id=2, event_name=LibB_Func2}
packet[12]: {Event: category_name = libA_Cat1, category_id=1, event_name=LibA_Func4}
packet[13]: {Event: category_name = libA_Cat1, category_id=1, event_name=LibA_Func3}
packet[14]: {Event: category_name = libC_Cat2, category_id=19, event_name=LibC_Func3}
packet[15]: {Event: category_name = libC_Cat3, category_id=35, event_name=LibC_Func4}
packet[16]: {Event: category_name = libC_Cat2, category_id=19, event_name=LibC_Func3}
packet[17]: {Event: category_name = libB_Cat2, category_id=18, event_name=LibB_Being_Called_From_LibC}
packet[18]: {Event: category_name = libB_Cat1, category_id=2, event_name=LibB_Func3}
packet[19]: {Event: category_name = libA_Cat3, category_id=33, event_name=LibA_Being_Called_From_LibC}
packet[20]: {Event: category_name = libA_Cat1, category_id=1, event_name=LibA_Func3}
packet[21]: {Event: category_name = libC_Cat1, category_id=3, event_name=LibC_Func2}
packet[22]: {Event: category_name = libB_Cat1, category_id=2, event_name=LibB_Func2}
packet[23]: {Event: category_name = libA_Cat1, category_id=1, event_name=LibA_Func4}
packet[24]: {Event: category_name = libA_Cat1, category_id=1, event_name=LibA_Func3}
```

-------

For Further discussion : `mohitms@google.com`

