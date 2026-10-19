#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
#
# COPR runs "make -f .copr/Makefile srpm spec=..." with the Spec File field either as typed in its
# settings or already joined to the checkout as an absolute path; both must name the same spec.
set -u
top=${1:?usage: copr-makefile.sh <source dir>}
makefile=$top/.copr/Makefile
[ -f "$makefile" ] || exit 77
want=$top/packaging/rpm/fedora/fcitx5-ngosen.spec
# Other distributions' package builds do not ship the Fedora spec.
[ -f "$want" ] || exit 77
fail=0
for spec in "" packaging/rpm/fedora/fcitx5-ngosen.spec /packaging/rpm/fedora/fcitx5-ngosen.spec "$want"; do
    cmds=$(make -n -f "$makefile" srpm outdir=/nonexistent spec="$spec")
    case "$cmds" in
    *"rpmspec -q --srpm --qf '%{version}' $want)"*) ;;
    *)
        echo "spec='$spec' does not resolve to $want:" >&2
        printf '%s\n' "$cmds" | grep rpmspec >&2
        fail=1
        ;;
    esac
done
exit $fail
