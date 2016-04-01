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

#ifndef __SSRC_SPATIAL_DETAIL_KDTREE_NEAREST_NEIGHBOR_H
#define __SSRC_SPATIAL_DETAIL_KDTREE_NEAREST_NEIGHBOR_H

#include <ssrc/spatial/detail/coordinate_limits.h>
#include <ssrc/spatial/distance.h>

__BEGIN_NS_SSRC_SPATIAL

namespace detail {

  template<typename TreeTraits, typename Distance>
  class kd_tree_nearest_neighbor {
    typedef TreeTraits traits;
    typedef typename traits::key_type key_type;
    typedef typename traits::node_type node_type;
    typedef typename traits::discriminator_type discriminator_type;
    typedef Distance distance_type;

    mutable bool _omit_query_point;
    mutable distance_type _min_distance;
    mutable node_type *_nearest;
    mutable key_type _query;

    void find(node_type * const node) const {
      if(node == 0)
        return;

      const discriminator_type discriminator = node->discriminator;
      const key_type & point = node->point();
      distance_type d2 = euclidean_distance<key_type>::d2(_query, point);

      if(d2 < _min_distance && (d2 != 0 || !_omit_query_point)) {
        _min_distance = d2;
        _nearest = node;
      }

      // Distance to partition plane.
      const distance_type dp =
        static_cast<distance_type>(_query[discriminator]) -
        static_cast<distance_type>(point[discriminator]);

      d2 = dp*dp;

      if(dp < 0) {
        find(node->child_low);
        if(d2 < _min_distance) {
          find(node->child_high);
        }
      } else {
        find(node->child_high);
        if(d2 < _min_distance) {
          find(node->child_low);
        }
      }
    }

  public:

    kd_tree_nearest_neighbor() { }

    node_type * find(node_type * const root, const key_type & query,
                     const bool omit_query_point) const
    {
      _omit_query_point = omit_query_point;
      _min_distance = coordinate_limits<distance_type>::highest();
      _nearest = 0;
      _query = query;

      find(root);

      return _nearest;
    }
  };

}

__END_NS_SSRC_SPATIAL

#endif
