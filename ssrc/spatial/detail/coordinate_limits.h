/*
 * Copyright 2009 Savarese Software Research Corporation
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

#ifndef __SSRC_SPATIAL_DETAIL_COORDINATE_LIMITS_H
#define __SSRC_SPATIAL_DETAIL_COORDINATE_LIMITS_H

#include <ssrc/libssrckdtree-packages.h>

#include <limits>

__BEGIN_NS_SSRC_SPATIAL

namespace detail {

  template<typename coordinate_type, bool is_integer, bool has_infinity>
  struct coordinate_limits_impl;

  template<typename coordinate_type>
  struct coordinate_limits_impl<coordinate_type, true, false> {
    static const coordinate_type highest() {
      return std::numeric_limits<coordinate_type>::max();
    }
    static const coordinate_type lowest() {
      return std::numeric_limits<coordinate_type>::min();
    }
  };

  template<typename coordinate_type>
  struct coordinate_limits_impl<coordinate_type, false, false> {
    static const coordinate_type highest() {
      return std::numeric_limits<coordinate_type>::max();
    }
    static const coordinate_type lowest() {
      // Note, for C++0x, this should be std::numeric_limits::lowest()
      // because not all environments have floating point type implementations
      // where the most negative value is equal to the negative maximum value.
      // But no known compilers yet implement std::numeric_limits::lowest().
      return -std::numeric_limits<coordinate_type>::max();
    }
  };

  template<typename coordinate_type>
  struct coordinate_limits_impl<coordinate_type, false, true> {
    static const coordinate_type highest() {
      return std::numeric_limits<coordinate_type>::infinity();
    }
    static const coordinate_type lowest() {
      return -std::numeric_limits<coordinate_type>::infinity();
    }
  };

  // <coordinate_type, true, true> is a compile-time error.

  template<typename coordinate_type>
  struct coordinate_limits : public coordinate_limits_impl<coordinate_type, std::numeric_limits<coordinate_type>::is_integer, std::numeric_limits<coordinate_type>::has_infinity> { };
}

__END_NS_SSRC_SPATIAL

#endif
