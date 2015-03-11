autoreconf -i
diff -Naur $HOME/.config/feednix/config.json ./config.json > patchfile
patch $HOME/.config/feednix/config.json < patchfile
sudo mkdir -p /etc/xdg/feednix && sudo cp config.json /etc/xdg/feednix
