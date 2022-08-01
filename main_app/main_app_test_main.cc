#include "perfetto_sdk/tracing.h"

#include "main_app.h"
#include "main_using_libA.h"
#include "main_using_libA_and_B.h"
#include "main_using_libA_and_C.h"

#include <iostream>

void TopLevelFunc() {
  MainApp_Func3();
  MainFunc1UsingLibAAndB();
  MainApp_Func1();
  MainApp_Func2();
  MainApp_Func3();
  MainFunc1UsingLibA();
  MainFunc1UsingLibAAndC();
  MainApp_Func4();
}

int main() {
  perfetto::TrackEvent::Register();
  auto session = perfetto::StartSession();
  TopLevelFunc();
  auto trace = session.StopAndReturnTrace();
  std::cout << "Trace = \n";
  perfetto::Print(trace);
  auto trace2 = perfetto::TraceProcess(trace);
  std::cout << "---------\nAfter TraceProcessor (replacing category_id by names) = \n";
  perfetto::Print(trace2);
  std::cout << "---------\n";
}
