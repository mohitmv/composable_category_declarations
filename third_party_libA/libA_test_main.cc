#include "libA.h"
#include "perfetto_sdk/tracing.h"

#include <iostream>

void TopLevelFunc() {
  LibA_Func1();
  LibA_Func2();
  LibA_Func3();
}

template <size_t input_value> struct Value {
  static constexpr size_t value = input_value;
};

void Test1() {
  using perfetto::internal::GetCategoryId;
  struct unique_type1;
  // Wrapped in Value<> to prove that expression returned by GetCategoryId is
  // truly a compile time constant.
  std::cout << "category_id = "
            << Value<GetCategoryId<unique_type1>("libA_Cat2")>::value
            << std::endl;
}

int main() {
  Test1();
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
