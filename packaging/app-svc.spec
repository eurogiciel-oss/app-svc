Name:	    app-svc
Summary:    Application Service
Version: 0.1.50
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
cp %{SOURCE1001} .


%build
%cmake .
make %{?jobs:-j%jobs}

%install
%make_install

# Create database
mkdir -p %{buildroot}/opt/dbspace
sqlite3 %{buildroot}/opt/dbspace/.appsvc.db < data/appsvc_db.sql

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%license LICENSE
%manifest app-svc.manifest
%defattr(-,root,root,-)
%config(noreplace) %verify(not md5 mtime size) %attr(664,0,5000) /opt/dbspace/.appsvc.db
%config(noreplace) %verify(not md5 mtime size) %attr(664,0,5000) /opt/dbspace/.appsvc.db-journal
/usr/bin/appsvc_test
%{_libdir}/libappsvc.so.0
%{_libdir}/libappsvc.so.0.1.0

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/appsvc.pc
%{_libdir}/libappsvc.so
/usr/include/appsvc/appsvc.h


