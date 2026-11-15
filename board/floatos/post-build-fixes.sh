#!/bin/sh
set -eu
TARGET_DIR="${1:?target directory is required}"

# FloatOS starts X.Org through startx from the user's graphical session.
# Disable Buildroot's standalone X.Org init service to avoid starting a
# second server on display :0 before the desktop session begins.
rm -f "$TARGET_DIR/etc/init.d/S40xorg"

# X.Org is started from a dedicated unprivileged live desktop account.
# On this no-logind Buildroot live image, the server must be setuid root
# to acquire the VT and DRM/input devices.
if [ -x "$TARGET_DIR/usr/bin/Xorg" ]; then
    chmod 4755 "$TARGET_DIR/usr/bin/Xorg"
elif [ -x "$TARGET_DIR/usr/lib/xorg/Xorg" ]; then
    chmod 4755 "$TARGET_DIR/usr/lib/xorg/Xorg"
else
    echo "FloatOS: X.Org server binary not found" >&2
    exit 1
fi

# Keep the FloatOS home directory writable by the live desktop user.
chown -R 1000:1000 "$TARGET_DIR/home/floatos"

# Give each panel a unique panel ID.
sed -i 's/matchbox-panel --orientation north/matchbox-panel --id 1 --orientation north/' "$TARGET_DIR/home/floatos/.xinitrc"
sed -i 's/matchbox-panel --orientation south/matchbox-panel --id 2 --orientation south/' "$TARGET_DIR/home/floatos/.xinitrc"
