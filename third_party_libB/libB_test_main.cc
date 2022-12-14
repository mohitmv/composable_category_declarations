#include "libB.h"
#include "perfetto_sdk/tracing.h"

#include <iostream>

void TopLevelFunc() {
  LibB_Func1();
  LibB_Func2();
  LibB_Func3();
  LibB_Func2();
  LibB_Func2();
  LibB_Func4();
}

int main() {
  perfetto::TrackEvent::Register();
  auto session =
      perfetto::StartSession({"libB_Cat3", "libB_Cat4", "libB_Cat1"});
  TopLevelFunc();
  auto trace = session.StopAndReturnTrace();
  std::cout << "Trace = \n";
  perfetto::Print(trace);
  auto trace2 = perfetto::TraceProcess(trace);
  std::cout << "---------\nAfter TraceProcessor = \n";
  perfetto::Print(trace2);
  std::cout << "---------\n";
}
