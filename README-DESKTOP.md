# FloatOS Desktop — first native shell milestone

This work replaces the generic Matchbox panel layout with a FloatOS-owned GTK3 shell.

## User interface

- FloatOS top bar with network state and live clock
- Animated application launcher with search and app tiles
- Centered floating dock
- Quick settings popover for network, sound, display and power
- Settings window with animated page transitions
- Charcoal, white and lime visual theme
- Openbox keyboard shortcuts for launcher, files and terminal

## Architecture

FloatOS remains an independent x86-64 Linux system built from upstream source with Buildroot.
The desktop shell is a native GTK3 application packaged as floatos-shell.
Openbox remains the lightweight window manager; GTK3 provides the FloatOS-owned desktop UI.
The previous ISO is not modified by this source work.

## Build verification

The next validation stage must compile the floatos-shell Buildroot package and its GTK3
dependencies before starting the full ISO workflow. This milestone is source code and
configuration, not yet a boot-tested desktop or a finished installer.