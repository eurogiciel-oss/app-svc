Name:	    app-svc
Summary:    Application Service
Version: 0.1.50
Release:    1
Group:      Application Framework/Service
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(post): sqlite3
Requires(postun): /sbin/ldconfig
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore) 
BuildRequires: pkgconfig(x11)
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


%build
%cmake .
make %{?jobs:-j%jobs}

%install
%make_install


%post
/sbin/ldconfig

sqlite3 /opt/dbspace/.appsvc.db < /opt/share/appsvc_db.sql
rm -rf /opt/share/appsvc_db.sql

chown 0:5000 /opt/dbspace/.appsvc.db
chown 0:5000 /opt/dbspace/.appsvc.db-journal
chmod 664 /opt/dbspace/.appsvc.db
chmod 664 /opt/dbspace/.appsvc.db-journal

%postun -p /sbin/ldconfig

%files
%license LICENSE
%manifest app-svc.manifest
%defattr(-,root,root,-)
/opt/share/appsvc_db.sql
/usr/bin/appsvc_test
%{_libdir}/libappsvc.so.0
%{_libdir}/libappsvc.so.0.1.0

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/appsvc.pc
%{_libdir}/libappsvc.so
/usr/include/appsvc/appsvc.h


