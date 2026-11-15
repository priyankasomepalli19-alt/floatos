#!/bin/sh
set -eu
TARGET_DIR="${1:?target directory is required}"

# Start X.Org manually through startx in the FloatOS desktop session.
rm -f "$TARGET_DIR/etc/init.d/S40xorg"

# Give each Matchbox panel a unique panel ID.
sed -i 's/matchbox-panel --orientation north/matchbox-panel --id 1 --orientation north/' "$TARGET_DIR/home/floatos/.xinitrc"
sed -i 's/matchbox-panel --orientation south/matchbox-panel --id 2 --orientation south/' "$TARGET_DIR/home/floatos/.xinitrc"
