#ifndef MAP_ELITES_AGGREGATOR_MAP_HPP
#define MAP_ELITES_AGGREGATOR_MAP_HPP


namespace sferes
{
  namespace aggregator{




    template<typename Phen, typename Params>
    class Map{
    public:
      typedef boost::shared_ptr<Phen> indiv_t;
      typedef typename std::vector<indiv_t> pop_t;
      typedef typename pop_t::iterator it_t;
      typedef typename std::vector<std::vector<indiv_t> > front_t;

      
      static const size_t behav_dim = Params::ea::behav_dim;
      typedef boost::multi_array<indiv_t, behav_dim> array_t;
      typedef typename array_t::multi_array_base::index_range index_range_t;
      typedef boost::detail::multi_array::index_gen<behav_dim, behav_dim> index_gen_t;
      typedef typename array_t::template const_array_view<behav_dim>::type view_t;
      //typedef boost::detail::multi_array::array_view<behav_dim> myview_t;
      
      typedef boost::array<typename array_t::index, behav_dim> behav_index_t;
      
      typedef boost::array<float, behav_dim> point_t;

      behav_index_t behav_shape;
      

      Map()
      {
	assert(behav_dim == Params::ea::behav_shape_size());
        //boost::array<long int, behav_dim> tmp_shape;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
	  behav_shape[i] = Params::ea::behav_shape(i);
		
        //boost::array<typename array_t::index, behav_dim> shape = behav_shape;
        _array.resize(behav_shape);
        _array_parents.resize(behav_shape);
	
        //boost::array<typename array_t::index, behav_dim> shape = {{ 2, 2 }}; //behav_shape
        //allocate space for _array and _array_parents
      }


      // TODO CLEAN THIS MESS. one method with proper usage...
      //-----------------------------------------------
      /*long int getindex(const array_t & m, const indiv_t* requestedElement, const unsigned short int direction) const
      {
        int offset = requestedElement - m.origin();
        return (offset / m.strides()[direction] % m.shape()[direction] +  m.index_bases()[direction]);
      }
      
      behav_index_t getindexarray(const array_t & m, const indiv_t* requestedElement ) const
      {
        behav_index_t _index;
        for (unsigned int dir = 0; dir < behav_dim; dir++ )
	  {
            _index[dir] = getindex( m, requestedElement, dir );
	  }
	
        return _index;
	}*/
      
      
      
      template<typename I>
      point_t get_point(const I& indiv) const
      {
        point_t p;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
	  p[i] = std::min(1.0f, indiv->fit().desc()[i]);

	return p;
      }
      
      
      template<typename I>
      behav_index_t get_index(const I& indiv) const
      {
	point_t p = get_point(indiv);
	behav_index_t behav_pos;
	for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
	  {
	    behav_pos[i] = round(p[i] * behav_shape[i]);
	    behav_pos[i] = std::min(behav_pos[i], behav_shape[i] - 1);
	    assert(behav_pos[i] < behav_shape[i]);
	  }
	return behav_pos;
      }
      
      //-----------------------------------------------


      void get_full_content(std::vector<indiv_t>& content)
      {
	for(const indiv_t* i = _array.data(); i < (_array.data() + _array.num_elements()); ++i)
	  if(*i)
	    content.push_back(*i);
	  
      }

      

      bool add_to_archive(indiv_t i1, indiv_t parent)
      {
        if(i1->fit().dead())
	  return false;
	
        behav_index_t behav_pos = get_index(i1);
	        
        float epsilon = 0.05;
        if (!_array(behav_pos)
	    || (i1->fit().value() - _array(behav_pos)->fit().value()) > epsilon
	    || (fabs(i1->fit().value() - _array(behav_pos)->fit().value()) <= epsilon && _dist_center(i1) < _dist_center(_array(behav_pos))))
	  {
            _array(behav_pos) = i1;
            _array_parents(behav_pos) = parent;
	    parent->fit().set_curiosity(parent->fit().curiosity()+1);
            return true;
	  }
	parent->fit().set_curiosity(parent->fit().curiosity()-0.5);
        return false;
	
      }

      
      void update(){_update_novelty();}


    

    const array_t& archive() const { return _array; }
    const array_t& parents() const { return _array_parents; }

    protected:

    template<typename I>
    float _dist_center(const I& indiv)
    {
        /* Returns distance to center of behavior descriptor cell */
        float dist = 0.0;
        point_t p = get_point(indiv);
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
            dist += pow(p[i] - (float)round(p[i] * (float)(behav_shape[i] - 1))/(float)(behav_shape[i] - 1), 2);

        dist=sqrt(dist);
        return dist;
    }
      

      void _update_novelty(){
      
	tbb::parallel_for( tbb::blocked_range<indiv_t*>(_array.data(),_array.data() + _array.num_elements()), 
			   Par_novelty<Map<Phen,Params> >(*this));
      }




      // Functor to iterate over a Boost MultiArray concept instance.
      template<typename T, typename V, size_t Dimensions = T::dimensionality>
      struct IterateHelper {
	void operator()(T& array, V& vect) const {
	  for (auto element : array)
	    IterateHelper<decltype(element), V>()(element, vect);
	}
      };

      // Functor specialization for the final dimension.
      template<typename T, typename V>
      struct IterateHelper<T, V, 1> {
	void operator()(T& array, V& vect) const {
	  for (auto& element : array)
	    if(element)
	      vect.push_back(element);
	}
      };

      // Utility function to apply a function to each element of a Boost
      // MultiArray concept instance (which includes views).
      template<typename T, typename V>
      static void iterate(T& array, V& vect) {
	IterateHelper<T, V>()(array, vect);
      }



      template<typename Map_t>
      struct Par_novelty{
	Par_novelty(Map_t& map):_map(map),_par_array(map._array){}
	Map_t& _map;
	array_t& _par_array;	
	void operator()(const tbb::blocked_range<indiv_t*>& r ) const {
	  for(indiv_t* indiv=r.begin(); indiv!=r.end(); ++indiv) 
	    if(*indiv)                                                                                                                                              
	      {                                                                                                                                                     
		int count =0;
		view_t neighborhood = _map.get_neighborhood(*indiv);
		std::vector<indiv_t> neigh;
		iterate(neighborhood,neigh);
		(*indiv)->fit().set_novelty(-(double)neigh.size());
	      }
	}
	
	};
      
      view_t get_neighborhood(indiv_t indiv)const{
	behav_index_t ind = get_index(indiv);
	index_gen_t indix;
	int i=0;
	for(auto it=indix.ranges_.begin();it!=indix.ranges_.end();it++)                                                                                     
	  {
	    *it=index_range_t(std::max((int)ind[i]-(int)Params::nov::deep,0),std::min(ind[i]+Params::nov::deep+1,(size_t) behav_shape[i]-1));//bound! so stop at id[i]+2-1
	    i++;
	  }

	view_t ngbh=  _array[ indix ];  
	return ngbh;
      }


      array_t _array;
      array_t _array_parents;
      
    };
  }
}


#endif
