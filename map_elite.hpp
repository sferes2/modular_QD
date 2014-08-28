//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//|
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.


#ifndef MAP_ELITE_HPP_
#define MAP_ELITE_HPP_

#include <algorithm>
#include <limits>

#include <boost/foreach.hpp>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <sferes/stc.hpp>
#include <sferes/ea/ea.hpp>
#include <sferes/fit/fitness.hpp>


namespace sferes
{
namespace ea
{

// Main class
SFERES_EA(MapElite, Ea)
{
    public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;
    typedef typename pop_t::iterator it_t;
    typedef typename std::vector<std::vector<indiv_t> > front_t;

    //typedef boost::array<float, 2> point_t;


    typedef boost::shared_ptr<Phen> phen_t;
    //typedef boost::multi_array<phen_t, 2> array_t;


    /*static const size_t res_x = Params::ea::res_x;
      static const size_t res_y = Params::ea::res_y;*/

    //static const size_t behav_dim = Params::ea::behav_shape_size();
    static const size_t behav_dim = Params::ea::behav_dim;

    typedef boost::array<float, behav_dim> point_t;
    typedef boost::multi_array<phen_t, behav_dim> array_t;
    typedef boost::array<typename array_t::index, behav_dim> behav_index_t;
    behav_index_t behav_shape;


    MapElite()
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

    /*MapElite() :
    _array(boost::extents[res_x][res_y]),
    _array_parents(boost::extents[res_x][res_y])
      {
      }*/

    void random_pop()
    {
        parallel::init();
        this->_pop.resize(Params::pop::init_size);
        BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_pop)
        {
            indiv = boost::shared_ptr<Phen>(new Phen());
            indiv->random();
        }
        this->_eval.eval(this->_pop, 0, this->_pop.size());
        BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_pop)
                _add_to_archive(indiv, indiv);
    }

    void epoch()
    {
        this->_pop.clear();

        //for_each(_array, a function call);


        /*        ‘boost::multi_array<boost::shared_ptr<sferes::phen::Parameters<sferes::gen::EvoFloat<10, Params>, Rastrigin<Params>, Params> >, 2ul, std::allocator<boost::shared_ptr<sferes::phen::Parameters<sferes::gen::EvoFloat<10, Params>, Rastrigin<Params>, Params> > > >::element {aka boost::shared_ptr<sferes::phen::Parameters<sferes::gen::EvoFloat<10, Params>, Rastrigin<Params>, Params> >}’)
        type ‘boost::none_t {aka int boost::detail::none_helper::*}’*/


        //for(auto i = _array.data(); i < (_array.data() + _array.num_elements()); ++i)
        for(const phen_t* i = _array.data(); i < (_array.data() + _array.num_elements()); ++i)
            if(*i)
                this->_pop.push_back(*i);

        /*for (size_t i = 0; i < res_x; ++i)
            for (size_t j = 0; j < res_y; ++j)
                if (_array[i][j])
                    this->_pop.push_back(_array[i][j]);*/

        pop_t ptmp, p_parents;
        for (size_t i = 0; i < Params::pop::size; ++i)
        {
            indiv_t p1 = _selection(this->_pop);
            indiv_t p2 = _selection(this->_pop);
            boost::shared_ptr<Phen> i1, i2;
            p1->cross(p2, i1, i2);
            i1->mutate();
            i2->mutate();
            i1->develop();
            i2->develop();
            ptmp.push_back(i1);
            ptmp.push_back(i2);
            p_parents.push_back(p1);
            p_parents.push_back(p2);
        }
        this->_eval.eval(ptmp, 0, ptmp.size());

        assert(ptmp.size() == p_parents.size());
        for (size_t i = 0; i < ptmp.size(); ++i)
            _add_to_archive(ptmp[i], p_parents[i]);
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

    const array_t& archive() const { return _array; }
    const array_t& parents() const { return _array_parents; }

    template<typename I>
    point_t get_point(const I& indiv) const
    {
            return _get_point(indiv);
    }

    protected:
    array_t _array;
    array_t _prev_array;
    array_t _array_parents;

    bool _add_to_archive(indiv_t i1, indiv_t parent)
    {
        if(i1->fit().dead())
            return false;

        point_t p = _get_point(i1);

        behav_index_t behav_pos;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
        {
            behav_pos[i] = round(p[i] * behav_shape[i]);
            behav_pos[i] = std::min(behav_pos[i], behav_shape[i] - 1);
            assert(behav_pos[i] < behav_shape[i]);
        }

        //boost::array<typename array_t::index, behav_dim> idx = behav_pos;

        /*if (!_array(behav_pos)
                || i1->fit().value() >
                _array(behav_pos)->fit().value())*/
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


        /*size_t x = round(p[0] * res_x);
        size_t y = round(p[1] * res_y);
        x = std::min(x, res_x - 1);
        y = std::min(y, res_y - 1);
        assert(x < res_x);
        assert(y < res_y);

        if (!_array[x][y]
                || i1->fit().value() >
                _array[x][y]->fit().value())
        {
            _array[x][y] = i1;
            _array_parents[x][y] = parent;
            return true;
        }
        return false;*/
    }


    template<typename I>
    float _dist_center(const I& indiv)
    {
        /* Returns distance to center of behavior descriptor cell */
        float dist = 0.0;
        point_t p = _get_point(indiv);
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
            dist += pow(p[i] - (float)round(p[i] * (float)(behav_shape[i] - 1))/(float)(behav_shape[i] - 1), 2);

        dist=sqrt(dist);
        return dist;
    }

    template<typename I>
    point_t _get_point(const I& indiv) const
    {
        point_t p;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
            p[i] = std::min(1.0f, indiv->fit().desc()[i]);

        /*p[0] =
                std::min(1.0f, indiv->fit().desc()[0]);
        p[1] =
                std::min(1.0f, indiv->fit().desc()[1]);*/

        return p;
    }

    indiv_t _selection(const pop_t& pop)
    {
        int x1 = misc::rand< int > (0, pop.size());
        return pop[x1];
    }


};
}
}
#endif
