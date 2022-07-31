CCFLAGS=" -c -std=c++11 -I../"

rm -rf /tmp/libB /tmp/perfetto_sdk

mkdir -p /tmp/libB
mkdir -p /tmp/perfetto_sdk

g++ $CCFLAGS libB_test_main.cc -o /tmp/libB/libB_test_main.o &&
g++ $CCFLAGS libB_categories.cc -o /tmp/libB/libB_categories.o &&
g++ $CCFLAGS libB.cc -o /tmp/libB/libB.o &&

g++ $CCFLAGS ../perfetto_sdk/track_event_category_registry.cc -o /tmp/perfetto_sdk/track_event_category_registry.o &&

g++ $CCFLAGS ../perfetto_sdk/tracing.cc -o /tmp/perfetto_sdk/tracing.o &&

g++ /tmp/libB/libB_test_main.o /tmp/libB/libB_categories.o /tmp/perfetto_sdk/track_event_category_registry.o /tmp/perfetto_sdk/tracing.o  /tmp/libB/libB.o -o /tmp/libB/libB_test_main &&

/tmp/libB/libB_test_main

