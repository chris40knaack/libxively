LIBXIVELY := $(shell git rev-parse --show-toplevel)

include $(LIBXIVELY)/Makefile.include

ifeq ($(XI_MQTT_ENABLED),true)
	XI_EXAMPLE_SOURCES = mqtt_connect_and_publish.c
    XI_LAYER_DIRS += ../libxively/mqtt/layer ../libxively/mqtt/codec
else
	XI_EXAMPLE_SOURCES = feed_get.c
	XI_LAYER_DIRS += ../libxively/csv ../libxively/http
endif

XI_CFLAGS += $(foreach layerdir,$(XI_LAYER_DIRS),-I$(layerdir))

XI_CFLAGS += -I../libxively/
XI_EXAMPLES = $(addprefix $(XI_BINDIR)/,$(XI_EXAMPLE_SOURCES:.c=))

all: $(XI_EXAMPLES)

$(XI_BINDIR)/%: %.c $(XI)
	@-mkdir -p $(dir $@)
	$(CC) $(XI_CFLAGS) $^ -o $@

$(XI):
	$(MAKE) -C .. libxively

include $(LIBXIVELY)/Makefile.rules
