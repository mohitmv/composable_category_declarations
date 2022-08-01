#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(libC, perfetto::Category{"libC_Cat1"},
                            perfetto::Category{"libC_Cat2"},
                            perfetto::Category{"libC_Cat3"}, );
