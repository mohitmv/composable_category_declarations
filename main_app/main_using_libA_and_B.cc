#include "main_using_libA_and_B.h"

#include "third_party_libA/libA.h"  // Exposing libA's categories as well.
#include "third_party_libB/libB.h"  // Exposing libB's categories as well.

// An example of TU having access to only `libA_categories` and
// `libB_categories` but dont have access to `main_app_categories`.

void MainFunc1UsingLibAAndB() {
  {
    // This category is getting resolved to libA's categories.
    TRACE_EVENT("libA_Cat2", "CallingLibA_Func2");
    LibA_Func2();
  }
  {
    // This category is getting resolved to libB's categories.
    TRACE_EVENT("libB_Cat2", "CallingLibB_Func1");
    LibB_Func1();
  }
}

