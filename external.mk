# FloatOS compatibility flags and custom desktop packages.

# GCC 14 diagnoses this legacy libfm assignment as an error.
LIBFM_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -Wno-error=incompatible-pointer-types"

include $(sort $(wildcard $(BR2_EXTERNAL_FLOATOS_PATH)/package/*/*.mk))
