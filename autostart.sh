#!/bin/sh
exec <&-

pipewire &
pipewire-pulse &
wireplumber &
wlsunset -l 39.9 -L 116.4 &
kanshi &
fcitx5 -d &
$HOME/.local/bin/dim &
wl-paste --watch cliphist store &
foot -s &
lf -server &
$HOME/.local/bin/setbg $HOME/.local/share/wallpaper &
dbus-update-activation-environment DISPLAY WAYLAND_DISPLAY XDG_CURRENT_DESKTOP=dwl
