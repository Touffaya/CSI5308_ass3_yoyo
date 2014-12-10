#special targets
.PHONY: clean, mrproper

.SUFFIXES:

#var
CC = x86_64-w64-mingw32-g++ -std=c++1y -m64
SRCDIR = src
BUILDDIR = build
TARGET = bin/yoyo.exe
DEBUG = bin/debug.exe

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS = -g -Wall
LIB = -pthread
INC = -I include

#create exec file yoyo.exe
$(TARGET): $(OBJECTS)
		@echo " Linking..."
		@echo " $(CC) $^ -o $(TARGET) $(LIB)"
		$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
		@mkdir -p $(BUILDDIR)
		@echo " $(CC) $(CFLAGS) -DNDEBUG $(INC) -c -o $@ $<";
		$(CC) $(CFLAGS) -DNDEBUG $(INC) -c -o $@ $<

#create exec with debugging
debug:	
		@echo " $(CC) $(CFLAGS) $(SOURCES) $(INC) $(LIB) -o $(DEBUG)";
		$(CC) $(CFLAGS) $(SOURCES) $(INC) $(LIB) -o $(DEBUG)


#delete temporary files
clean:
		@echo " cleaning..."
		@echo " rm -rf $(BUILDDIR)";
		rm -rf $(BUILDDIR)

#prepare rebuild
mrproper: clean
		@echo " cleaning..."
		@echo " rm -rf $(TARGET)";
		rm -rf $(TARGET)
		@echo " rm -rf $(DEBUG)";
		rm -rf $(DEBUG)

#delete debug files
clean_debug:
		@echo " Removing debug..."
		@echo " rm -rf $(DEBUG)";
		rm -rf $(DEBUG)