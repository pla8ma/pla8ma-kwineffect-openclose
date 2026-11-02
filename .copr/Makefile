# For COPR, build method "make srpm": packs the checked-out commit as the
# source the spec expects and builds the source RPM from it.

NAME := pla8ma-kwineffect-openclose
TOP := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
outdir ?= $(TOP)/.copr/out

srpm:
	dnf -y install git rpm-build
	mkdir -p $(outdir)
	git -c safe.directory='*' -C $(TOP) archive --format=tar.gz \
		--prefix=$(NAME)-main/ -o $(outdir)/$(NAME)-main.tar.gz HEAD
	rpmbuild -bs --define "_sourcedir $(outdir)" --define "_srcrpmdir $(outdir)" \
		$(TOP)/packaging/fedora/$(NAME).spec
