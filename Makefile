.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR   ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

CTRPFLIB ?= $(TOPDIR)/libctrpf

PLGNAME  := Gen6CTRPluginFramework
PLGINFO  := $(PLGNAME).plgInfo

BUILD    := Build
INCLUDES := Includes
SOURCES  := Sources

#---------------------------------------------------------------------------------
# Options for code generation
#---------------------------------------------------------------------------------

ARCH      := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS    := $(ARCH) -Os -mword-relocations \
             -fomit-frame-pointer -ffunction-sections -fno-strict-aliasing

CFLAGS    += $(INCLUDE) -D__3DS__

CXXFLAGS  := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS   := $(ARCH)
LDFLAGS   := -T $(TOPDIR)/3gx.ld $(ARCH) -Os -Wl,--gc-sections,--strip-discarded,--strip-debug

LIBS      := -lctrpf -lctru
LIBDIRS   := $(TOPDIR) $(CTRPFLIB) $(CTRULIB) $(PORTLIBS)

#---------------------------------------------------------------------------------
# No real need to edit anything past this point unless you need to add additional rules for different file extensions
#---------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT   := $(TOPDIR)/$(PLGNAME)
export TOPDIR   := $(TOPDIR)
export VPATH    := $(foreach dir,$(SOURCES),$(TOPDIR)/$(dir)) $(foreach dir,$(DATA),$(TOPDIR)/$(dir))

export DEPSDIR  := $(TOPDIR)/$(BUILD)

CFILES          := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES        := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES          := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export LD       := $(CXX)
export OFILES   := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE  := $(foreach dir,$(INCLUDES),-I $(TOPDIR)/$(dir)) \
                   $(foreach dir,$(LIBDIRS),-I $(dir)/include) \
                   -I $(TOPDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),-L $(dir)/lib)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(TOPDIR)/Makefile
	@rm -rf $(BUILD)

#---------------------------------------------------------------------------------

clean:
	@echo Cleaning ...
	@rm -fr $(BUILD) $(OUTPUT).3gx $(OUTPUT).elf luma luma.zip

re: clean all

#---------------------------------------------------------------------------------

else

DEPENDS := $(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------------

$(OUTPUT).3gx : $(OFILES)

#---------------------------------------------------------------------------------
# You need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------

%.bin.o: %.bin
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------

ifeq ($(shell uname -s), Darwin)
    TOOL = $(TOPDIR)/3gxtool
else
    TOOL = $(TOPDIR)/3gxtool.exe
endif

%.3gx: %.elf
	@echo creating $(notdir $@)
	@$(TOOL) -s $(word 1, $^) $(TOPDIR)/$(PLGINFO) $@

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif