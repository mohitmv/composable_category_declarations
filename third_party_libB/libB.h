#pragma once

#include "libB_categories.h"

inline void LibB_Func1() { TRACE_EVENT("libB_Cat2", "LibB_Func1"); }

inline void LibB_Func2() { TRACE_EVENT("libB_Cat1", "LibB_Func2"); }

void LibB_Func3();

inline void LibB_Func4() {
  TRACE_EVENT("libB_Cat4", "LibB_Func4");
  return LibB_Func3();
}
