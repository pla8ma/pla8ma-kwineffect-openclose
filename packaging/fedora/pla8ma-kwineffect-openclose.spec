# Builds the newest state of the main branch:
#   sudo dnf install rpm-build rpmdevtools
#   spectool -g -R pla8ma-kwineffect-openclose.spec
#   sudo dnf builddep pla8ma-kwineffect-openclose.spec
#   rpmbuild -bb pla8ma-kwineffect-openclose.spec
#   sudo dnf install ~/rpmbuild/RPMS/x86_64/pla8ma-kwineffect-openclose-*.rpm
#
# KWin only loads effects built against its exact version: after a KWin
# update the effect stays off until the package is rebuilt.

# .copr/Makefile fixes both when making the source RPM: snapdate is the
# commit's date, buildstamp the time of the build, so every rebuild is an
# update for dnf.
%global snapdate %(date +%Y%m%d)
%global buildstamp %(date -u +%Y%m%d%H%M)

Name:           pla8ma-kwineffect-openclose
Version:        0^%{snapdate}
Release:        %{buildstamp}%{?dist}
Summary:        Pla8ma open/close animation for KDE Plasma
License:        GPL-2.0-or-later
URL:            https://github.com/pla8ma/pla8ma-kwineffect-openclose
Source0:        %{url}/archive/refs/heads/main.tar.gz#/%{name}-main.tar.gz

BuildRequires:  cmake
BuildRequires:  extra-cmake-modules
BuildRequires:  gcc-c++
BuildRequires:  kf6-rpm-macros
BuildRequires:  kwin-devel
BuildRequires:  libdrm-devel
BuildRequires:  libepoxy-devel

Requires:       kwin-libs%{?_isa} >= 6.8

%description
An open/close effect for KDE Plasma 6, forked from KWin's Glide effect:
the window tilts in 3D while it fades in or out. Switch it on in System
Settings > Window Management > Desktop Effects.

%prep
%autosetup -n %{name}-main

%build
%cmake_kf6
%cmake_build

%install
%cmake_install

%files
%license LICENSES/GPL-2.0-or-later.txt
%doc README.md
%{_kf6_qtplugindir}/kwin/effects/plugins/kwineffect_pla8ma_openclose.so

%changelog
* Mon Nov 02 2026 Pla8ma <johndoe@example.com>
- Snapshot of the main branch
