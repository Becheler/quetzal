// Copyright 2016 Arnaud Becheler    <Arnaud.Becheler@egce.cnrs-gif.fr>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#ifndef __SIMULATORS_UTILS_H_INCLUDED__
#define __SIMULATORS_UTILS_H_INCLUDED__

#include "../demography/History.h"
#include "../coalescence/containers/Forest.h"
#include "../coalescence/policies/merger.h"

#include <map>

namespace quetzal {

  template<typename Env, typename F>
  struct neighbor_migration
  {

  	using coord_type = typename Env::coord_type;
    using env_type = Env;
    using F_type = F;

    env_type const& _landscape;
    F_type _f;

    neighbor_migration(env_type const& env, F f):
    _landscape(env),
    _f(f)
    {}

    template<typename T>
  	std::vector<coord_type> arrival_space(coord_type const& x, const T&) const
  	{
      auto v = _landscape.four_nearest_defined_cells(x);
      v.push_back(x);
      return v;
  	}

    template<typename T>
  	double operator()(coord_type const& x, coord_type const& y, T const& t) const
  	{
  		auto v = arrival_space(x,t);
      auto binop = [this, t](auto const& a, auto const& b){return a + _f(b,t);};
      double sum = std::accumulate(v.begin(), v.end(), 0.0, binop);
      return _f(y,t) / sum;
  	}

  };

template<typename Env, typename F>
auto make_neighbor_migration(Env const& env, F f )
{
  return neighbor_migration<Env, F>(env, f);
}

class cell_JC69
{
private:
	int m_t = 0;
	char m_allelic_state;

public:
	cell_JC69(int t): m_t(t) {}
	cell_JC69() = default;
	int time() const {return m_t;}
	char allelic_state() const {return m_allelic_state;}
	auto& allelic_state(char a)
	{
		m_allelic_state = a;
		return *this;
	}
};



/*
 * Set of mutation kernel policies to update the mutational states along a tree.
 */
namespace mutation_kernel{
  /* @brief Mute allelic states in the Arlequin standard data format.
  */
  class standard_data
  {
  public:
    // @typedef typde of the mutational state
    using state_type = bool;
    class cell_type
    {
    private:
    	int m_time = 0;
    	state_type m_allelic_state=false;

    public:
    	cell_type(int t): m_time(t) {}
    	cell_type() = default;
    	int time() const {return m_time;}
    	state_type allelic_state() const {return m_allelic_state;}
    	auto& allelic_state(state_type a)
    	{
    		m_allelic_state = a;
    		return *this;
    	}
    };
    /* @brief Sample a new mutational state
    * @param state the initial mutational state
    * @param nb_mutations the number of mutational event to simulate
    * @return the updated mutational state
    */
    template<typename UnusedRNG>
    static bool mute(state_type state, unsigned int nb_mutations, UnusedRNG&)
    {
      if ( nb_mutations % 2 != 0)
      {
        state = !state;
      }
      return state;
    }
  };


 /* @brief JC69 mutational kernel
  */
  class JC69
  {
    static const std::map<char, std::string> dico;
  public:
    // @typedef allelic state type
    using state_type = char;
    /*
     * @brief Sample a new mutational state
     * @param state the initial mutational state
     * @param nb_mutations the number of mutational events to simulate
     * @param gen a random number generator
     * @tparam Generator a random number generator
     * @return
     */
    template<typename Generator>
    static state_type mute(state_type state, unsigned int nb_mutations, Generator & gen)
    {
      assert( dico.find(state) != dico.end() );
      for(unsigned int i = 0; i < nb_mutations; ++i)
      {
        std::uniform_int_distribution dist(0,2);
        state = dico.at(state)[dist(gen)];
      }
      return state;
    }
  };

} // namespace mutation_kernel



} // namespace quetzal

const std::map<char, std::string> quetzal::mutation_kernel::JC69::dico = { {'A', "TGC"}, {'T', "ACG"}, {'G', "ATC"}, {'C', "ATG"} };

#endif
