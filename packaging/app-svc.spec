%bcond_with x
%bcond_with wayland

Name:	    app-svc
Summary:    Application Service
Version: 0.1.53
Release:    1
Group:      Application Framework/Service
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: 	app-svc.manifest

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires: cmake
BuildRequires: sqlite3
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore) 
%if %{with x}
BuildRequires: pkgconfig(x11)
BuildRequires:  pkgconfig(ecore-x)
%endif
BuildRequires: pkgconfig(libprivilege-control)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(xdgmime)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(libsoup-2.4)
BuildRequires: pkgconfig(iniparser)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(libtzplatform-config)


%description
Application Service

%package devel
Summary:    App svc
Group:      Development/Application Framework
Requires:   %{name} = %{version}-%{release}
%description devel
%devel_desc

%prep
%setup -q
cp %{SOURCE1001} .

%build
%cmake . \
%if %{with wayland} && !%{with x}
-Dwith_wayland=TRUE
%else
-Dwith_x=TRUE
%endif

make %{?jobs:-j%jobs}

%install
%make_install

# Create database
mkdir -p %{buildroot}%{TZ_SYS_DB}
sqlite3 %{buildroot}%{TZ_SYS_DB}/.appsvc.db < data/appsvc_db.sql

mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%manifest %{name}.manifest
%license LICENSE
%config(noreplace) %verify(not md5 mtime size) %attr(664,root,%{TZ_SYS_USER_GROUP}) %{TZ_SYS_DB}/.appsvc.db
%config(noreplace) %verify(not md5 mtime size) %attr(664,root,%{TZ_SYS_USER_GROUP}) %{TZ_SYS_DB}/.appsvc.db-journal
/usr/bin/appsvc_test
%{_libdir}/libappsvc.so.0
%{_libdir}/libappsvc.so.0.1.0
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{_libdir}/pkgconfig/appsvc.pc
%{_libdir}/libappsvc.so
%{_includedir}/appsvc/appsvc.h


