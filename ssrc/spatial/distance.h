/*
 * Copyright 2010 Savarese Software Research Corporation
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
 * This header defines distance functions.
 */

#ifndef __SSRC_SPATIAL_DISTANCE_H
#define __SSRC_SPATIAL_DISTANCE_H

#include <ssrc/libssrckdtree-packages.h>

#include <tuple>

__BEGIN_NS_SSRC_SPATIAL

/**
 * euclidean_distance is a template class that houses functions for
 * determining distances and distance-related values in a Euclidean
 * space.  These functions are static and rely on the enclosing class
 * to enable implementation of partial specializations.
 */
template<typename Point,
         typename Distance = double,
         const unsigned int i = NS_TR1::tuple_size<Point>::value - 1>
struct euclidean_distance {
  typedef Distance distance_type;

  /**
   * Returns the square of the distance between two points.
   *
   * @param from The first end point.
   * @param to The second end point.
   * @return The square of the distance between from and to.
   */
  static distance_type d2(const Point & from, const Point & to) {
    const distance_type d =
      static_cast<distance_type>(to[i]) - static_cast<distance_type>(from[i]);

    return (d*d) + euclidean_distance<Point, distance_type, i-1>::d2(from, to);
  }
};

template<typename Point, typename Distance>
struct euclidean_distance<Point, Distance, 0> {
  typedef Distance distance_type;

  static distance_type d2(const Point & from, const Point & to) {
    const distance_type d0 =
      static_cast<distance_type>(to[0]) - static_cast<distance_type>(from[0]);

    return (d0*d0);
  }
};

template<typename Point, typename Distance>
struct euclidean_distance<Point, Distance, 1> {
  typedef Distance distance_type;

  static distance_type d2(const Point & from, const Point & to) {
    const distance_type d0 =
      static_cast<distance_type>(to[0]) - static_cast<distance_type>(from[0]);
    const distance_type d1 =
      static_cast<distance_type>(to[1]) - static_cast<distance_type>(from[1]);

    return (d0*d0) + (d1*d1);
  }
};

template<typename Point, typename Distance>
struct euclidean_distance<Point, Distance, 2> {
  typedef Distance distance_type;

  static distance_type d2(const Point & from, const Point & to) {
    const distance_type d0 =
      static_cast<distance_type>(to[0]) - static_cast<distance_type>(from[0]);
    const distance_type d1 =
      static_cast<distance_type>(to[1]) - static_cast<distance_type>(from[1]);
    const distance_type d2 =
      static_cast<distance_type>(to[2]) - static_cast<distance_type>(from[2]);

    return (d0*d0) + (d1*d1) + (d2*d2);
  }
};

__END_NS_SSRC_SPATIAL

#endif
