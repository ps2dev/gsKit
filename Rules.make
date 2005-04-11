# Common rules shared by all build targets.

.PHONY: dummy

all: build

# Use SUBDIRS to descend into subdirectories.
subdir_list    = $(patsubst %,all-%,$(SUBDIRS))
subdir_clean   = $(patsubst %,clean-%,$(SUBDIRS))
subdir_install = $(patsubst %,install-%,$(SUBDIRS))
subdirs: dummy $(subdir_list)

build: $(subdir_list)

clean: $(subdir_clean)

install: $(subdir_install)

ifdef SUBDIRS
$(subdir_list): dummy
	GSKITSRC=$(GSKITSRC) $(MAKE) -C $(patsubst all-%,%,$@)
$(subdir_clean): dummy
	GSKITSRC=$(GSKITSRC) $(MAKE) -C $(patsubst clean-%,%,$@) clean
$(subdir_install): dummy
	GSKITSRC=$(GSKITSRC) $(MAKE) -C $(patsubst install-%,%,$@) install
endif

# Default rule for clean.
clean: $(subdir_clean)

# A rule to do nothing.
dummy:
