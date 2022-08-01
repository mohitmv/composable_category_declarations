#pragma once

#include "libC_categories.h"

#include "third_party_libA/libA.h" // Exposing libA's categories as well.
#include "third_party_libB/libB.h" // Exposing libB's categories as well.

inline void LibC_Func1() { TRACE_EVENT("libC_Cat2", "LibC_Func1"); }

inline void LibC_Func2() {
  TRACE_EVENT("libC_Cat1", "LibC_Func2");
  LibB_Func2();
  LibA_Func4();
}

void LibC_Func3();

inline void LibC_Func4() {
  TRACE_EVENT("libC_Cat3", "LibC_Func4");
  return LibC_Func3();
}

inline void LibC_UsingLibBCategory() {
  TRACE_EVENT("libB_Cat2", "LibB_Being_Called_From_LibC");
  return LibB_Func3();
}

void LibC_UsingLibACategory();
