CC := g++
SRCDIR := src
BUILDDIR := build
TARGET := app

SRCEXT := cpp
CFLAGS := -g -Wno-deprecated -std=c++17 -Wall
INC := -I .
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIB := -lglut -lGLU -lGL
	SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
else
	LIB := -framework OpenGL -framework GLUT
	SOURCES := $(shell find -E $(SRCDIR) -type f -regex ".*\.($(SRCEXT))$$")
endif
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))


$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) -o $(TARGET) $^ $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(shell dirname $@)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean