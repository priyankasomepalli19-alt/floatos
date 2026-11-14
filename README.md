# FloatOS

FloatOS is a custom Bazzite-based Linux desktop project. It keeps Bazzite's gaming-focused Fedora Atomic foundation and adds FloatOS identity, KDE defaults, desktop layout, and boot branding.

## Current development status

- Base: Bazzite stable, KDE desktop image.
- Branding: FloatOS OS release metadata, dark/lime KDE color scheme, Papirus Dark icons, and FloatOS wallpaper.
- Desktop: Plasma first-login layout with a top status panel and centered floating dock.
- Boot: FloatOS Plymouth splash theme.
- Login: FloatOS wallpaper on Plasma Login Manager where supported. Plasma Login Manager uses its native Breeze greeter; this project does not replace it with an unsupported custom QML theme.

## Build the image

1. Open the repository's **Actions** tab.
2. Select the **bluebuild** workflow.
3. Confirm the latest run for the commit you want has completed successfully.
4. The image is published to `ghcr.io/priyankasomepalli19-alt/floatos`.

The main branch build is the recommended image to install. Feature branches are for development and validation.

## Build a bootable installer ISO

1. Merge the tested changes into `main`.
2. Wait for the **bluebuild** workflow on `main` to finish successfully.
3. Open **Actions → Build FloatOS ISO → Run workflow**.
4. Leave `image_tag` as `latest` to build the current main image.
5. When the run succeeds, open it and download the `FloatOS-Installer-ISO` artifact.

The ISO workflow uses BlueBuild's ISO generator with the KDE/Kinoite installer variant. It creates an installer ISO, not a live desktop session. GitHub artifact storage quotas may prevent storing a large ISO; the workflow keeps the artifact for one day to minimize storage use.

## How to test FloatOS safely

### Recommended: test in a virtual machine first

1. Use VMware Workstation or another UEFI-capable virtual machine manager.
2. Create a new Linux/Fedora 64-bit VM with UEFI firmware, 2 virtual CPU cores, and 4 GB RAM (close other apps first on an 8 GB host).
3. Attach the FloatOS ISO as the VM's virtual CD/DVD.
4. Create a virtual disk of at least 50 GB. Use a dynamically allocated disk and store it on a drive with enough free space.
5. Boot the VM from the ISO and install FloatOS **only to that VM's virtual disk**.
6. When installation finishes, disconnect the ISO and boot the VM from its virtual disk.
7. Check the boot splash, login background, top panel, dock, wallpaper, app launcher, file manager, audio, network, and Steam.

The installer ISO is not a live preview. To see the installed FloatOS desktop, install it to a VM virtual disk or a separate spare test drive.

### Testing on physical hardware

- Use a spare USB drive of at least 16 GB for the installer. Writing the ISO erases that USB.
- Keep Windows and your internal NVMe untouched.
- For an installed hardware test, use a separate spare SSD/drive as the installation target. Do not select the Windows disk or any disk containing personal files.
- The USB installer and installation target must be two different drives.

## Safety

Do not run a rebase or installation command against your daily Windows/Bazzite laptop during initial testing. Do not format or repartition the internal SSD. Verify the image build and test it in a virtual machine before considering any physical installation.

## Source and build documentation

- [Bazzite custom image guide](https://docs.bazzite.gg/Advanced/creating_custom_image/)
- [BlueBuild ISO guide](https://blue-build.org/how-to/generate-iso/)
- [BlueBuild recipe reference](https://blue-build.org/reference/recipe/)
