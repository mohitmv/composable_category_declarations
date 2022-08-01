#include "main_using_libA.h"

#include "main_app_categories.h"
#include "third_party_libA/libA.h" // Exposing libA's categories as well.

// An example of TU having access to only `main_app_categories` and
// `libA_categories`.

void MainFunc1UsingLibA() {
  // This category is getting resolved to main_app's categories.
  TRACE_EVENT("main_app_Cat3", "MainFunc1UsingLibA");
  {
    // This category is getting resolved to libA's categories.
    TRACE_EVENT("libA_Cat2", "CallingLibA_Func2");
    LibA_Func2();
  }
}
