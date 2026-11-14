// Standalone FloatOS centered floating dock.
var dock = new Panel();
dock.location = "bottom";
dock.lengthMode = "fit";
dock.alignment = "center";
dock.height = 66;
dock.floating = true;
dock.hiding = "dodgewindows";

if (knownWidgetTypes.indexOf("org.kde.plasma.icontasks") !== -1) {
    var tasks = dock.addWidget("org.kde.plasma.icontasks");
    tasks.currentConfigGroup = ["General"];
    tasks.writeConfig(
        "launchers",
        "applications:org.kde.dolphin.desktop,applications:org.kde.konsole.desktop,applications:com.valvesoftware.Steam.desktop,applications:org.kde.discover.desktop,applications:systemsettings.desktop"
    );
    tasks.writeConfig("maxStripes", "1");
}

if (knownWidgetTypes.indexOf("org.kde.plasma.showdesktop") !== -1) {
    dock.addWidget("org.kde.plasma.showdesktop");
}
