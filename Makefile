BUILDDIR = build/
SRCDIR = src/
HEADERSDIR = headers/
CC = clang++
CFLAGS = -Wall -std=c++03 -I$(HEADERSDIR) -DDEBUG
LDFLAGS =
SRCLIST = main.cpp shell.cpp
OBJECTS = $(addprefix $(BUILDDIR), $(SRCLIST:.cpp=.o))
EXECUTABLE = $(addprefix $(BUILDDIR), life)

all: $(EXECUTABLE)

run: all
	$(EXECUTABLE)

rebuild: clean all

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILDDIR)%.o: $(SRCDIR)%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	-rm $(EXECUTABLE)
	-rm $(OBJECTS)

.PHONY: clean all run
