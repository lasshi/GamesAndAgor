# ------------------------------------------------
#originates from stackoverflow user 'qroberts' post
#http://stackoverflow.com/a/27794283
#http://ubuntuforums.org/showthread.php?t=1879827
# ------------------------------------------------

# Compiler
CC			:= g++
# The Target Binary Program
TARGET      := program
# The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := res
# Extensions
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

# Flags, Libraries and Includes
CFLAGS      := -Wall -w -std=c++11 -O
LIB         := -lSDL2 -lSDL2main -lSDL2_mixer -lSDL2_ttf
INC         := -I$(INCDIR) -I/usr/include
INCDEP      := -I$(INCDIR)

SOURCES		:=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
#SOURCES		:= $(wildcard $(addsuffix /*.$(SRCEXT), $(SRCDIR)))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Defauilt Make
all: resources $(TARGET)

run: all
	$(TARGETDIR)/$(TARGET)
# Remake
remake: cleaner all

# Copy Resources from Resources Directory to Target Directory
resources: directories
	@cp -r $(RESDIR)/ $(TARGETDIR)/

# Make the Directories
directories:
	#rm -R $(TARGETDIR)
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

# Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

# Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

# Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources
