#!/bin/sh
set -eu
TARGET_DIR="${1:?target directory is required}"

# FloatOS identity and boot-to-desktop configuration.
mkdir -p "$TARGET_DIR/usr/share/backgrounds/floatos"
mkdir -p "$TARGET_DIR/usr/share/floatos"
mkdir -p "$TARGET_DIR/etc/xdg/openbox"
mkdir -p "$TARGET_DIR/etc/xdg/gtk-3.0"
mkdir -p "$TARGET_DIR/home/floatos/Desktop"
mkdir -p "$TARGET_DIR/usr/share/applications"
mkdir -p "$TARGET_DIR/usr/share/themes/FloatOS/openbox-3"

cat > "$TARGET_DIR/etc/os-release" <<'EOF'
NAME=FloatOS
PRETTY_NAME="FloatOS 0.1"
ID=floatos
VERSION="0.1"
VERSION_ID=0.1
ANSI_COLOR="1;32"
HOME_URL="https://github.com/priyankasomepalli19-alt/floatos"
EOF
echo "floatos" > "$TARGET_DIR/etc/hostname"

cat > "$TARGET_DIR/etc/motd" <<'EOF'
  ______ _             _    ____  _____
 |  ____| |           | |  / __ \\/ ____|
 | |__  | | ___   __ _| |_| |  | | (___
 |  __| | |/ _ \\ / _\` | __| |  | |\\___ \\
 | |    | | (_) | (_| | |_| |____) |
 |_|    |_|\\___/ \\__,_|\\__|\\____/|_____/

 FloatOS 0.1 — a desktop that moves with you.
EOF

# Boot directly into the unprivileged FloatOS desktop account.
cat > "$TARGET_DIR/etc/inittab" <<'EOF'
::sysinit:/bin/mount -t proc proc /proc
::sysinit:/bin/mount -o remount,rw /
::sysinit:/bin/mkdir -p /dev/pts /dev/shm
::sysinit:/bin/mount -a
::sysinit:/bin/mkdir -p /run/lock/subsys
null::sysinit:/bin/ln -sf /proc/self/fd /dev/fd
null::sysinit:/bin/ln -sf /proc/self/fd/0 /dev/stdin
null::sysinit:/bin/ln -sf /proc/self/fd/1 /dev/stdout
null::sysinit:/bin/ln -sf /proc/self/fd/2 /dev/stderr
::sysinit:/bin/hostname -F /etc/hostname
::sysinit:/etc/init.d/rcS
tty1::respawn:/sbin/getty -L -n -l /usr/sbin/floatos-login 38400 tty1 vt100
tty2::respawn:/sbin/getty -L 38400 tty2 vt100
tty3::respawn:/sbin/getty -L 38400 tty3 vt100
::ctrlaltdel:/sbin/reboot
::shutdown:/etc/init.d/rcK
::shutdown:/bin/umount -a -r
EOF

cat > "$TARGET_DIR/usr/sbin/floatos-login" <<'EOF'
#!/bin/sh
exec /bin/su -l floatos -c 'exec /usr/bin/startx /home/floatos/.xinitrc -- :0 vt1 -keeptty'
EOF
chmod 0755 "$TARGET_DIR/usr/sbin/floatos-login"

cat > "$TARGET_DIR/home/floatos/.xinitrc" <<'EOF'
#!/bin/sh
xsetroot -solid "#080b0a"
feh --bg-fill /usr/share/backgrounds/floatos/floatos-wallpaper.ppm
xcompmgr -c -t-5 -l-5 -r8 -o 0.24 &
# A slim status strip and a bottom app launcher.
matchbox-panel --orientation north --size 32 --default-apps "mb-applet-menu-launcher,mb-applet-clock" --no-session &
matchbox-panel --orientation south --size 58 --margin-start +500 --margin-end +500 --default-apps "mb-applet-menu-launcher,mb-applet-clock" --no-session &
pcmanfm --desktop --profile=FloatOS &
exec openbox-session
EOF
chmod 0755 "$TARGET_DIR/home/floatos/.xinitrc"

cat > "$TARGET_DIR/etc/xdg/openbox/rc.xml" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<openbox_config xmlns="http://openbox.org/3.4/rc">
  <focus>
    <focusNew>yes</focusNew>
    <followMouse>no</followMouse>
    <focusLast>yes</focusLast>
    <underMouse>no</underMouse>
    <focusDelay>200</focusDelay>
    <raiseOnFocus>no</raiseOnFocus>
  </focus>
  <placement><policy>Smart</policy><center>yes</center></placement>
  <theme>
    <name>FloatOS</name>
    <titleLayout>NLIMC</titleLayout>
    <keepBorder>yes</keepBorder>
    <animateIconify>yes</animateIconify>
  </theme>
  <desktops>
    <number>1</number>
    <firstdesk>1</firstdesk>
    <names><name>FloatOS</name></names>
    <popupTime>0</popupTime>
  </desktops>
  <resize><drawContents>yes</drawContents></resize>
  <keyboard>
    <keybind key="W-Return"><action name="Execute"><command>xterm</command></action></keybind>
    <keybind key="W-e"><action name="Execute"><command>pcmanfm</command></action></keybind>
    <keybind key="W-d"><action name="ToggleShowDesktop"/></keybind>
    <keybind key="W-r"><action name="Execute"><command>xterm -e top</command></action></keybind>
  </keyboard>
  <mouse>
    <context name="Root">
      <mousebind button="Right" action="Press"><action name="ShowMenu"><menu>root-menu</menu></action></mousebind>
    </context>
  </mouse>
  <menu>
    <file>/etc/xdg/openbox/menu.xml</file>
    <hideDelay>200</hideDelay>
    <submenuShowDelay>100</submenuShowDelay>
    <applicationIcons>yes</applicationIcons>
  </menu>
</openbox_config>
EOF

cat > "$TARGET_DIR/etc/xdg/openbox/menu.xml" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<openbox_menu xmlns="http://openbox.org/3.4/menu">
  <menu id="root-menu" label="FloatOS">
    <item label="Files"><action name="Execute"><command>pcmanfm</command></action></item>
    <item label="Web Browser"><action name="Execute"><command>dillo</command></action></item>
    <item label="Wi-Fi / Network Settings"><action name="Execute"><command>xterm -e connmanctl</command></action></item>
    <item label="Terminal"><action name="Execute"><command>xterm</command></action></item>
    <item label="System Monitor"><action name="Execute"><command>xterm -e top</command></action></item>
    <item label="About FloatOS"><action name="Execute"><command>xterm -e cat /usr/share/floatos/README.txt</command></action></item>
    <separator/>
    <item label="Exit Desktop"><action name="Exit"/></item>
  </menu>
</openbox_menu>
EOF

cat > "$TARGET_DIR/usr/share/themes/FloatOS/openbox-3/themerc" <<'EOF'
border.width: 1
border.color: #a6ff4d
window.active.title.bg: flat solid
window.active.title.bg.color: #101713
window.active.label.text.color: #f4f8f3
window.inactive.title.bg: flat solid
window.inactive.title.bg.color: #111514
window.inactive.label.text.color: #9aa49b
window.active.button.unpressed.image.color: #a6ff4d
window.inactive.button.unpressed.image.color: #6e796f
window.active.grip.bg.color: #a6ff4d
window.inactive.grip.bg.color: #38423b
window.active.handle.bg.color: #101713
window.inactive.handle.bg.color: #111514
menu.items.bg: flat solid
menu.items.bg.color: #101713
menu.items.text.color: #edf2ed
menu.items.active.bg: flat solid
menu.items.active.bg.color: #a6ff4d
menu.items.active.text.color: #090d0a
menu.border.color: #303b33
menu.border.width: 1
EOF

cat > "$TARGET_DIR/etc/xdg/gtk-3.0/settings.ini" <<'EOF'
[Settings]
gtk-theme-name=Adwaita-dark
gtk-icon-theme-name=Adwaita
gtk-font-name=Sans 10
gtk-application-prefer-dark-theme=1
EOF

cat > "$TARGET_DIR/usr/share/applications/floatos-files.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=FloatOS Files
Comment=Browse files and folders
Exec=pcmanfm
Icon=system-file-manager
Terminal=false
Categories=System;FileTools;
EOF
cat > "$TARGET_DIR/usr/share/applications/floatos-browser.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=FloatOS Web
Comment=Browse the web
Exec=dillo
Icon=web-browser
Terminal=false
Categories=Network;WebBrowser;
EOF
cat > "$TARGET_DIR/usr/share/applications/floatos-network.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=FloatOS Network Settings
Comment=Manage Ethernet and Wi-Fi connections
Exec=xterm -e connmanctl
Icon=network-wireless
Terminal=false
Categories=Network;Settings;
EOF
cat > "$TARGET_DIR/usr/share/applications/floatos-terminal.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=FloatOS Terminal
Comment=Open a command line
Exec=xterm
Icon=utilities-terminal
Terminal=false
Categories=System;TerminalEmulator;
EOF
cat > "$TARGET_DIR/usr/share/applications/floatos-about.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=About FloatOS
Comment=View information about this FloatOS build
Exec=xterm -e cat /usr/share/floatos/README.txt
Icon=floatos
Terminal=false
Categories=System;
EOF

cat > "$TARGET_DIR/home/floatos/Desktop/Files.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=Files
Comment=Browse files and folders
Exec=pcmanfm
Icon=system-file-manager
Terminal=false
Categories=System;FileTools;
EOF
cat > "$TARGET_DIR/home/floatos/Desktop/Web Browser.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=Web Browser
Comment=Browse the web
Exec=dillo
Icon=web-browser
Terminal=false
Categories=Network;WebBrowser;
EOF
cat > "$TARGET_DIR/home/floatos/Desktop/Network Settings.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=Network Settings
Comment=Manage Ethernet and Wi-Fi connections
Exec=xterm -e connmanctl
Icon=network-wireless
Terminal=false
Categories=Network;Settings;
EOF
cat > "$TARGET_DIR/home/floatos/Desktop/Terminal.desktop" <<'EOF'
[Desktop Entry]
Type=Application
Name=Terminal
Comment=Open a command line
Exec=xterm
Icon=utilities-terminal
Terminal=false
Categories=System;TerminalEmulator;
EOF

# Generate the FloatOS black-and-lime desktop wallpaper (host-side Python).
python3 - "$TARGET_DIR/usr/share/backgrounds/floatos/floatos-wallpaper.ppm" <<'PY'
import math, sys
path=sys.argv[1]
w,h=1280,720
cx,cy=930,295
pixels=bytearray()
glyphs={
"F":["11111","10000","11110","10000","10000","10000","10000"],
"L":["10000","10000","10000","10000","10000","10000","11111"],
"O":["01110","10001","10001","10001","10001","10001","01110"],
"A":["01110","10001","10001","11111","10001","10001","10001"],
"T":["11111","00100","00100","00100","00100","00100","00100"],
"S":["01111","10000","10000","01110","00001","00001","11110"]}
for y in range(h):
    for x in range(w):
        d=math.hypot((x-cx)*0.92,(y-cy)*1.05)
        glow=max(0.0,1.0-d/840.0)
        ring=max((max(0,30-int(abs(d-r)*6)) for r in (265,215,165)),default=0)
        orbit=28 if 1.55<math.atan2(y-cy,x-cx)<2.5 and abs(d-305)<3 else 0
        pixels.extend((5+int(glow*2),8+int(glow*15)+ring+orbit,7+int(glow*3)))
scale,sx,sy=8,78,500
for n,ch in enumerate("FLOATOS"):
    for gy,line in enumerate(glyphs[ch]):
        for gx,bit in enumerate(line):
            if bit=="1":
                for yy in range(scale):
                    for xx in range(scale):
                        x,y=sx+n*48+gx*scale+xx,sy+gy*scale+yy
                        if x<w and y<h:
                            p=(y*w+x)*3
                            pixels[p:p+3]=bytes((166,255,77) if ch=="F" else (238,244,238))
with open(path,"wb") as f:
    f.write(f"P6\n{w} {h}\n255\n".encode("ascii"))
    f.write(pixels)
PY
chmod 0644 "$TARGET_DIR/usr/share/backgrounds/floatos/floatos-wallpaper.ppm"

cat > "$TARGET_DIR/usr/share/floatos/README.txt" <<'EOF'
FloatOS 0.1
===========

Independent x86-64 Linux live system built from upstream source with Buildroot.
No Fedora, Bazzite, Ubuntu or other distribution is used as a parent base.

Desktop: X.Org + Openbox + PCManFM
Browser: Dillo
Theme: charcoal, white and FloatOS lime (#A6FF4D)

This live preview does not install to or repartition internal disks.
EOF
