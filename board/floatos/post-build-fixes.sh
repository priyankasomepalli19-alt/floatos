#!/bin/sh
set -eu
TARGET_DIR="${1:?target directory is required}"

# Fail the image build if the custom desktop shell or theme was not installed.
test -x "$TARGET_DIR/usr/bin/floatos-shell" || {
    echo "FloatOS desktop shell was not installed into the target rootfs." >&2
    exit 1
}
test -s "$TARGET_DIR/usr/share/floatos/theme.css" || {
    echo "FloatOS desktop theme is missing from the target rootfs." >&2
    exit 1
}
