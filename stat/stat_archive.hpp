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
	   //_write_progress(std::string("progress"), ea);
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
	    ofs << " " << it->second->fit().value() << "         ";
	    for(size_t dim = 0; dim < it->second->size(); ++dim)
              ofs << it->second->data(dim) << " ";
	    ofs<<std::endl;
            ++offset;
	  }
      }
      /*      template<typename EA>
	void _write_progress(const std::string& prefix,
			     const EA& ea) const
      {
	std::cout << "writing..." << prefix << std::endl;
	std::string fname = ea.res_dir() + "/"
                + prefix
	  + std::string(".dat");

	std::ofstream ofs(fname.c_str(), std::ofstream::out | std::ofstream::app);

        size_t archive_size = ea.aggreg().archive().size();
        double archive_mean = 0.0f;
        double archive_max = 0.0f;
        double sum_novelty = 0.0f;
	double mean_novelty = 0.0f;
        double var_novelty = 0.0f;
	double sum_quality = 0.0f;

	for(auto it = ea.aggreg().archive().begin(); it != ea.aggreg().archive().end(); ++it)
          {
	    
	    archive_mean += it->second->fit().value();
	    sum_quality += it->second->fit().value();
	    sum_novelty += ea.aggreg().get_novelty(it->second,ea.aggreg().archive());
	    if(archive_max < it->second->fit().value())
	      archive_max = it->second->fit().value();
	  }
            
        archive_mean /= archive_size;
	mean_novelty /= archive_size;
        for(auto it = ea.aggreg().archive().begin(); it != ea.aggreg().archive().end(); ++it)
          {
	    var_novelty+=std::pow((ea.aggreg().get_novelty(it->second,ea.aggreg().archive()) - mean_novelty),2);
	  }
	var_novelty /= archive_size;

        ofs << ea.gen() << " " << archive_size << " " << archive_mean << " " << archive_max << " " << sum_novelty <<  " " << sum_quality <<   " " << var_novelty<<std::endl;
	}*/


    };

  }
}

#endif
