#special targets
.PHONY: clean, mrproper

.SUFFIXES:

#var
CC = x86_64-w64-mingw32-g++ -std=c++11 -m64
SRCDIR = src
BUILDDIR = build
TARGET = bin/yoyo.exe

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS = -g -Wall -pthread
INC = -I include

#create exec file yoyo.exe
$(TARGET): $(OBJECTS)
		@echo " Linking..."
		@echo " $(CC) $^ -o $(TARGET)"
		$(CC) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
		@mkdir -p $(BUILDDIR)
		@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"
		$(CC) $(CFLAGS) $(INC) -c -o $@ $<

#delete temporary files
clean:
		@echo " cleaning..."
		@echo " rm -rf $(BUILDDIR)"
		rm -rf $(BUILDDIR)

#prepare rebuild
mrproper: clean
		@echo " cleaning..."
		@echo " rm -rf $(TARGET)"
		rm -rf $(TARGET)