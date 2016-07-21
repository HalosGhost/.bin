#!/usr/bin/env bash

msg () { printf '%s\n' "$1"; }

# This should only be used for scripts simple or no control flow
trap_err() { ret=$?; printf '%s\n' "${BASH_SOURCE[0]}:$LINENO: $BASH_COMMAND ($ret)"; exit $ret; }
trap 'trap_err' ERR

msg "Verifying we have the necessary tools"
hash curl unsquashfs

msg "Enter mirror to use: "
read -r -p 'Enter mirror to use: ' mirror
msg "using mirror $mirror"

date="$(date -u +'%Y.%m.01')"
iso=archlinux-"$date"-dual.iso
arch=x86_64

msg "Downloading ISO from $mirror"
curl -Cf - -#O "$mirror/iso/$date/$iso"

msg "Mounting ISO to /mnt"
mount -o loop "$iso" /mnt

msg "Copying squashfs"
cp -v /mnt/arch/"$arch"/airootfs.* "$PWD"

msg "Verifying squashfs"
md5sum -c airootfs.md5

msg "Unmounting ISO"
umount -v /mnt

msg "Extracting Squashfs"
unsquashfs airootfs.sfs

msg "\nCleaning old airootfs files"
rm -v -- airootfs.*

msg "Making new_root target"
mkdir -pv new_root

nbytes="$(($(du -s squashfs-root|cut -f1)+100000))K"
msg "Creating tmpfs of $nbytes bytes"
mount -v -o size="$nbytes" -t tmpfs none ./new_root

msg "Moving live files to new_root"
for i in squashfs-root/*; do
    cp -rv "$i" ./new_root/
done; rm -rv -- squashfs-root

msg "Creating location for old_root"
mkdir -p new_root/old_root

msg "Copying ethernet info"
ip a >> new_root/root/ifcfgeth
ip r >> new_root/root/ifcfgeth

msg 'Grabbing second stage script'
curl 'https://raw.githubusercontent.com/HalosGhost/.bin/master/cac/stage_2.sh' \
    -s#o -f new_root/root/stage_2.sh

msg 'Changing permissions of second stage script'
chmod 0755 new_root/root/stage_2.sh

msg "Making old_root rprivate"
mount -v --make-rprivate /

msg "Preemptively modprobing ext4 and xfs"
modprobe -v ext4
modprobe -v xfs

msg "Pivoting root"
pivot_root new_root new_root/old_root
cd

msg "Moving old mounts"
for i in {dev,run,sys,proc}; do
    mount -v --move /old_root/"$i" /"$i"
done

msg "Restarting daemons"
systemctl daemon-reexec

msg "Turning swapoff"
swapoff -a

msg "Press enter to kill pids"
msg "Note: run ./stage_2.sh to finish the intial setup"
msg "before continuing with the standard Arch install"
read -r

msg "Killing pids holding old_root"
fuser -k -m /old_root
