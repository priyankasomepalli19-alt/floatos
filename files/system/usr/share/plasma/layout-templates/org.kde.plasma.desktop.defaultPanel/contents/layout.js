// FloatOS first-login desktop layout for KDE Plasma 6.
// Plasma runs the defaultPanel template when a fresh desktop profile is created.
// Existing user layouts are not overwritten during upgrades.

var plasma = getApiVersion(1);

function floatosAddWidget(panel, pluginId) {
    if (knownWidgetTypes.indexOf(pluginId) !== -1) {
        return panel.addWidget(pluginId);
    }
    print("FloatOS: optional Plasma widget not available: " + pluginId);
    return null;
}

// Apply FloatOS wallpaper to each available desktop.
var wallpaperUrl = "file:///usr/share/backgrounds/floatos/floatos-wallpaper.svg";
var desktopList = desktops();

for (var d = 0; d < desktopList.length; d++) {
    var desktop = desktopList[d];
    desktop.wallpaperPlugin = "org.kde.image";
    desktop.currentConfigGroup = ["Wallpaper", "org.kde.image", "General"];
    desktop.writeConfig("Image", wallpaperUrl);
    desktop.writeConfig("PreviewImage", wallpaperUrl);
}

// Top menu and system-status bar.
var top = new Panel();
top.location = "top";
top.lengthMode = "fill";
top.height = 34;
top.alignment = "left";
top.floating = false;
top.hiding = "none";

var launcher = floatosAddWidget(top, "org.kde.plasma.kickoff");
if (launcher) {
    launcher.currentConfigGroup = ["General"];
    launcher.writeConfig("icon", "floatos");
}

// Global application menu is optional; applications without support still work.
floatosAddWidget(top, "org.kde.plasma.appmenu");

// Keep status indicators and clock aligned to the right.
floatosAddWidget(top, "org.kde.plasma.panelspacer");
floatosAddWidget(top, "org.kde.plasma.systemtray");

var clock = floatosAddWidget(top, "org.kde.plasma.digitalclock");
if (clock) {
    clock.currentConfigGroup = ["Appearance"];
    clock.writeConfig("showDate", "true");
    clock.writeConfig("showSeconds", "false");
}

// Centered floating dock.
var dock = new Panel();
dock.location = "bottom";
dock.lengthMode = "fit";
dock.alignment = "center";
dock.height = 66;
dock.floating = true;
dock.hiding = "dodgewindows";

var tasks = floatosAddWidget(dock, "org.kde.plasma.icontasks");
if (tasks) {
    tasks.currentConfigGroup = ["General"];
    tasks.writeConfig(
        "launchers",
        "applications:org.kde.dolphin.desktop,applications:org.kde.konsole.desktop,applications:com.valvesoftware.Steam.desktop,applications:org.kde.discover.desktop,applications:systemsettings.desktop"
    );
    tasks.writeConfig("maxStripes", "1");
    tasks.writeConfig("showOnlyCurrentDesktop", "false");
    tasks.writeConfig("showOnlyCurrentScreen", "false");
}

floatosAddWidget(dock, "org.kde.plasma.showdesktop");

// Float KRunner over the desktop when invoked.
var runner = ConfigFile("krunnerrc");
runner.group = "General";
runner.writeEntry("FreeFloating", true);

print("FloatOS: initial Plasma layout created.");
