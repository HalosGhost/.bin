#!/usr/bin/env bash

die () { printf "failed to $1\n"; exit 1; }
msg () { printf "$1\n"; }

msg 'Unmounting old_root'
umount -R /old_root || die 'unmount old_root'

msg 'Cleaning up old_root'
rm -rf /old_root || die 'clean up old_root'

msg 'Killing LVM'
vgremove -ff centos || die 'kill LVM'
