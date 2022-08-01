#include "libC.h"

#include "third_party_libA/libA.h" // Exposing libA's categories as well.

void LibC_Func3() { TRACE_EVENT("libC_Cat2", "LibC_Func3"); }

void LibC_UsingLibACategory() {
  TRACE_EVENT("libA_Cat3", "LibA_Being_Called_From_LibC");
  return LibA_Func3();
}
