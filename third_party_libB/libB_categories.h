#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(libB,
                            perfetto::Category{"libB_Cat1"},
                            perfetto::Category{"libB_Cat2"},
                            perfetto::Category{"libB_Cat3"},
                            perfetto::Category{"libB_Cat4"});
