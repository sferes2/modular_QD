#ifndef STAT_SELECTION_HPP_
#define STAT_SELECTION_HPP_

#include <numeric>
#include <boost/multi_array.hpp>
#include <sferes/stat/stat.hpp>

//#define MAP_WRITE_PARENTS

namespace sferes
{
namespace stat
{
SFERES_STAT(Selection, Stat)
{
    public:
    typedef boost::shared_ptr<Phen> phen_t;
    typedef boost::multi_array<phen_t, Params::ea::behav_dim> array_t;
    


    template<typename E>
    void refresh(const E& ea)
    {
      if(!ofs.is_open()){
	std::string prefix("selection");
	
	std::string fname =  ea.res_dir() + "/"
	  + prefix
	  + std::string(".dat");
	ofs=std::ofstream(fname.c_str());
      }

      assert(ea.ptmp().size()==ea.p_parents().size());
      assert(ea.ptmp().size()==ea.added().size());
      
      for(size_t i=0;i<ea.ptmp().size();i++){
	ofs<<ea.gen()<<"  ";
	for(size_t dim = 0; dim < ea.p_parents()[i]->fit().desc().size(); ++dim)
	  ofs<<ea.p_parents()[i]->fit().desc()[dim]<<" ";
	ofs<<"   "<<ea.p_parents()[i]->fit().value()<<"   ";
	ofs<<"   "<<ea.p_parents()[i]->fit().curiosity()<<"   ";
	
	for(size_t dim = 0; dim < ea.ptmp()[i]->fit().desc().size(); ++dim)
          ofs<<ea.ptmp()[i]->fit().desc()[dim]<<" ";
	ofs<<"   "<<ea.ptmp()[i]->fit().value()<<"   ";
	
	ofs<<"   "<<ea.added()[i]<<std::endl;
	    
      }
    }
 protected:
    std::ofstream ofs;
};

}}

#endif
