package OpenSSL::safe::installdata;

use strict;
use warnings;
use Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw(
    @PREFIX
    @libdir
    @BINDIR @BINDIR_REL_PREFIX
    @LIBDIR @LIBDIR_REL_PREFIX
    @INCLUDEDIR @INCLUDEDIR_REL_PREFIX
    @APPLINKDIR @APPLINKDIR_REL_PREFIX
    @ENGINESDIR @ENGINESDIR_REL_LIBDIR
    @MODULESDIR @MODULESDIR_REL_LIBDIR
    @PKGCONFIGDIR @PKGCONFIGDIR_REL_LIBDIR
    @CMAKECONFIGDIR @CMAKECONFIGDIR_REL_LIBDIR
    $VERSION @LDLIBS
);

our @PREFIX                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3' );
our @libdir                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3' );
our @BINDIR                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/apps' );
our @BINDIR_REL_PREFIX          = ( 'apps' );
our @LIBDIR                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3' );
our @LIBDIR_REL_PREFIX          = ( '' );
our @INCLUDEDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/include', '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/include' );
our @INCLUDEDIR_REL_PREFIX      = ( 'include', './include' );
our @APPLINKDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/ms' );
our @APPLINKDIR_REL_PREFIX      = ( 'ms' );
our @ENGINESDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/engines' );
our @ENGINESDIR_REL_LIBDIR      = ( 'engines' );
our @MODULESDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/providers' );
our @MODULESDIR_REL_LIBDIR      = ( 'providers' );
our @PKGCONFIGDIR               = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3' );
our @PKGCONFIGDIR_REL_LIBDIR    = ( '.' );
our @CMAKECONFIGDIR             = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3' );
our @CMAKECONFIGDIR_REL_LIBDIR  = ( '.' );
our $VERSION                    = '3.5.3';
our @LDLIBS                     =
    # Unix and Windows use space separation, VMS uses comma separation
    $^O eq 'VMS'
    ? split(/ *, */, '-ldl -pthread -latomic ')
    : split(/ +/, '-ldl -pthread -latomic ');

1;
