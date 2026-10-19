# COPR "make srpm" entry point: make -f .copr/Makefile srpm outdir=<dir> spec=<spec>
# Runs as root in a Fedora mock chroot, so build tools can be installed here.
outdir ?= $(CURDIR)
topdir := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
# COPR passes the Spec File field possibly empty, anchored with "/", or already joined to the checkout.
specfile := $(or $(wildcard $(filter /%,$(spec))),$(topdir)/$(patsubst /%,%,$(or $(spec),packaging/rpm/fedora/fcitx5-ngosen.spec)))

srpm:
	dnf -y install git-core rpm-build
	git config --global --add safe.directory $(topdir)
	# The Rust core is vendored in-tree; the Go submodule is not needed.
	set -e; \
	version=$$(rpmspec -q --srpm --qf '%{version}' $(specfile)); \
	sources=$$(mktemp -d); \
	git -C $(topdir) archive --format=tar.gz \
		--prefix=fcitx5-ngosen-$$version/ \
		-o $$sources/fcitx5-ngosen-$$version.tar.gz HEAD; \
	rpmbuild -bs --define "_sourcedir $$sources" --define "_srcrpmdir $(outdir)" \
		$(specfile)

.PHONY: srpm
