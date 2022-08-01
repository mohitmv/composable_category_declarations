CCFLAGS=" -c -std=c++11 -I../"

rm -rf /tmp/libA /tmp/perfetto_sdk

mkdir -p /tmp/libA
mkdir -p /tmp/perfetto_sdk

g++ $CCFLAGS libA_test_main.cc -o /tmp/libA/libA_test_main.o &&
g++ $CCFLAGS libA_categories.cc -o /tmp/libA/libA_categories.o &&
g++ $CCFLAGS libA.cc -o /tmp/libA/libA.o &&

g++ $CCFLAGS ../perfetto_sdk/track_event_category_registry.cc -o /tmp/perfetto_sdk/track_event_category_registry.o &&

g++ $CCFLAGS ../perfetto_sdk/tracing.cc -o /tmp/perfetto_sdk/tracing.o &&

g++ /tmp/libA/libA_test_main.o /tmp/libA/liba_categories.o /tmp/perfetto_sdk/track_event_category_registry.o /tmp/perfetto_sdk/tracing.o  /tmp/libA/liba.o -o /tmp/libA/libA_test_main &&

# g++ /tmp/libA/libA_test_main.o /tmp/libA/libA_categories.o -o /tmp/libA/libA_test_main &&

/tmp/libA/libA_test_main

