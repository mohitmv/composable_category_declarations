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
  auto session = perfetto::StartSession({"libB_Cat3", "libB_Cat1", "libA_Cat1",
                                         "libA_Cat2", "libC_Cat2", "libC_Cat3",
                                         "libC_Cat4"});
  TopLevelFunc();
  auto trace = session.StopAndReturnTrace();
  std::cout << "Trace = \n";
  perfetto::Print(trace);
  auto trace2 = perfetto::TraceProcess(trace);
  std::cout << "---------\nAfter TraceProcessor = \n";
  perfetto::Print(trace2);
  std::cout << "---------\n";
}
