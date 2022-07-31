#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(
  PERFETTO_CATEGORY_DECLARATION_SLOT_NUMBER(1),
  perfetto::Category{"libA_Cat1"},
  perfetto::Category{"libA_Cat2"},
  perfetto::Category{"libA_Cat3"},
);

#include <iostream>

struct S {
  S() { std::cout << "S::S" << std::endl; }
  S(int x) { std::cout << "S::S x = " << x << std::endl; }
};

