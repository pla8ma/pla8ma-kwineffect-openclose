#!/usr/bin/env bash
# Builds and installs the open/close effect for KWin for the current user
# and switches it on in place of the other open/close animations.
# Needs: cmake extra-cmake-modules gcc-c++ kwin-devel
# The plugin only works with the KWin it was built against: run this
# again after every KWin update.
set -euo pipefail

cd "$(dirname "$0")"

ID="kwineffect_pla8ma_openclose"
PREFIX="$HOME/.local"
PLUGINDIR="lib64/qt6/plugins"
TARGET="$PREFIX/$PLUGINDIR/kwin/effects/plugins"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build -j"$(nproc)"

# Replace the file instead of overwriting it: KWin may have the old one
# loaded.
mkdir -p "$TARGET"
cp build/bin/kwin/effects/plugins/"$ID".so "$TARGET/.$ID.so.new"
mv -f "$TARGET/.$ID.so.new" "$TARGET/$ID.so"

# KWin only finds plugins in Qt's plugin path: add ~/.local for future
# logins ...
ENV_DIR="$HOME/.config/plasma-workspace/env"
mkdir -p "$ENV_DIR"
cat > "$ENV_DIR/pla8ma-kwineffects.sh" <<'EOF'
# Added by the Pla8ma KWin effect installer.
case ":$QT_PLUGIN_PATH:" in
    *":$HOME/.local/lib64/qt6/plugins:"*) ;;
    *) export QT_PLUGIN_PATH="$HOME/.local/lib64/qt6/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}" ;;
esac
EOF

# One open/close animation at a time (as the effect settings do).
kwriteconfig6 --file kwinrc --group Plugins --key "${ID}Enabled" true
for other in fade scale glide; do
    kwriteconfig6 --file kwinrc --group Plugins --key "${other}Enabled" false
done

# Swap the effects in the running KWin.
effects() {
    gdbus call --session -d org.kde.KWin -o /Effects -m "org.kde.kwin.Effects.$1" "$2" >/dev/null 2>&1 || true
}
for other in fade scale glide; do
    effects unloadEffect "$other"
done
effects unloadEffect "$ID"
loaded=$(gdbus call --session -d org.kde.KWin -o /Effects -m org.kde.kwin.Effects.loadEffect "$ID" 2>/dev/null || true)

echo
if [ "$loaded" = "(true,)" ]; then
    echo "Installed and switched on (System Settings > Window Management > Desktop Effects)."
else
    echo "Installed. KWin did not load it now (it has no ~/.local in its plugin path yet,"
    echo "or an older build is still loaded): log out and back in."
fi
