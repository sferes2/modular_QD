#ifndef STAT_AGGREGATOR_HPP_
#define STAT_AGGREGATOR_HPP_

#include <numeric>
#include <sferes/stat/stat.hpp>

//#define MAP_WRITE_PARENTS

namespace sferes
{
  namespace stat
  {
    SFERES_STAT(Aggregator, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {

	if (ea.gen() % Params::pop::dump_period == 0)
	   _write_aggregator(std::string("archive_"), ea);

      }
      template<typename EA>
	void _write_aggregator(const std::string& prefix,
			    const EA& ea) const
      {
        std::cout << "writing..." << prefix << ea.gen() << std::endl;
        std::string fname = ea.res_dir() + "/"
	  + prefix
	  + boost::lexical_cast<
	std::string>(ea.gen())
	  + std::string(".dat");
	
        std::ofstream ofs(fname.c_str());
	
        size_t offset = 0;
        for(auto it = ea.pop().begin(); it != ea.pop().end(); ++it)
	  {
	    ofs << offset << "    ";
	    for(size_t dim = 0; dim < (*it)->fit().desc().size(); ++dim)
	      ofs << (*it)->fit().desc()[dim] << " ";
	    //ofs << " " << array(idx)->fit().value() << std::endl;

	    ofs << " " << (*it)->fit().value() << "         ";

	    for(size_t dim = 0; dim < (*it)->size(); ++dim)
		  ofs << (*it)->data(dim) << " ";
	    ofs<<std::endl;
            ++offset;
	  }
      }

    };

  }
}

#endif
