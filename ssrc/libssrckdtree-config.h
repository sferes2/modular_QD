/* automatically generated */
#ifndef _LIBSSRCKDTREE_CONFIG_H
#define _LIBSSRCKDTREE_CONFIG_H 1
 
/* libssrckdtree-config.h. Generated automatically at end of configure. */
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Enable debug code. */
/* #undef LIBSSRCKDTREE_DEBUG */

/* Boost is available. */
#ifndef LIBSSRCKDTREE_HAVE_BOOST 
#define LIBSSRCKDTREE_HAVE_BOOST  1 
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_DLFCN_H 
#define LIBSSRCKDTREE_HAVE_DLFCN_H  1 
#endif

/* Define if the compiler supports extended friend declarations. */
/* #undef LIBSSRCKDTREE_HAVE_EXTENDED_FRIEND_DECLARATIONS */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_INTTYPES_H 
#define LIBSSRCKDTREE_HAVE_INTTYPES_H  1 
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_MEMORY_H 
#define LIBSSRCKDTREE_HAVE_MEMORY_H  1 
#endif

/* Define if the compiler implements namespaces. */
#ifndef LIBSSRCKDTREE_HAVE_NAMESPACES 
#define LIBSSRCKDTREE_HAVE_NAMESPACES  1 
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_STDINT_H 
#define LIBSSRCKDTREE_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_STDLIB_H 
#define LIBSSRCKDTREE_HAVE_STDLIB_H  1 
#endif

/* Define if std::tuple_size is present. */
#ifndef LIBSSRCKDTREE_HAVE_STD_TUPLE 
#define LIBSSRCKDTREE_HAVE_STD_TUPLE  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_STRINGS_H 
#define LIBSSRCKDTREE_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_STRING_H 
#define LIBSSRCKDTREE_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_SYS_STAT_H 
#define LIBSSRCKDTREE_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_SYS_TYPES_H 
#define LIBSSRCKDTREE_HAVE_SYS_TYPES_H  1 
#endif

/* Define if std::tr1::tuple_size is present. */
/* #undef LIBSSRCKDTREE_HAVE_TR1_TUPLE */

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef LIBSSRCKDTREE_HAVE_UNISTD_H 
#define LIBSSRCKDTREE_HAVE_UNISTD_H  1 
#endif

/* Name of package */
#ifndef LIBSSRCKDTREE_PACKAGE 
#define LIBSSRCKDTREE_PACKAGE  "libssrckdtree" 
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef LIBSSRCKDTREE_PACKAGE_BUGREPORT 
#define LIBSSRCKDTREE_PACKAGE_BUGREPORT  "" 
#endif

/* Define to the full name of this package. */
#ifndef LIBSSRCKDTREE_PACKAGE_NAME 
#define LIBSSRCKDTREE_PACKAGE_NAME  "libssrckdtree" 
#endif

/* Define to the full name and version of this package. */
#ifndef LIBSSRCKDTREE_PACKAGE_STRING 
#define LIBSSRCKDTREE_PACKAGE_STRING  "libssrckdtree 1.0.8" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef LIBSSRCKDTREE_PACKAGE_TARNAME 
#define LIBSSRCKDTREE_PACKAGE_TARNAME  "libssrckdtree" 
#endif

/* Define to the version of this package. */
#ifndef LIBSSRCKDTREE_PACKAGE_VERSION 
#define LIBSSRCKDTREE_PACKAGE_VERSION  "1.0.8" 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef LIBSSRCKDTREE_STDC_HEADERS 
#define LIBSSRCKDTREE_STDC_HEADERS  1 
#endif

/* Version number of package */
#ifndef LIBSSRCKDTREE_VERSION 
#define LIBSSRCKDTREE_VERSION  "1.0.8" 
#endif


#if !defined(NS_KD)
#  define NS_KD kd_v1_0_8
#endif

#if !defined(KD_DEFINE_NAMESPACE)
#  define KD_DEFINE_NAMESPACE(name) NS_KD::name
#endif

#if !defined(NS_KD_DECL_PREFIX)
#  define NS_KD_DECL_PREFIX \
namespace kd_v1_0_8 {
#endif

#if !defined(NS_KD_DECL_SUFFIX)
#  define NS_KD_DECL_SUFFIX \
}
#endif

#if defined(LIBSSRCKDTREE_HAVE_STD_TUPLE)
#  define NS_TR1 std
#elif defined(LIBSSRCKDTREE_HAVE_TR1_TUPLE)
#  define NS_TR1 std::tr1
#endif

 
/* _LIBSSRCKDTREE_CONFIG_H */
#endif
