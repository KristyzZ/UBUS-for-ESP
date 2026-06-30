CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11 -MMD -MP -I../lib/libserialport -I./includes

LDLIBS := -lubox -lubus -lserialport
LDFLAGS := -L../lib/lib -L../lib/libserialport/.libs -Wl,-rpath,'$$ORIGIN/lib/lib' -Wl,-rpath,'$$ORIGIN/lib/libserialport/.libs'

BUILD_DIR := ../

TARGET := ubus-for-esp
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: default_target clean
default_target: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(BUILD_DIR)/$(TARGET)
-include $(DEPS)