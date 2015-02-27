# GNU Makefile
# Copyright (C) 2012 Przemyslaw Pawelczyk <przemoc@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SRCS := main.c
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(addsuffix .d,$(basename $(SRCS)))
NAME := awaitsig

SRCDIR := $(dir $(lastword $(MAKEFILE_LIST)))
OBJDIR ?= ./
OBJS := $(addprefix $(OBJDIR),$(OBJS))
BIN := $(NAME)

vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)

CC ?= gcc
INSTALL ?= install

PREFIX ?= /usr
CFLAGS ?= -O2
LDFLAGS ?= -static
MANDATORY_FLAGS := -std=gnu99 -Wall -Wextra -pedantic
override CFLAGS := $(MANDATORY_FLAGS) $(CFLAGS)

CC_PARAMS = $(CFLAGS)

all: package

objects: $(OBJS)
package: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)%.o: %.c
	$(CC) $(CC_PARAMS) -c -o $@ $<

%.d: %.c
	@$(CC) $(CC_PARAMS) -MT "\$$(OBJDIR)$(basename $^).o $@" -MM $< -o $@

clean:
	$(RM) $(BIN) $(OBJS) $(DEPS)

install: package
	$(INSTALL) $(BIN) $(PREFIX)/bin/$(NAME)

.PHONY: all clean objects package install

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
