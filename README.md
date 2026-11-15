# FloatOS

**FloatOS is an independently assembled x86-64 Linux operating system project.** Its toolchain, kernel, userspace and bootable image are assembled from source with Buildroot. It does not use Fedora, Bazzite, Ubuntu, or another distribution as its base.

## FloatOS 0.1

The first bootable development image includes:

- Linux 6.18.55 kernel for x86-64 PCs.
- A Buildroot-generated userspace using BusyBox init and glibc.
- X.Org, Openbox, PCManFM file manager and Xterm.
- FloatOS charcoal, white and lime-green (#A6FF4D) visual identity.
- FloatOS boot menu, orbital charcoal/lime wallpaper, and circular FloatOS application icon.
- Top status panel and a centered bottom launcher panel.
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
3. The ISO is saved to `%USERPROFILE%\\Downloads\\FloatOS` and its SHA-256 is checked automatically.

The helper uses aria2 multi-connection downloads when aria2 is already installed; otherwise it uses Windows' built-in curl with resume and retries.

## Build

The Actions workflow downloads Buildroot 2026.08.1, loads `floatos_x86_64_defconfig`, builds the kernel and selected packages, creates a BIOS/UEFI ISO, checks it with xorriso and publishes it.

To build locally on Linux:

```sh
make BR2_EXTERNAL="$PWD" O="$PWD/output" floatos_x86_64_defconfig
make BR2_EXTERNAL="$PWD" O="$PWD/output" -j2
```

## Safety

- Boot the ISO from a USB or virtual machine for testing.
- The live ISO does not install itself to a hard drive.
- Do not use an installer or partitioning tool on your Windows drive.
- Use a separate test disk if a future FloatOS installer is added.

## Project structure

- `configs/`: FloatOS Buildroot configuration.
- `board/floatos/`: bootloader, kernel fragment, users and root filesystem setup.
- `.github/workflows/build.yml`: source build and ISO publishing pipeline.
