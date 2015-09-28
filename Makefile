DEBUG_DIR := build/debug/
RELEASE_DIR := build/release/
SRC_DIR := src/
INCLUDE_DIR := include/

CC := clang++
CC_DEBUG_FLAGS := -Wall -Wextra -std=c++03 -I$(INCLUDE_DIR) -DDEBUG -g
CC_RELEASE_FLAGS := -Wall -Wextra -O3 -std=c++03 -I$(INCLUDE_DIR)
LD_FLAGS := -lpthread

SRC_LIST := main.cpp shell.cpp life_game.cpp utils.cpp threading.cpp life_game_singlethreaded.cpp

DEBUG_OBJECTS := $(addprefix $(DEBUG_DIR), $(SRC_LIST:.cpp=.o))
RELEASE_OBJECTS := $(addprefix $(RELEASE_DIR), $(SRC_LIST:.cpp=.o))
OBJECTS := $(DEBUG_OBJECTS) $(RELEASE_OBJECTS)

DEBUG_TARGET := bin/debug
RELEASE_TARGET := bin/release

all: debug release

debug: $(DEBUG_TARGET)

release: $(RELEASE_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CC) $^ $(LD_FLAGS) -o $@

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CC) $^ $(LD_FLAGS) -o $@

$(DEBUG_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) -c $^ $(CC_DEBUG_FLAGS) -o $@

$(RELEASE_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) -c $^ $(CC_RELEASE_FLAGS) -o $@

clean:
	-rm -f $(OBJECTS) $(RELEASE_TARGET) $(DEBUG_TARGET)

.PHONY: clean all debug release
