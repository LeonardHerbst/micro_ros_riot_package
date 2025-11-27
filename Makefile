PKG_NAME=micro_ros
PKG_URL=https://github.com/LeonardHerbst/micro_ros_riot
PKG_VERSION=18f9ab8cad08423d8c5ad6c4d3dc9a83c699d943
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

ifeq (,$(filter micro_ros_%_transport,$(USEMODULE)))
  $(error "Please specify a transport that Micro-ROS should use. \
  Either USEMODULE=micro_ros_serial_transport or USEMODULE=micro_ros_udp_transport")
endif
ifneq (,$(filter micro_ros_serial_transport,$(USEMODULE)))
  ifneq (,$(filter micro_ros_serial_transport,$(USEMODULE)))
    $(error "micro_ros_serial_transport and micro_ros_udp_transport are mutually exclusive!")
  endif
endif

CFLAGS += -Wno-error
CXXFLAGS = $(filter-out $(CXXUWFLAGS), $(CFLAGS)) $(CXXEXFLAGS)

all: $(BINDIR)/micro_ros.a

prepare: $(BINDIR)/.prepared

$(BINDIR)/.prepared:
	@mkdir -p $(BINDIR)/include

$(BINDIR)/micro_ros.a: $(PKG_BUILD_DIR)/toolchain.cmake $(PKG_BUILD_DIR)/toolchain_host.cmake $(PKG_SOURCE_DIR)/kconfig_vars
	$(QQ) "$(MAKE)" -C $(PKG_SOURCE_DIR)/ all \
		PKG_SOURCE_DIR=$(PKG_SOURCE_DIR) \
		PKG_BUILD_DIR=$(PKG_BUILD_DIR);

$(PKG_BUILD_DIR)/toolchain.cmake:
	$(QQ) mkdir -p $(PKG_BUILD_DIR)
	$(QQ) $(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $@
	@echo "set(CMAKE_CXX_FLAGS [==[$(CXXFLAGS)]==] CACHE STRING \"\" FORCE)" >> $@

$(PKG_BUILD_DIR)/toolchain_host.cmake:
	@echo "set(CMAKE_SYSTEM_NAME RIOT CACHE STRING \"\")" > $@
	@echo "set(CMAKE_SYSTEM_PROCESSOR \"\" CACHE STRING \"\")" >> $@
	@echo "set(CMAKE_C_COMPILER \"gcc\" CACHE FILEPATH \"\")" >> $@
	@echo "set(CMAKE_CXX_COMPILER \"g++\" CACHE FILEPATH \"\")" >> $@
	@echo "set(CMAKE_C_COMPILER_WORKS TRUE CACHE BOOL \"\")" >> $@
	@echo "set(CMAKE_CXX_COMPILER_WORKS TRUE CACHE BOOL \"\")" >> $@
	@echo "set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)" >> $@
	@echo "set(CMAKE_C_FLAGS \"\" CACHE STRING \"\" FORCE)" >> $@
	@echo "set(CMAKE_CXX_FLAGS \"\" CACHE STRING \"\" FORCE)" >> $@

$(PKG_SOURCE_DIR)/kconfig_vars:
	@if [ -f $(BINDIR)/generated/out.config ]; then \
		grep 'CONFIG_MICRO_ROS_' $(BINDIR)/generated/out.config > $@; \
	fi

clean::
	$(BINDIR)/.prepared:
	$(QQ) rm $(PKG_BUILD_DIR)/toolchain.cmake
	$(QQ) rm $(PKG_BUILD_DIR)/toolchain_host.cmake
	$(QQ) rm -rf $(PKG_BUILD_DIR)/configured_colcon.meta
	$(QQ) rm -rf $(PKG_SOURCE_DIR)/kconfig_vars
	$(QQ) "$(MAKE)" -C $(PKG_SOURCE_DIR)/ clean \
		PKG_SOURCE_DIR=$(PKG_SOURCE_DIR) \
		PKG_BUILD_DIR=$(PKG_BUILD_DIR);

.PHONY: all clean
