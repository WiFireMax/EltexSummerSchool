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

our @PREFIX                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install' );
our @libdir                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/lib' );
our @BINDIR                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/bin' );
our @BINDIR_REL_PREFIX          = ( 'bin' );
our @LIBDIR                     = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/lib' );
our @LIBDIR_REL_PREFIX          = ( 'lib' );
our @INCLUDEDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/include' );
our @INCLUDEDIR_REL_PREFIX      = ( 'include' );
our @APPLINKDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/include/openssl' );
our @APPLINKDIR_REL_PREFIX      = ( 'include/openssl' );
our @ENGINESDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/lib/engines-3' );
our @ENGINESDIR_REL_LIBDIR      = ( 'engines-3' );
our @MODULESDIR                 = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/lib/ossl-modules' );
our @MODULESDIR_REL_LIBDIR      = ( 'ossl-modules' );
our @PKGCONFIGDIR               = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/lib/pkgconfig' );
our @PKGCONFIGDIR_REL_LIBDIR    = ( 'pkgconfig' );
our @CMAKECONFIGDIR             = ( '/home/firemax/Eltex/EltexSummerSchool/Practika19/openssl-3.5.3/_install/lib/cmake/OpenSSL' );
our @CMAKECONFIGDIR_REL_LIBDIR  = ( 'cmake/OpenSSL' );
our $VERSION                    = '3.5.3';
our @LDLIBS                     =
    # Unix and Windows use space separation, VMS uses comma separation
    $^O eq 'VMS'
    ? split(/ *, */, '-ldl -pthread -latomic ')
    : split(/ +/, '-ldl -pthread -latomic ');

1;
