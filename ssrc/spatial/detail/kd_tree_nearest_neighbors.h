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

#ifndef __SSRC_SPATIAL_DETAIL_KDTREE_NEAREST_NEIGHBORS_H
#define __SSRC_SPATIAL_DETAIL_KDTREE_NEAREST_NEIGHBORS_H

#include <ssrc/spatial/detail/coordinate_limits.h>
#include <ssrc/spatial/distance.h>

#include <vector>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/shared_container_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

__BEGIN_NS_SSRC_SPATIAL

namespace detail {

  template<typename TreeTraits, typename Distance>
  struct kd_tree_nn_pq_value {
    typedef TreeTraits traits;
    typedef Distance distance_type;
    typedef typename traits::node_type node_type;

    distance_type distance;
    node_type *node;

    kd_tree_nn_pq_value(const distance_type distance = distance_type(0),
                        node_type * const node = 0) :
      distance(distance), node(node)
    { }

    friend bool operator<(const kd_tree_nn_pq_value & v1,
                          const kd_tree_nn_pq_value & v2)
    {
      return (v1.distance < v2.distance);
    }
  };

  template<typename TreeTraits, typename Distance>
  class kd_tree_nearest_neighbors {
    typedef TreeTraits traits;
    typedef typename traits::key_type key_type;
    typedef typename traits::node_type node_type;
    typedef typename traits::discriminator_type discriminator_type;
    typedef Distance distance_type;
    typedef kd_tree_nn_pq_value<traits, distance_type> pq_value_type;
    typedef std::vector<pq_value_type> pq_type;
    typedef boost::shared_container_iterator<pq_type> shared_iterator;

    mutable bool _omit_query_point;
    mutable unsigned int _num_neighbors;
    mutable distance_type _min_distance;
    mutable pq_type *_pq;
    mutable key_type _query;

    struct get_node_pair {
      typedef
      typename kd_tree_nearest_neighbors::traits::const_reference result_type;
      typedef kd_tree_nearest_neighbors::pq_value_type pq_value_type;

      result_type operator()(const pq_value_type  & v) const {
        return v.node->pair();
      }
    };

    void push(const pq_value_type & v) const {
      _pq->push_back(v);
      std::push_heap(_pq->begin(), _pq->end());
    }

    void pop() const {
      std::pop_heap(_pq->begin(), _pq->end());
      _pq->pop_back();
    }

    const pq_value_type & top() const {
      return _pq->front();
    }

    void find(node_type * const node) const {
      if(node == 0)
        return;

      const discriminator_type discriminator = node->discriminator;
      const key_type & point = node->point();
      distance_type d2 = euclidean_distance<key_type>::d2(_query, point);

      if(d2 < _min_distance && (d2 != 0 || !_omit_query_point)) {
        if(_pq->size() == _num_neighbors) {
          pop();
          push(pq_value_type(d2, node));
          _min_distance = top().distance;
        } else {
          push(pq_value_type(d2, node));
          if(_pq->size() == _num_neighbors) {
            _min_distance = top().distance;
          }
        }
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
    typedef
    boost::transform_iterator<get_node_pair, shared_iterator>
    iterator;

    kd_tree_nearest_neighbors() { }

    std::pair<iterator,iterator> find(node_type * const root,
                                      const key_type & query,
                                      const unsigned int num_neighbors,
                                      const bool omit_query_point) const
    {
      boost::shared_ptr<pq_type> neighbors(new pq_type());
      _pq = neighbors.get();
      _pq->reserve(num_neighbors);

      _omit_query_point = omit_query_point;
      _num_neighbors = num_neighbors;
      _min_distance = coordinate_limits<distance_type>::highest();
      _query = query;

      if(num_neighbors > 0) {
        find(root);
      }

      std::sort_heap(_pq->begin(), _pq->end());
      _pq = 0;

     
      return
        std::pair<iterator,iterator>(
                  iterator(shared_iterator(neighbors->begin(), neighbors),
                           get_node_pair()),
                  iterator(shared_iterator(neighbors->end(), neighbors),
                           get_node_pair()));

    }
  };

}

__END_NS_SSRC_SPATIAL

#endif
