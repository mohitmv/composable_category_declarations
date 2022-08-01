#pragma once

#include "main_app_categories.h"

inline void MainApp_Func1() { TRACE_EVENT("main_app_Cat2", "MainApp_Func1"); }

inline void MainApp_Func2() { TRACE_EVENT("main_app_Cat6", "MainApp_Func2"); }

void MainApp_Func3();

inline void MainApp_Func4() {
  TRACE_EVENT("main_app_Cat7", "MainApp_Func4");
  return MainApp_Func3();
}
