#!/bin/bash
# Imports the repository signing subkey from $APT_SIGNING_KEY (the release environment secret).
# The APT and pacman repositories are signed with the same key.
set -euo pipefail

if [ -z "${APT_SIGNING_KEY:-}" ]; then
    echo "::error::APT_SIGNING_KEY is not set in the release environment"
    exit 1
fi
printf '%s\n' "$APT_SIGNING_KEY" | gpg --batch --import
