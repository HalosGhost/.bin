#!/usr/bin/env bash

msg () { printf "$1\n"; }
die () { msg "failed to $1"; exit 1; }

msg 'Set locale'
echo 'en_US.UTF-8 UTF-8' > /etc/locale.gen || die 'specify locale'
locale-gen || die 'generate locale'
echo 'LANG=en_US.UTF-8' > /etc/locale.conf || die 'set $LANG'

bootconf='/boot/syslinux/syslinux.conf'

msg 'Setting up Syslinux'
sed -e 's/sda3/sda1/g' -i "$bootconf" || die 'set root to sda1'
sed -e 's/vmlinuz-linux/&-grsec/g' -i "$bootconf" || die 'use grsec kernel'
syslinux-install_update -iam || die 'install syslinux to MBR'

msg 'Setting up network'
systemctl enable systemd-networkd || die 'enable networkd'
systemctl enable systemd-resolved || die 'enable resolved'

ipaddr="$(ip a show dev ens33 | awk '/inet/ { print $2 }')"
read -r -d '' hosts_file << EOF
#
# /etc/hosts: static lookup table for host names
#

#<ip-address>   <hostname.domain.org>   <hostname>
127.0.0.1       localhost.localdomain   localhost
::1             localhost.localdomain   localhost
$ipaddr         $1.halosgho.st          $1

# End of file
EOF

msg 'Setting up hostname'
echo "$1" > /etc/hostname || die 'write hostname'
echo "$hosts_file" > /etc/hosts || die 'write hosts'

