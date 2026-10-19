#!/bin/bash
# Builds the signed pacman repository "ngosen" from one release's Arch package. pacman only
# offers the newest version of a package anyway, so the repository holds just that one.
# Signs with the default secret key in the caller's GnuPG home. Needs repo-add from pacman.
set -euo pipefail

src=${1:?usage: build-arch-repo.sh <packages-dir> <output-dir>}
out=${2:?usage: build-arch-repo.sh <packages-dir> <output-dir>}

pkgs=$(find "$src" -maxdepth 1 -type f -name 'fcitx5-ngosen-*-x86_64.pkg.tar.zst' | sort)
if [ "$(printf '%s\n' "$pkgs" | grep -c .)" != 1 ]; then
    echo "expected exactly one fcitx5-ngosen x86_64 package in $src, found: ${pkgs:-none}" >&2
    exit 1
fi

rm -rf "$out"
dir=$out/x86_64
mkdir -p "$dir"
cp "$pkgs" "$dir/"
pkg=$dir/$(basename "$pkgs")
gpg --batch --yes --detach-sign -o "$pkg.sig" "$pkg"
repo-add --sign --verify "$dir/ngosen.db.tar.gz" "$pkg"

# repo-add links ngosen.db to ngosen.db.tar.gz; Pages does not serve symlinks.
for link in "$dir"/*; do
    if [ -L "$link" ]; then
        cp --remove-destination "$(readlink -f "$link")" "$link"
    fi
done
rm -f "$dir"/*.old "$dir"/*.old.sig

gpg --batch --armor --export > "$out/ngosen.asc"
