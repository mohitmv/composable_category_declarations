#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(
  PERFETTO_CATEGORY_DECLARATION_SLOT_NUMBER(3),
  perfetto::Category{"libC_Cat1"},
  perfetto::Category{"libC_Cat2"},
  perfetto::Category{"libC_Cat3"},
);
