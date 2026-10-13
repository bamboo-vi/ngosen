#!/bin/bash
# Gives a next branch build a version that sorts above the release it starts from and below the
# next one, with the commit time and hash in it: 1.2.2-1 becomes 1.2.2-1.202610121530.git8cb4b89.
# Testers can then install it over a release and get the next release as a normal upgrade.
set -euo pipefail

cd "$(dirname "$0")"
stamp=$(TZ=UTC git log -1 --format=%cd --date=format-local:%Y%m%d%H%M)
hash=$(git rev-parse --short=7 HEAD)
suffix="$stamp.git$hash"

# The release number must be followed by a number: rpm and dpkg sort "1.fc44" and "1.dev" below "1".
sed -i "s/^\(Release: *[0-9]*\)/\1.$suffix/" rpm/fedora/fcitx5-ngosen.spec rpm/opensuse/fcitx5-ngosen.spec
sed -i "1s/(\([^)]*\))/(\1.$suffix)/" debian/changelog
# pkgrel takes only digits, so the stamp goes on pkgver.
sed -i "s/^pkgver=.*/&.$suffix/" arch/PKGBUILD

grep -H '^Release:' rpm/fedora/fcitx5-ngosen.spec rpm/opensuse/fcitx5-ngosen.spec
head -1 debian/changelog
grep -H '^pkgver=' arch/PKGBUILD
