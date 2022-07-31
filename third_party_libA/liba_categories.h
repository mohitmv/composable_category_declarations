#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(
  PERFETTO_CATEGORY_DECLARATION_SLOT_NUMBER(1),
  perfetto::Category{"libA_Cat1"},
  perfetto::Category{"libA_Cat2"},
  perfetto::Category{"libA_Cat3"},
);
