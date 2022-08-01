#include "libC.h"
#include "perfetto_sdk/tracing.h"

#include <iostream>

void TopLevelFunc() {
  LibC_Func1();
  LibC_Func2();
  LibC_Func3();
  LibC_Func4();
  LibC_UsingLibBCategory();
  LibC_UsingLibACategory();
  LibC_Func2();
}

int main() {
  perfetto::TrackEvent::Register();
  auto session = perfetto::StartSession();
  TopLevelFunc();
  auto trace = session.StopAndReturnTrace();
  std::cout << "Trace = \n";
  perfetto::Print(trace);
  auto trace2 = perfetto::TraceProcess(trace);
  std::cout << "---------\nAfter TraceProcessor = \n";
  perfetto::Print(trace2);
  std::cout << "---------\n";
}
