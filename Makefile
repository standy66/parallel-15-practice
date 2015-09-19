CC = clang++
CFLAGS = -Wall -std=c++03
LDFLAGS =
BUILDDIR = build/
SOURCES = main.cpp
OBJECTS = $(addprefix $(BUILDDIR), $(SOURCES:.cpp=.o))
EXECUTABLE = $(addprefix $(BUILDDIR), life)

all: $(EXECUTABLE) $(SOURCES)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILDDIR)%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@
