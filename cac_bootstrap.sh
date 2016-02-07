#!/usr/bin/env bash

mirror=http://archlinux.surlyjake.com
date=2016.02.01
iso=archlinux-"$date"-dual.iso
arch=x86_64

die () { printf "failed to $1\n"; exit 1; }
msg () { printf "$1\n"; }

msg "Downloading ISO from $mirror"
curl -O "$mirror/archlinux/iso/$date/$iso" || die "download iso"

msg "Mounting ISO to /mnt"
mount -o loop "$iso" /mnt || die "mount iso"

msg "Copying squashfs"
cp -v /mnt/arch/"$arch"/airootfs.* . || die "copy squashfs"

msg "Verifying squashfs"
md5sum -c airootfs.md5 || die "verify squashfs"

msg "Unmounting ISO"
umount /mnt || die "unmount iso"

msg "Deleting ISO"
rm -- "$iso" || die "delete ISO"

msg "Extracting Squashfs"
unsquashfs airootfs.sfs || die "extract squashfs"

msg "\nCleaning old airootfs files"
rm -- airootfs.* || die "clean up old airootfs files"

msg "Making new_root target"
mkdir -p new_root || die "create new_root target"

nbytes="$(du -hs squashfs-root|cut -f1)"
msg "Creating tmpfs of $nbytes bytes"
mount -o size="$nbytes" -t tmpfs none ./new_root || die "create tmpfs"

msg "Moving live files to new_root"
for i in squashfs-root/*; do
    cp -r "$i" ./new_root/ || die "move $i to new_root"
done; rm -r -- squashfs-root || die "clean up squashfs-root dir"

msg "Creating location for old_root"
mkdir -p new_root/old_root || die "create old_root"

msg "Copying ethernet config"
cp /mnt2/ifcfgeth new_root/opt/ || die "copy ethernet config"

msg "Making old_root rprivate"
mount --make-rprivate / || die "make old_root rprivate"

msg "Preemptively modprobing ext4 and xfs"
modprobe ext4 || die "modprobe ext4"
modprobe xfs || die "modprobe xfs"

msg "Pivoting root"
pivot_root new_root new_root/old_root || die "pivot root"
cd || die "cd to new root"

msg "Moving old mounts"
for i in {dev,run,sys,proc}; do
    mount --move /old_root/"$i" /"$i" || die "move $i"
done

msg "Restarting daemons"
systemctl daemon-reexec || die "restarting daemons"

msg "Killing pids holding old_root"
fuser -k -m /old_root || die "kill pids holding old_root"

msg "Unmounting old_root"
umount -R /old_root || die "unmount old_root"
