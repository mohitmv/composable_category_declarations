#pragma once

#include "libA_categories.h"

inline void LibA_Func1() { TRACE_EVENT("libA_Cat2", "LibA_Func1"); }

inline void LibA_Func2() { TRACE_EVENT("libA_Cat3", "LibA_Func2"); }

void LibA_Func3();

inline void LibA_Func4() {
  TRACE_EVENT("libA_Cat1", "LibA_Func4");
  return LibA_Func3();
}
