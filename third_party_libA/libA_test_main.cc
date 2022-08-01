#include "libA.h"

#include "perfetto_sdk/tracing.h"

#include <iostream>

void TopLevelFunc() {
  LibA_Func1();
  LibA_Func2();
  LibA_Func3();
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
