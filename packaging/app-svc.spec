Name:	    app-svc
Summary:    App svc
Version:	0.1.18
Release:    1.1
Group:      System/Libraries
License:    SAMSUNG
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
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
%description
App svc

%package -n libapp-svc
Summary:    App svc Library
Group:      Development/Libraries
%description -n libapp-svc
App svc (developement files)

%package -n libapp-svc-devel
Summary:    App svc Library Development files
Group:      Development/Libraries
Requires:   libapp-svc = %{version}-%{release}
%description -n libapp-svc-devel
App svc (developement files)

%prep
%setup -q


%build

CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" cmake . -DCMAKE_INSTALL_PREFIX=/usr

make %{?jobs:-j%jobs}

%install
%make_install


%post -n libapp-svc
/sbin/ldconfig
mkdir -p /opt/dbspace
sqlite3 /opt/dbspace/.appsvc.db < /opt/share/appsvc_db.sql
rm -rf /opt/share/appsvc_db.sql

chown root:5000 /opt/dbspace/.appsvc.db
chown root:5000 /opt/dbspace/.appsvc.db-journal
chmod 664 /opt/dbspace/.appsvc.db
chmod 664 /opt/dbspace/.appsvc.db-journal

%postun -n libapp-svc -p /sbin/ldconfig
rm -f /opt/dbspace/.appsvc.db
rm -f /opt/dbspace/.appsvc.db-journal


%files
/usr/bin/appsvc_test

%files -n libapp-svc
%config(missingok) /opt/share/appsvc_db.sql
/usr/lib/libappsvc.so.0
/usr/lib/libappsvc.so.0.1.0

%files -n libapp-svc-devel
/usr/lib/pkgconfig/appsvc.pc
/usr/lib/libappsvc.so
/usr/include/appsvc/appsvc.h
