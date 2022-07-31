#include "main_using_libA_and_C.h"

#include "third_party_libA/libA.h"  // Exposing libA's categories as well.
#include "third_party_libC/libC.h"  // Exposing libC's categories as well.

// An example of TU having access to only `libA_categories` and
// `libC_categories`, but in turn having exposure (transitively) to
// `libB_categories` as well via `libC`.
// It doesn't have access to `main_app_categories`.

void MainFunc1UsingLibAAndC() {
  {
    // This category is getting resolved to libA's categories.
    TRACE_EVENT("libA_Cat2", "CallingLibA_Func4");
    LibA_Func4();
  }
  {
    // This category is getting resolved to libC's categories.
    TRACE_EVENT("libC_Cat3", "CallingLibC_Func1");
    LibC_Func1();
  }
}

