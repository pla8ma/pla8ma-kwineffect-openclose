#!/usr/bin/env bash
# Removes the open/close effect again and switches KDE's Scale back on.
set -uo pipefail

ID="kwineffect_openclose"

kwriteconfig6 --file kwinrc --group Plugins --key "${ID}Enabled" --delete
for other in fade scale glide; do
    kwriteconfig6 --file kwinrc --group Plugins --key "${other}Enabled" --delete
done
gdbus call --session -d org.kde.KWin -o /Effects -m org.kde.kwin.Effects.unloadEffect "$ID" >/dev/null
rm -f "$HOME/.local/lib64/qt6/plugins/kwin/effects/plugins/$ID.so"
gdbus call --session -d org.kde.KWin -o /Effects -m org.kde.kwin.Effects.loadEffect scale >/dev/null
# The env file adding ~/.local to Qt's plugin path stays: other parts use it too.
