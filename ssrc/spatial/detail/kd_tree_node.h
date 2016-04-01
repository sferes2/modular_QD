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

#ifndef __SSRC_SPATIAL_DETAIL_KDTREE_NODE_H
#define __SSRC_SPATIAL_DETAIL_KDTREE_NODE_H

#include <ssrc/libssrckdtree-packages.h>

__BEGIN_NS_SSRC_SPATIAL

namespace detail {

  template<typename TreeTraits>
  class kd_tree_node {
  public:
    typedef TreeTraits traits;
    typedef typename traits::key_type key_type;
    typedef typename traits::mapped_type mapped_type;
    typedef typename traits::value_type value_type;
    typedef typename traits::pointer pointer;
    typedef typename traits::const_pointer const_pointer;
    typedef typename traits::reference reference;
    typedef typename traits::const_reference const_reference;
    typedef typename traits::discriminator_type discriminator_type;
    typedef typename traits::node_type node_type;

    discriminator_type discriminator;
    // Convention we use is that values < discriminator are in low
    // subtree and those >= are in high subtree.
    node_type *child_low;
    node_type *child_high;

  private:
    value_type _pair;

  public:

    kd_tree_node(const discriminator_type discriminator,
                 const key_type & point, const mapped_type & value) :
      discriminator(discriminator), child_low(0), child_high(0),
      _pair(point,value)
    { }

    ~kd_tree_node() {
      delete child_low;
      delete child_high;
    }

    const key_type & point() const {
      return _pair.first;
    }

    mapped_type & value() {
      return _pair.second;
    }

    const mapped_type & value() const {
      return _pair.second;
    }

    reference pair() {
      return _pair;
    }

    const_reference pair() const {
      return _pair;
    }
  };

}

__END_NS_SSRC_SPATIAL

#endif
