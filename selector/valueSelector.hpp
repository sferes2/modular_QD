#ifndef _SELECTOR_VALUESELECTOR_HPP
#define _SELECTOR_VALUESELECTOR_HPP

namespace sferes{
  namespace selector{

 
    struct getFitness{
      template <typename Phen> 
      static inline double getValue(Phen p){return p->fit().value();}
    };

    struct getNovelty{
      template <typename Phen> 
      static inline double getValue(Phen p){return p->fit().novelty();}
    };

    struct getCuriosity{
      template <typename Phen> 
      static inline double getValue(Phen p){return p->fit().curiosity();}
    };
    struct getLocalQuality{
      template <typename Phen> 
      static inline double getValue(Phen p){return p->fit().local_quality();}
    };
  }}
#endif
