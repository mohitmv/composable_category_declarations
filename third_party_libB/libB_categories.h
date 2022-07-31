#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(
  PERFETTO_CATEGORY_DECLARATION_SLOT_NUMBER(2),
  perfetto::Category{"libB_Cat1"},
  perfetto::Category{"libB_Cat2"},
  perfetto::Category{"libB_Cat3"},
);
