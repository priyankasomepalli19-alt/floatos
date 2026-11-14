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
        "preferred://browser,applications:steam.desktop,applications:net.lutris.Lutris.desktop,applications:org.kde.konsole.desktop,applications:io.github.kolunmi.Bazaar.desktop,preferred://filemanager,applications:systemsettings.desktop"
    );
    tasks.writeConfig("maxStripes", "1");
}

if (knownWidgetTypes.indexOf("org.kde.plasma.showdesktop") !== -1) {
    dock.addWidget("org.kde.plasma.showdesktop");
}
