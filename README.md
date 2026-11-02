# Pla8ma open/close animation for KDE Plasma

> **Info:** highly AI-generated.

An open/close effect for KDE Plasma 6, forked from KWin's Glide effect:
the window tilts in 3D while it fades in or out.

![Opening and closing a window](docs/openclose.webp)

4 times slower:

![Opening and closing a window, slowed down](docs/openclose-slow.webp)

## Install

Needs a compiler and KWin's development files.

Fedora:

```bash
sudo dnf install cmake extra-cmake-modules gcc-c++ kwin-devel libepoxy-devel libdrm-devel
```

Arch Linux:

```bash
sudo pacman -S --needed base-devel cmake extra-cmake-modules kwin vulkan-headers
```

Ubuntu / Kubuntu 26.04 LTS (Plasma 6.6, use the `kde6.6` branch):

```bash
sudo apt install build-essential cmake extra-cmake-modules kwin-dev libkf6coreaddons-dev \
    qt6-base-dev qt6-base-private-dev libepoxy-dev libdrm-dev
```

Then build and install it for your user:

```bash
./install.sh
```

The script switches the effect on and turns off Fade, Scale and Glide, as
only one open/close animation can run at a time. You can change this in
System Settings > Window Management > Desktop Effects.

If KWin does not pick it up right away, log out and back in.

**After every KWin update, run `./install.sh` again.** KWin only loads
effects built against its exact version.

## Packages

Instead of `install.sh`, you can build a package of the newest state of
`main`:

- Fedora: [`packaging/fedora/pla8ma-kwineffect-openclose.spec`](packaging/fedora/pla8ma-kwineffect-openclose.spec),
  the steps are at the top of the file.
- Arch Linux: [`packaging/arch/PKGBUILD`](packaging/arch/PKGBUILD), run
  `makepkg -si` in its folder.

The package installs the effect for all users but does not switch it on:
do that in System Settings > Window Management > Desktop Effects.
Rebuild the package after every KWin update, as with `install.sh`; until
then KWin leaves the effect off.

## Plasma versions

Use the branch that matches your Plasma version:

| Branch | Plasma |
|---|---|
| `main` | newest (currently 6.8) |
| `kde6.8` | 6.8 |
| `kde6.7` | 6.7 |
| `kde6.6` | 6.6 |

## Uninstall

```bash
./uninstall.sh
```

This removes the effect and switches KDE's Scale effect back on.

## Credits

Forked from the Glide effect of [KWin](https://invent.kde.org/plasma/kwin)
([source](https://invent.kde.org/plasma/kwin/-/tree/master/src/plugins/glide)),
by Philip Falkner, Martin Gräßlin, Alexandre Pereira and Vlad Zahorodnii.
Thanks to them and the KDE community.

## License

GPL-2.0-or-later, like KWin's Glide effect it is forked from.
