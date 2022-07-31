CCFLAGS=" -c -std=c++11 -I../"

rm -rf /tmp/libA /tmp/libB /tmp/libC /tmp/perfetto_sdk

mkdir -p /tmp/libC
mkdir -p /tmp/libB
mkdir -p /tmp/libA
mkdir -p /tmp/perfetto_sdk

g++ $CCFLAGS libC_test_main.cc -o /tmp/libC/libC_test_main.o &&

g++ $CCFLAGS libC_categories.cc -o /tmp/libC/libC_categories.o &&
g++ $CCFLAGS libC.cc -o /tmp/libC/libC.o &&

g++ $CCFLAGS ../third_party_libB/libB_categories.cc -o /tmp/libB/libB_categories.o &&
g++ $CCFLAGS ../third_party_libB/libB.cc -o /tmp/libB/libB.o &&

g++ $CCFLAGS ../third_party_libA/libA_categories.cc -o /tmp/libA/libA_categories.o &&
g++ $CCFLAGS ../third_party_libA/libA.cc -o /tmp/libA/libA.o &&

g++ $CCFLAGS ../perfetto_sdk/track_event_category_registry.cc -o /tmp/perfetto_sdk/track_event_category_registry.o &&
g++ $CCFLAGS ../perfetto_sdk/tracing.cc -o /tmp/perfetto_sdk/tracing.o &&

g++ /tmp/libC/libC_test_main.o /tmp/libC/libC.o /tmp/libC/libC_categories.o /tmp/libA/libA.o /tmp/libA/libA_categories.o  /tmp/libB/libB.o /tmp/libB/libB_categories.o  /tmp/perfetto_sdk/track_event_category_registry.o /tmp/perfetto_sdk/tracing.o  -o /tmp/libC/libC_test_main &&

/tmp/libC/libC_test_main

