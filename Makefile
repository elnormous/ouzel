.PHONY: all
all: engine tools

.PHONY: engine
engine:
	$(MAKE) -C engine DEBUG=$(DEBUG) PLATFORM=$(PLATFORM) VC_DIR=$(VC_DIR) $(target)

.PHONY: tools
tools:
	$(MAKE) -C tools DEBUG=$(DEBUG)

.PHONY: clean
clean:
	$(MAKE) -C engine clean
	$(MAKE) -C tools clean