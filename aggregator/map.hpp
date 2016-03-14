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
      typedef boost::shared_ptr<Phen> phen_t;
      
      static const size_t behav_dim = Params::ea::behav_dim;
      typedef boost::multi_array<phen_t, behav_dim> array_t;
      typedef boost::array<typename array_t::index, behav_dim> behav_index_t;
      
      typedef boost::array<float, behav_dim> point_t;

      behav_index_t behav_shape;
      

      Map()
      {
        // - my_type my_name(size_t i)
        // - size_t my_name_size()
	
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



    long int getindex(const array_t & m, const phen_t* requestedElement, const unsigned short int direction) const
    {
        int offset = requestedElement - m.origin();
        return (offset / m.strides()[direction] % m.shape()[direction] +  m.index_bases()[direction]);
    }

    behav_index_t getindexarray(const array_t & m, const phen_t* requestedElement ) const
    {
        behav_index_t _index;
        for (unsigned int dir = 0; dir < behav_dim; dir++ )
        {
            _index[dir] = getindex( m, requestedElement, dir );
        }

        return _index;
    }

    template<typename I>
    point_t get_point(const I& indiv) const
    {
        point_t p;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
            p[i] = std::min(1.0f, indiv->fit().desc()[i]);

        return p;
    }

      std::vector<indiv_t> get_full_content()
      {
	std::vector<indiv_t> content;
	for(const phen_t* i = _array.data(); i < (_array.data() + _array.num_elements()); ++i)
	  if(*i)
	    content.push_back(*i);
	return content;
      }

    bool add_to_archive(indiv_t i1, indiv_t parent)
    {
        if(i1->fit().dead())
            return false;

        point_t p = get_point(i1);

        behav_index_t behav_pos;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
        {
            behav_pos[i] = round(p[i] * behav_shape[i]);
            behav_pos[i] = std::min(behav_pos[i], behav_shape[i] - 1);
            assert(behav_pos[i] < behav_shape[i]);
        }

        float epsilon = 0.05;
        if (!_array(behav_pos)
                || (i1->fit().value() - _array(behav_pos)->fit().value()) > epsilon
                || (fabs(i1->fit().value() - _array(behav_pos)->fit().value()) <= epsilon && _dist_center(i1) < _dist_center(_array(behav_pos))))
        {
            _array(behav_pos) = i1;
            _array_parents(behav_pos) = parent;
            return true;
        }
        return false;

    }


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

    

    const array_t& archive() const { return _array; }
    const array_t& parents() const { return _array_parents; }

    protected:
      array_t _array;
      //array_t _prev_array;
      array_t _array_parents;
      
    };
  }
}


#endif
