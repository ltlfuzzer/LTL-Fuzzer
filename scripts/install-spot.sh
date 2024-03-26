wget -q -O - https://www.lrde.epita.fr/repo/debian.gpg |apt-key add -
echo 'deb http://www.lrde.epita.fr/repo/debian/ stable/' >> /etc/apt/sources.list
apt update
apt install -y spot libspot-dev libgtest-dev
