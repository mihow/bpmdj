Summary: BpmDj - Free DJ tools. 
Vendor: http://bpmdj.sourceforge.net/
Name: cBpmDj 
Version: _VERSION_ 
Release: 2
Source: http://bpmdj.sourceforge.net/%{name}-%{version}.tar.gz
Packager: Thomas Steudten <thomas@steudten.com>
Copyright: GPL
Group: Applications/Multimedia 
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildRequires: gzip  
Requires: mpg123  

%define strip_binaries 1
%define gzip_man 1
%define  __prefix /usr

Prefix: %{__prefix} 

%description
BpmDj: Free Dj tools, contains a _very_ accurate BPM Counter and a player which
can nudge, change the tempo and shift the song by measures. Persistent cue's,
multiple dsp's are supported. The index format is readable text.

%prep
%setup  

%build 
%configure
make 

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make ROOT=$RPM_BUILD_ROOT install

%if %{strip_binaries}
{ cd $RPM_BUILD_ROOT
  strip .%{__prefix}/bin/* || /bin/true
}
%endif
%if %{gzip_man}
{ cd $RPM_BUILD_ROOT
}
%endif

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{__prefix}/bin/*
%doc AUTHORS COPYRIGHT ChangeLog README TODO INSTALL index.html

%changelog

* Sun Mar 10 2002  Thomas Steudten <thomas@steudten.com>
- Fix Summary line.

* Fri Jan 25 2002 Thomas Steudten <thomas@steudten.com>
- Rel. 1: First package version
