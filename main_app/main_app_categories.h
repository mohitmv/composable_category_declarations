#pragma once

#include "perfetto_sdk/tracing.h"

PERFETTO_DECLARE_CATEGORIES(main_app,
                            perfetto::Category{"main_app_Cat1"},
                            perfetto::Category{"main_app_Cat2"},
                            perfetto::Category{"main_app_Cat3"},
                            perfetto::Category{"main_app_Cat4"},
                            perfetto::Category{"main_app_Cat5"},
                            perfetto::Category{"main_app_Cat6"},
                            perfetto::Category{"main_app_Cat7"}, );
