.PHONY: all
all: engine tools editor

.PHONY: engine
engine:
	$(MAKE) -C engine DEBUG=$(DEBUG) PLATFORM=$(PLATFORM) VC_DIR=$(VC_DIR) $(target)

.PHONY: tools
tools:
	$(MAKE) -C tools DEBUG=$(DEBUG)

# editor depends on engine; the editor's Makefile is desktop-only
# (Windows/Linux/macOS), so we skip it for mobile / web platforms.
.PHONY: editor
editor: engine
ifeq ($(PLATFORM),ios)
	@echo "editor: skipped on iOS (desktop-only)"
else ifeq ($(PLATFORM),tvos)
	@echo "editor: skipped on tvOS (desktop-only)"
else ifeq ($(PLATFORM),android)
	@echo "editor: skipped on Android (desktop-only)"
else ifeq ($(PLATFORM),emscripten)
	@echo "editor: skipped on Emscripten (desktop-only)"
else
	$(MAKE) -C editor DEBUG=$(DEBUG) $(target)
endif

.PHONY: clean
clean:
	$(MAKE) -C engine clean
	$(MAKE) -C tools clean
	-$(MAKE) -C editor clean