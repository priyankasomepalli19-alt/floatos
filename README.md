# FloatOS

**FloatOS is an independently assembled x86-64 Linux operating system project.** Its toolchain, kernel, userspace and bootable image are assembled from source with Buildroot. It does not use Fedora, Bazzite, Ubuntu, or another distribution as its base.

## FloatOS 0.1

The first bootable development image includes:

- Linux 6.18.55 kernel for x86-64 PCs.
- A Buildroot-generated userspace using BusyBox init and glibc.
- X.Org, Openbox and a native GTK3-powered FloatOS desktop shell, with PCManFM and Xterm as system apps.
- FloatOS charcoal, white and lime-green (#A6FF4D) visual identity.
- FloatOS boot menu, orbital charcoal/lime wallpaper, and circular FloatOS application icon.
- Animated FloatOS app launcher, centered floating dock, top status bar, quick settings and animated settings pages.
- AMD Radeon graphics firmware and Realtek 8822CE rtw88 Wi-Fi firmware.
- ConnMan for Ethernet and Wi-Fi management, with a desktop Network Settings shortcut.
- A live desktop that starts automatically after boot.

This is a **live preview ISO**, not a disk installer. It boots into FloatOS without partitioning or modifying your internal drive. Files and settings created during a live session are temporary and are lost when you shut down.

## Download

After a successful GitHub Actions build, the ISO and SHA-256 checksum are published here:

https://github.com/priyankasomepalli19-alt/floatos/releases/tag/floatos-latest

GitHub Actions also retains the downloadable build artifact for 14 days.

## Windows ISO download

When the `floatos-latest` release has been published, Windows users can download and verify it with the included helper:

1. Download [download-floatos.bat](https://raw.githubusercontent.com/priyankasomepalli19-alt/floatos/main/download-floatos.bat).
2. Run the downloaded `download-floatos.bat` file.
3. The ISO is saved to `%USERPROFILE%\Downloads\FloatOS` and its SHA-256 is checked automatically.

From **Windows CMD**, this single command downloads the helper and starts it:

```cmd
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $p=Join-Path $env:USERPROFILE 'Downloads\download-floatos.bat'; Invoke-WebRequest -UseBasicParsing -Uri 'https://raw.githubusercontent.com/priyankasomepalli19-alt/floatos/main/download-floatos.bat' -OutFile $p; & $p"
```

The helper tries to install the official portable aria2 using WinGet when needed, then downloads with up to eight connections and resume support. If WinGet or aria2 is unavailable, it falls back to Windows' built-in curl. It verifies the ISO's SHA-256 before reporting success.

## Build

The Actions workflow downloads Buildroot 2026.08.1, loads `floatos_x86_64_defconfig`, builds the kernel and selected packages, creates a BIOS/UEFI ISO, checks it with xorriso and publishes it.

Buildroot limits each package to two compiler jobs. The experimental top-level parallel mode is intentionally disabled for reliability.

To build locally on Linux:

```sh
make BR2_EXTERNAL="$PWD" O="$PWD/output" floatos_x86_64_defconfig
make BR2_EXTERNAL="$PWD" O="$PWD/output"
```

## Safety

- Boot the ISO from a USB or virtual machine for testing.
- The live ISO does not install itself to a hard drive.
- Do not use an installer or partitioning tool on your Windows drive.
- Use a separate test disk if a future FloatOS installer is added.

## Project structure

- `configs/`: FloatOS Buildroot configuration.
- `board/floatos/`: bootloader, kernel fragment, users and root filesystem setup.
- `package/floatos-shell/`: FloatOS's native desktop shell and GTK theme.
- `.github/workflows/build.yml`: source build and ISO publishing pipeline.
