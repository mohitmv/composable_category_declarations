CCFLAGS=" -c -std=c++11 -I../"

mkdir -p /tmp/libA
mkdir -p /tmp/perfetto_sdk

g++ $CCFLAGS libA_test_main.cc -o /tmp/libA/libA_test_main.o &&
g++ $CCFLAGS liba_categories.cc -o /tmp/libA/liba_categories.o &&
# g++ $CCFLAGS liba.cc -o /tmp/libA/liba.o &&

g++ $CCFLAGS ../perfetto_sdk/track_event_category_registry.cc -o /tmp/perfetto_sdk/perfetto.o &&

# g++ $CCFLAGS ../perfetto_sdk/tracing.cc -o /tmp/perfetto_sdk/tracing.o &&

g++ /tmp/libA/libA_test_main.o /tmp/libA/liba_categories.o /tmp/perfetto_sdk/track_event_category_registry.o -o /tmp/libA/libA_test_main &&

# /tmp/perfetto_sdk/tracing.o  /tmp/libA/liba.o 

/tmp/libA/libA_test_main

