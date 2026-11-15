# FloatOS uses upstream Buildroot packages only.

# GCC 14 diagnoses this legacy libfm 1.3.2 assignment as an error.
# Keep the compatibility flag scoped to libfm rather than weakening all packages.
LIBFM_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -Wno-error=incompatible-pointer-types"
