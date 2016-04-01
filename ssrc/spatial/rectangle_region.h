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
 * This header defines the rectangle_region class.
 */

#ifndef __SSRC_SPATIAL_RECTANGLE_REGION_H
#define __SSRC_SPATIAL_RECTANGLE_REGION_H

#include <ssrc/libssrckdtree-packages.h>

#include <tuple>

__BEGIN_NS_SSRC_SPATIAL

/**
 * A rectangle_region implements the region concept for rectangular
 * regions defined by a lower left-hand corner and an upper right-hand
 * corner.  The region concept is not yet fully defined and may change
 * in a future version.  Currently, a region must define a bounding
 * box consisting of two public member variables&mdash;lower and
 * upper&mdash;which specify the lower left and upper right corners
 * of the box.  In addition, a public non-static member function
 * must be defined with with the signature: bool contains(const Point &)
 *
 * Region classes are intended to allow iteration over ranges within
 * arbitrarily shaped regions without post-filtering a rectangular
 * range, reducing the total number of comparisions.  Such range
 * searches can be performed with the
 * kd_tree::begin<Region>(const Region &) and kd_tree<Region>::end() functions.
 * rectangle_region is the default region type used by kd_tree for
 * range searches.
 */
template<typename Point,
         const unsigned int i = NS_TR1::tuple_size<Point>::value - 1>
struct rectangle_region {
  Point lower, upper;

  //rectangle_region() = default;
  rectangle_region() : lower(), upper() { }

  rectangle_region(const Point & lower, const Point & upper) :
    lower(lower), upper(upper)
  { }

  // This is a helper function that is NOT part of the region concept.
  static bool
  contains(const Point & point, const Point & lower, const Point & upper)
  {
    return ((NS_TR1::get<i>(point) >= NS_TR1::get<i>(lower) &&
             NS_TR1::get<i>(point) <= NS_TR1::get<i>(upper)) &&
            rectangle_region<Point, i-1>::contains(point, lower, upper));
  }

  /**
   * Returns true if a given point is contained within the region.  The
   * point must lie within or on the boundary defining the region.
   *
   * @param point The point to test.
   * @return True if a given point is contained within the region, false
   *         otherwise.
   */
  bool contains(const Point & point) const {
    return rectangle_region::contains(point, lower, upper);
  }
};

template<typename Point>
struct rectangle_region<Point, 0> {
  Point lower, upper;

  //rectangle_region() = default;
  rectangle_region() : lower(), upper() { }

  rectangle_region(const Point & lower, const Point & upper) :
    lower(lower), upper(upper)
  { }

  // This is a helper function that is NOT part of the region concept.
  static bool
  contains(const Point & point, const Point & lower, const Point & upper) {
    return !(NS_TR1::get<0>(point) < NS_TR1::get<0>(lower) ||
             NS_TR1::get<0>(point) > NS_TR1::get<0>(upper));
  }


  bool contains(const Point & point) const {
    return rectangle_region::contains(point, lower, upper);
  }
};

template<typename Point>
struct rectangle_region<Point, 1> {
  Point lower, upper;

  //rectangle_region() = default;
  rectangle_region() : lower(), upper() { }

  rectangle_region(const Point & lower, const Point & upper) :
    lower(lower), upper(upper)
  { }

  // This is a helper function that is NOT part of the region concept.
  static bool
  contains(const Point & point, const Point & lower, const Point & upper) {
    return !(NS_TR1::get<0>(point) < NS_TR1::get<0>(lower) ||
             NS_TR1::get<1>(point) < NS_TR1::get<1>(lower) ||
             NS_TR1::get<0>(point) > NS_TR1::get<0>(upper) ||
             NS_TR1::get<1>(point) > NS_TR1::get<1>(upper));
  }

  bool contains(const Point & point) const {
    return rectangle_region::contains(point, lower, upper);
  }
};

template<typename Point>
struct rectangle_region<Point, 2> {
  Point lower, upper;

  //rectangle_region() = default;
  rectangle_region() : lower(), upper() { }

  rectangle_region(const Point & lower, const Point & upper) :
    lower(lower), upper(upper)
  { }

  // This is a helper function that is NOT part of the region concept.
  static bool
  contains(const Point & point, const Point & lower, const Point & upper) {
    return !(NS_TR1::get<0>(point) < NS_TR1::get<0>(lower) ||
             NS_TR1::get<1>(point) < NS_TR1::get<1>(lower) ||
             NS_TR1::get<2>(point) < NS_TR1::get<2>(lower) ||
             NS_TR1::get<0>(point) > NS_TR1::get<0>(upper) ||
             NS_TR1::get<1>(point) > NS_TR1::get<1>(upper) ||
             NS_TR1::get<2>(point) > NS_TR1::get<2>(upper));
  }

  bool contains(const Point & point) const {
    return rectangle_region::contains(point, lower, upper);
  }
};

__END_NS_SSRC_SPATIAL

#endif
