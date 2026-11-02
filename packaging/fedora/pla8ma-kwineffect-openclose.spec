# Builds the newest state of the main branch:
#   sudo dnf install rpm-build rpmdevtools
#   spectool -g -R pla8ma-kwineffect-openclose.spec
#   sudo dnf builddep pla8ma-kwineffect-openclose.spec
#   rpmbuild -bb pla8ma-kwineffect-openclose.spec
#   sudo dnf install ~/rpmbuild/RPMS/x86_64/pla8ma-kwineffect-openclose-*.rpm
#
# KWin only loads effects built against its exact version, so the package
# requires exactly the KWin it was built with: dnf holds KWin back until
# the package is rebuilt.

%global kwin_version %(rpm -q --qf '%%{VERSION}' kwin-devel 2>/dev/null | grep -E '^[0-9.]+$' || echo 0)
%global snapdate %(date +%Y%m%d)

Name:           pla8ma-kwineffect-openclose
Version:        0^%{snapdate}
Release:        1.kwin%{kwin_version}%{?dist}
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

Requires:       kwin-libs%{?_isa} = %{kwin_version}

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
* Mon Nov 02 2026 Pla8ma <johndoe@example.com> - 0^20261102-1
- Snapshot of the main branch
