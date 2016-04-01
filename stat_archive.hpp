#ifndef STAT_ARCHIVE_HPP_
#define STAT_ARCHIVE_HPP_

#include <numeric>
#include <sferes/stat/stat.hpp>

//#define MAP_WRITE_PARENTS

namespace sferes
{
  namespace stat
  {
    SFERES_STAT(Archive, Stat)
    {
    public:
      template<typename E>
	void refresh(const E& ea)
      {

	if (ea.gen() % Params::pop::dump_period == 0)
	  {
	   _write_archive(std::string("archive_"), ea);
	  }

      }
      template<typename EA>
	void _write_archive(const std::string& prefix,
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
        for(auto it = ea.aggreg().archive().begin(); it != ea.aggreg().archive().end(); ++it)
	  {
	    ofs << offset << "    ";
	    for(size_t dim = 0; dim < it->second->fit().desc().size(); ++dim)
	      ofs << it->second->fit().desc()[dim] << " ";
	    //ofs << " " << array(idx)->fit().value() << std::endl;
	    ofs << " " << it->second->fit().value() << std::endl;
	    
            ++offset;
	  }
      }


    };

  }
}

#endif
