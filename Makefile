rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
platformpth = $(subst /,$(PATHSEP),$1)

buildDir := bin
executable := app
target := $(buildDir)/$(executable)
sources := $(call rwildcard,src/,*.cpp)
objects := $(patsubst src/%, $(buildDir)/%, $(patsubst %.cpp, %.o, $(sources)))
depends := $(patsubst %.o, %.d, $(objects))

includes := -I vendor/glfw/include -I $(VULKAN_SDK)/include
linkFlags = -L lib/$(platform) -lglfw3
compileFlags := -std=c++17 $(includes)

ifeq ($(OS),Windows_NT)
	LIB_EXT = .lib
	CMAKE_CMD = cmake -G "MinGW Makefiles" .

	vulkanLibDir := Lib
	vulkanLib := vulkan-1
	vulkanLink := -l $(vulkanLib)

	platform := Windows
	CXX ?= g++
	linkFlags += $(vulkanLink) -Wl,--allow-multiple-definition -pthread -lopengl32 -lgdi32 -lwinmm -mwindows -static -static-libgcc -static-libstdc++
	THEN := &&
	PATHSEP := \$(BLANK)
	MKDIR := -mkdir -p
	RM := -del /q
	COPY = -robocopy "$(call platformpth,$1)" "$(call platformpth,$2)" $3
endif

# Lists phony targets for Makefile
.PHONY: all setup submodules execute clean shaders

all: $(target) execute clean

submodules:
	git submodule update --init --recursive

setup: submodules lib shaders

lib:
	cd vendor/glfw $(THEN) $(CMAKE_CMD) $(THEN) "$(MAKE)" 
	$(MKDIR) $(call platformpth, lib/$(platform))
	$(call COPY,vendor/glfw/src,lib/$(platform),libglfw3.a)
	$(call COPY,$(VULKAN_SDK)/$(vulkanLibDir),lib/$(platform),$(vulkanLibPrefix)$(vulkanLib)$(LIB_EXT))
	$(macOSVulkanLib)

# Link the program and create the executable
$(target): $(objects)
	$(CXX) $(objects) -o $(target) $(linkFlags)

# Add all rules from dependency files
-include $(depends)

# Compile objects to the build directory
$(buildDir)/%.o: src/%.cpp Makefile
	$(MKDIR) $(call platformpth, $(@D))
	$(CXX) -MMD -MP -c $(compileFlags) $< -o $@ $(CXXFLAGS)

clear: 
	clear;

execute: 
	$(target) $(ARGS)

clean: 
	$(RM) $(call platformpth, $(buildDir)/*)

shaders: $(patsubst shaders/%.vert, shaders/%.vert.spv, $(wildcard shaders/*.vert)) \
         $(patsubst shaders/%.frag, shaders/%.frag.spv, $(wildcard shaders/*.frag))

shaders/%.vert.spv: shaders/%.vert
	$(VULKAN_SDK)/Bin/glslc.exe $< -o $@

shaders/%.frag.spv: shaders/%.frag
	$(VULKAN_SDK)/Bin/glslc.exe $< -o $@
