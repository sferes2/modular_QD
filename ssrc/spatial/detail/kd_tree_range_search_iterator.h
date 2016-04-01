/* Copyright 2003-2005 Daniel F. Savarese
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

#ifndef __SSRC_SPATIAL_DETAIL_KDTREE_RANGE_SEARCH_ITERATOR_H
#define __SSRC_SPATIAL_DETAIL_KDTREE_RANGE_SEARCH_ITERATOR_H

#include <ssrc/libssrckdtree-packages.h>

#include <iterator>
#include <stack>
#include <array>

__BEGIN_NS_SSRC_SPATIAL

namespace detail {

  template<typename TreeTraits, typename Region>
  class kd_tree_range_search_iterator :
    public
    std::iterator<std::forward_iterator_tag,typename TreeTraits::value_type>
  {
  public:
    typedef TreeTraits traits;
    typedef typename traits::node_type node_type;
    typedef typename traits::key_type key_type;
    typedef typename traits::value_type value_type;
    typedef typename traits::pointer pointer;
    typedef typename traits::const_pointer const_pointer;
    typedef typename traits::reference reference;
    typedef typename traits::const_reference const_reference;
    typedef typename traits::discriminator_type discriminator_type;
    typedef Region region_type;

  private:
#ifdef LIBSSRCKDTREE_HAVE_EXTENDED_FRIEND_DECLARATIONS
    friend traits::const_iterator;
    friend traits::tree_type;
#else
    friend class traits::const_iterator;
    friend class traits::tree_type;
#endif
    typedef std::stack<node_type *> stack_type;

    node_type *_node;
    stack_type _stack;
    discriminator_type _discriminator;
    region_type _region;

    void advance() {
      while(!_stack.empty()) {
        node_type *node = _stack.top();
        _stack.pop();

        _discriminator = node->discriminator;

        // <= instead of < because values >= are in right subtree.
        if(node->point()[_discriminator] <= _region.upper[_discriminator] &&
           node->child_high != 0)
          _stack.push(node->child_high);

        // Push left subtree last so that it will be searched first
        if(node->point()[_discriminator] > _region.lower[_discriminator] &&
           node->child_low != 0)
          _stack.push(node->child_low);

        if(_region.contains(node->point())) {
          _node = node;
          return;
        }
      }
      _node = 0;
    }

    void advance_to_lower() {
      while(!_stack.empty()) {
        node_type *node = _stack.top();
        _stack.pop();

        _discriminator = node->discriminator;

        // <= instead of < because values >= are in right subtree.
        if(node->point()[_discriminator] <= _region.upper[_discriminator] &&
           node->child_high != 0)
          _stack.push(node->child_high);

        // Push left subtree last so that it will be searched first.
        if(node->point()[_discriminator] > _region.lower[_discriminator] &&
           node->child_low != 0)
          _stack.push(node->child_low);

        if(node->point() == _region.lower) {
          _node = node;
          return;
        }
      }
      _node = 0;
    }

  public:
    // For now, we rely on default assignment operator and for
    // const_iterator the default copy constructor.  They are
    // unacceptable only if Discriminator, Point, or Region types allocate and
    // destroy memory referenced by pointers, which for now we forbid.

    explicit kd_tree_range_search_iterator(node_type * const node = 0) :
      _node(node), _stack(), _discriminator(), _region()
    { }

    // Copy constructor to allow const_iterator to be created from iterator
    kd_tree_range_search_iterator(const typename traits::iterator & rsi) :
      _node(rsi._node), _stack(rsi._stack), _discriminator(rsi._discriminator),
      _region(rsi._region)
      { }

    kd_tree_range_search_iterator(const region_type & region,
                                  node_type* const root,
                                  bool find = false) :
      _node(root), _stack(), _discriminator(), _region(region)
    {
      if(_node != 0) {
        _stack.push(_node);
        if(!find)
          advance();
        else
          advance_to_lower();
      }
    }

    bool end_of_range() const {
      return (_node == 0);
    }

    reference operator*() {
      return _node->pair();
    }

    const_reference operator*() const {
      return _node->pair();
    }

    pointer operator->() {
      return &(_node->pair());
    }

    const_pointer operator->() const {
      return &(_node->pair());
    }

    kd_tree_range_search_iterator & operator++() {
      advance();
      return *this;
    }

    kd_tree_range_search_iterator operator++(int) {
      kd_tree_range_search_iterator old(*this);
      advance();
      return old;
    }

    bool operator==(const kd_tree_range_search_iterator & rsi) const {
      return (_node == rsi._node);
    }

    bool operator!=(const kd_tree_range_search_iterator & rsi) const {
      return (_node != rsi._node);
    }
  };

}

__END_NS_SSRC_SPATIAL

#endif
