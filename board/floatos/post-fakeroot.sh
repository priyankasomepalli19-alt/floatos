#!/bin/sh
set -eu
TARGET_DIR="${1:?target directory is required}"

# This script runs inside Buildroot's fakeroot phase. Ownership and special
# permission changes must be made here, not in a normal post-build script.
if [ -x "$TARGET_DIR/usr/bin/Xorg" ]; then
    chmod 4755 "$TARGET_DIR/usr/bin/Xorg"
elif [ -x "$TARGET_DIR/usr/lib/xorg/Xorg" ]; then
    chmod 4755 "$TARGET_DIR/usr/lib/xorg/Xorg"
else
    echo "FloatOS: X.Org server binary not found" >&2
    exit 1
fi

FLOATOS_UID="$(awk -F: '$1 == "floatos" { print $3 }' "$TARGET_DIR/etc/passwd")"
FLOATOS_GID="$(awk -F: '$1 == "floatos" { print $3 }' "$TARGET_DIR/etc/group")"
case "$FLOATOS_UID" in
    ''|*[!0-9]*) echo "FloatOS user ID could not be resolved" >&2; exit 1 ;;
esac
case "$FLOATOS_GID" in
    ''|*[!0-9]*) echo "FloatOS group ID could not be resolved" >&2; exit 1 ;;
esac
chown -R "$FLOATOS_UID:$FLOATOS_GID" "$TARGET_DIR/home/floatos"
