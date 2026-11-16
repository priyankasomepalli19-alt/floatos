################################################################################
# floatos-shell
################################################################################

FLOATOS_SHELL_VERSION = 1.0
FLOATOS_SHELL_SITE = $(BR2_EXTERNAL_FLOATOS_PATH)/package/floatos-shell/src
FLOATOS_SHELL_SITE_METHOD = local
FLOATOS_SHELL_DEPENDENCIES = libgtk3
FLOATOS_SHELL_INSTALL_TARGET = YES

define FLOATOS_SHELL_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define FLOATOS_SHELL_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/floatos-shell $(TARGET_DIR)/usr/bin/floatos-shell
endef

$(eval $(generic-package))