/*
 * Copyright 2003-2005 Daniel F. Savarese
 * Copyright 2006-2009 Savarese Software Research Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.savarese.com/software/ApacheLicense-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * This header defines the macros for the package namespaces.  These
 * macros are for use internal to the library.  Client code should
 * refer only to ssrc:: prefixed namespaces (e.g., ssrc::spatial)
 *
 * This header also includes libssrckdtree-config.h so that all modules pick
 * up the platform-specific feature definitions since every module
 * ultimately includes this header.
 */

#ifndef __SSRC_KD_PACKAGES_H
#define __SSRC_KD_PACKAGES_H

#include <ssrc/libssrckdtree-config.h>

// Begin namespace definitions.

#define NS_SSRC_SPATIAL KD_DEFINE_NAMESPACE(spatial)

#define NS_KD_INTERNAL_DECL_PREFIX NS_KD_DECL_PREFIX
#define NS_KD_INTERNAL_DECL_SUFFIX NS_KD_DECL_SUFFIX

#define NS_KD_EXTERNAL_DECL_PREFIX \
namespace ssrc {
#define NS_KD_EXTERNAL_DECL_SUFFIX \
}

#define __END_NS_KD \
    } \
NS_KD_DECL_SUFFIX

#define __END_NS_KD2 \
    } \
  } \
NS_KD_DECL_SUFFIX

#define KD_BEGIN_NS(name) \
  NS_KD_DECL_PREFIX \
    namespace name {

#define KD_BEGIN_NS2(name1,name2) \
  NS_KD_DECL_PREFIX \
    namespace name1 { \
      namespace name2 {

#define __BEGIN_NS_SSRC_SPATIAL  KD_BEGIN_NS(spatial)
#define __END_NS_SSRC_SPATIAL __END_NS_KD

// End namespace definitions.

// Document namespaces

NS_KD_INTERNAL_DECL_PREFIX

/**  
 * The spatial namespace houses data structures and algorithms for
 * spatial data structures and concerns itself primarily with range
 * searching.
 */
namespace spatial { }

NS_KD_INTERNAL_DECL_SUFFIX

// Create aliases

/**
 * The ssrc namespace is an alias intended for use by library users.
 * The alternate namespaces prefixed by kd_vX_X_X contain
 * versioning information internal to the library and should not be used
 * outside of it.  For example, you should use the namespace ssrc::spatial
 * instead of kd_vX_X_X::spatial.
 */
NS_KD_EXTERNAL_DECL_PREFIX

  namespace spatial = NS_SSRC_SPATIAL;

NS_KD_EXTERNAL_DECL_SUFFIX

// End namespace aliases

#endif
