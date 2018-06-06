
//  compiles with g++ -o3 cross_val.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../../quetzal.h"
#include "../../../modules/simulator/simulators.h"
#include "../../../modules/fuzzy_transfer_distance/FuzzyPartition.h"
#include "../../../modules/fuzzy_transfer_distance/RestrictedGrowthString.h"
#include <memory>
#include <random>
#include <functional>  // std::plus
#include <map>



template<typename Space, typename Time, typename Value>
std::ostream& operator <<(std::ostream& stream, const quetzal::demography::PopulationFlux<Space, Time, Value>& flows)
{
  for(auto const& it : flows){
    stream << it.first.time << "\t" << it.first.from <<  "\t" << it.first.to << "\t" << it.second << "\n";
  }
    return stream;
}

template<typename Space, typename Tree>
std::ostream& operator <<(std::ostream& stream, const quetzal::coalescence::Forest<Space, Tree>& forest)
{
  for(auto const& it1 : forest){
    stream << it1.first << "\t -> \t [";
    for(auto const& it2 : it1.second){
      stream << it2 << " ";
    }
    stream << "]\n";
  }
    return stream;
}

struct Gaussian
{
  class param_type{
    double _a;
  public:
    double a() const {return _a;}
    void a(double value) { _a = value ;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    assert(a > 0 && r >= 0);
    return 1/(M_PI*a*a) * std::exp(-(r*r)/(a*a)) ;
  }
};

struct Logistic
{
  class param_type{
    double _a;
    double _b;
  public:
    double a() const {return _a;}
    double b() const {return _b;}
    void a(double value) { _a = value;}
    void b(double value) { _b = value;}
  };

  static double pdf(double r, param_type const& p)
  {
    double a = p.a();
    double b = p.b();
    assert(a > 0 && b >2 && r >= 0);
    return (b/(2*M_PI*(a*a)*std::tgamma(2/b)*std::tgamma(1-2/b)))*(1/(1+(std::pow(r,b)/(std::pow(a,b)))));
  }

};

class GenerativeModel{

public:

  using generator_type = std::mt19937;

private:

  class Params : public Gaussian::param_type {
  public:

    auto N0() const {return m_N0; }
    void N0(unsigned int N) { m_N0 = N; }

    auto k() const {return m_k; }
    void k(unsigned int value) { m_k = value; }

    auto r() const { return m_r; }
    void r(unsigned int value) { m_r = value; }

  private:
    unsigned int m_k;
    unsigned int m_r;
    unsigned int m_N0;
  };

  struct Prior {
    Params operator()(generator_type& gen) const
    {
      GenerativeModel::param_type params;
      // locus 11 has max diversity: 8
      params.N0(8);
      params.k(std::uniform_int_distribution<>(1,500)(gen));
      params.r(std::uniform_real_distribution<>(1.0, 20.0)(gen));
      params.a(std::uniform_real_distribution<>(100.0, 1000.0)(gen));
      return params;
    }
  };

  // Spatio-temporal coordinates
public:
  using time_type = unsigned int;
  using landscape_type = quetzal::geography::EnvironmentalQuantity<time_type>;
  using coord_type = typename landscape_type::coord_type;
private:

  using distance_dico_type = std::unordered_map<coord_type, std::vector<coord_type::km>>;
  using law_type =  std::discrete_distribution<unsigned int>;

  // Demographic simulation types
  using N_type = unsigned int;
  using history_type = quetzal::demography::History<coord_type, time_type, N_type, quetzal::demography::Flow<coord_type, time_type, N_type>>;
  using simulator_type = quetzal::simulators::IDDC_model_1<coord_type, time_type, N_type>;
  using kernel_type = Gaussian;

  // Genetic sample
public:
  using loader_type = quetzal::genetics::Loader<coord_type, quetzal::genetics::microsatellite>;
private:
  using dataset_type = loader_type::return_type;
  using locus_ID_type = dataset_type::locus_ID_type;

  // Coalescence simulation
  using tree_type = std::vector<coord_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;

  // ftd
  mutable std::map<unsigned int, std::discrete_distribution<unsigned int>> m_distribs;

public:
  // Interface for ABC module
  using param_type = Params;
  using prior_type = Prior;
  using result_type = std::vector<FuzzyPartition<coord_type>> ;

private:

  const landscape_type & m_landscape;
  std::unique_ptr<dataset_type> m_dataset;
  std::unique_ptr<std::vector<coord_type>> m_demes;
  std::unordered_map<coord_type, unsigned int> m_reverse_demes;
  std::map<locus_ID_type, forest_type> m_forests;
  distance_dico_type m_distances;

  // Initial distribution
  coord_type m_x0;
  time_type m_t0;
  time_type m_sampling_time;

public:
  GenerativeModel(const landscape_type & landscape, dataset_type dataset):
  m_landscape(landscape),
  m_dataset(make_data(dataset)),
  m_demes(std::make_unique<std::vector<coord_type>>(landscape.geographic_definition_space())),
  m_reverse_demes(make_reverse(*m_demes)),
  m_forests(make_forests(*m_dataset)),
  m_distances(compute_distances())
  {}

  std::unordered_map<coord_type, unsigned int> make_reverse(std::vector<coord_type> const& demes) const {
    unsigned int position = 0;
    std::unordered_map<coord_type, unsigned int> m;
    for(auto const& it : demes)
    {
      m.emplace(it, position);
      ++position;
    }
    return m;
  }

  distance_dico_type compute_distances() const {
  distance_dico_type map;
    assert(m_demes->size() > 0);
    for(auto const& x0 : *m_demes){
      std::vector<coord_type::km> distances;
      distances.reserve(m_demes->size());
      auto unop = [x0](const auto & y){return x0.great_circle_distance_to(y);};
      std::transform(m_demes->cbegin(), m_demes->cend(), std::back_inserter(distances), unop);
      map.insert(std::make_pair(x0, std::move(distances)));
    }
    return map;
  }

  std::unique_ptr<dataset_type> make_data(dataset_type const& data){
    auto ptr = std::make_unique<dataset_type>(data);
    ptr->reproject(m_landscape);
    return ptr;
  }


  forest_type make_forest(dataset_type const& data, dataset_type::locus_ID_type locus) const {
    forest_type forest;
    for(auto const& x : data.get_sampling_points())
    {
      for(auto const& individual : data.individuals_at(x))
      {
        auto alleles = individual.alleles(locus);
        if(alleles.first.get_allelic_state() > 0) forest.insert(x, std::vector<coord_type>(1, x));
        if(alleles.second.get_allelic_state() > 0) forest.insert(x, std::vector<coord_type>(1, x));
      }
    }
    return forest;
  }

  std::map<locus_ID_type, forest_type> make_forests(dataset_type const& data) const {
    std::map<locus_ID_type, forest_type> m;
    for(auto const& it : data.loci()){
      m.emplace(it, make_forest(data, it));
    }
    return m;
  }

  auto& introduction_point(coord_type const& x0, time_type t0){
    m_x0 = m_landscape.reproject_to_centroid(x0);
    m_t0 = t0;
    return *this;
  }

  auto& sampling_time(time_type t){
    m_sampling_time = t;
    return *this;
  }

  auto make_growth_expression(param_type const& param, history_type::N_type const& N) const
  {
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;
    literal_factory<coord_type, time_type> lit;
    auto r = lit(param.r());
    auto K = lit(param.k());
    auto N_cref = std::cref(N);
    auto N_expr = use([N_cref](coord_type x, time_type t){return N_cref(x,t);});
    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/K));
    auto sim_N_tilde = [g](generator_type& gen, coord_type x, time_type t){
      std::poisson_distribution<history_type::N_type::value_type> poisson(g(x,t));
      return poisson(gen);
    };
    return sim_N_tilde;
  }

  template<typename Kernel>
  auto compute_weights(std::vector<coord_type::km> const& d, typename Kernel::param_type const& p) const
  {
    assert(d.size() > 0);
    std::vector<double> weights;
    weights.reserve(d.size());
    std::transform(d.cbegin(), d.cend(), std::back_inserter(weights), [p](auto r){return Kernel::pdf(r, p);} );
    return weights;
  }

  template<typename Kernel>
  std::discrete_distribution<unsigned int>
  make_distribution(coord_type const& x, typename Kernel::param_type const& p) const
  {
    auto const& d = m_distances.at(x);
    assert(d.size() > 0);
    auto w = compute_weights<Kernel>(d, p);
    return std::discrete_distribution<unsigned int>(w.begin(), w.end());
  }

  coord_type sample(
    quetzal::random::TransitionKernel<law_type> & kernel,
    coord_type const& x,
    param_type const& param,
    generator_type & gen) const
  {
    auto id = m_reverse_demes.at(x);
    if( ! kernel.has_distribution(id) )
    {
      kernel.set(id, make_distribution<Gaussian>(x, param));
    }
    return m_demes->at(kernel(gen, id));
  }

    auto fuzzifie(forest_type const& forest, locus_ID_type const& locus) const {
    std::map<coord_type, std::vector<double>> coeffs;

    for(auto const& it : m_forests.at(locus).positions())
    {
      coeffs[it].resize(forest.nb_trees());
    }

    unsigned int cluster_id = 0;
    for(auto const& it1 : forest )
    {
      for(auto const& it2 : it1.second)
      {
        coeffs[it2][cluster_id] += 1;
      }
      cluster_id += 1;
    }

    for(auto & it1 : coeffs){
      double sum = std::accumulate(it1.second.begin(), it1.second.end(), 0.0);
      assert(sum > 0.0);
      for(auto & it2 : it1.second){
        it2 = it2/sum;
      }
    }
    return FuzzyPartition<coord_type>(coeffs);
  }


    unsigned int factorial(unsigned int n) const
    {
        if (n == 0)
           return 1;
        return n * factorial(n - 1);
    }

    int countP(int n, int k) const
    {
      // Base cases
      if (n == 0 || k == 0 || k > n)
         return 0;
      if (k == 1 || k == n)
          return 1;

      // S(n+1, k) = k*S(n, k) + S(n, k-1)
      return  k*countP(n-1, k) + countP(n-1, k-1);
    }

    int Bell(unsigned int n) const
    {
      int sum = 0;
      for(unsigned int k = 1; k <= n; ++k)
      {
        sum += countP(n, k);
      }
      return sum;
    }

    double pdf(unsigned int k, unsigned int n) const
    {
      double a = static_cast<double>(factorial(k));
      double b = static_cast<double>(Bell(n));
      double e = 2.71828;
      double p = std::pow(k,n)/(a*b*e);
      return p;
    }

    template<typename Generator>
    auto sample_partition(unsigned int n, Generator& gen) const
    {
      if(m_distribs.find(n) == m_distribs.end()){
        std::vector<double> w;
        size_t size = 10000;
        w.resize(size);
        w[0] = 0;
        for(unsigned int k = 1; k < size; ++k)
        {
          w[k] = pdf(k,n);
        }
        m_distribs[n] = std::discrete_distribution<unsigned int>(w.begin(), w.end());
      }

      unsigned int K = m_distribs.at(n)(gen);

      std::uniform_int_distribution<> unif(1, K);
      std::vector<unsigned int> Cs;
      for(unsigned int i = 1; i <= n; ++i )
      {
        Cs.push_back(unif(gen));
      }

      std::map<unsigned int, unsigned int> ids;
      std::vector<unsigned int> rgs;
      rgs.resize(Cs.size());

      unsigned int block_ID = 0;
      for(unsigned int i = 0; i < Cs.size(); ++i){
        auto it = ids.find(Cs[i]);
        if( it != ids.end() ){
          rgs[i] = it->second;
        }else {
          rgs[i] = block_ID;
          ids[Cs[i]] = block_ID;
          ++block_ID;
        }
      }
      return RestrictedGrowthString(rgs);
    }

  auto operator()(generator_type& gen, param_type const& param) const
  {
    simulator_type simulator(m_x0, m_t0, param.N0());

    // Dispersal patterns

    auto ptr = std::make_shared<quetzal::random::TransitionKernel<law_type>>();
    auto light_kernel = [ptr, param, this](auto& gen, coord_type x, time_type t){
      return this->sample(*ptr, x, param, gen);
    };

    // Growth patterns
    auto growth = make_growth_expression(param, simulator.size_history());

    auto merge_binop = [](const tree_type &parent, const tree_type &child)
    {
      tree_type copy = parent;
      copy.insert( copy.end(), child.begin(), child.end() );
      return copy;
    };

    result_type fps;

    std::map<coord_type, unsigned int> counts;
    for(auto const& it : m_dataset->get_sampling_points()){
      counts[it] = (m_dataset->size(it)) * 2;
    }

    auto const& history = simulator.simulate_demography(counts, growth, light_kernel, m_sampling_time, gen);

    for(auto const& locus : m_dataset->loci() ){
      auto updated_forest = simulator.coalescence_process(m_forests.at(locus), history, merge_binop, gen);
      auto S_sim = fuzzifie(updated_forest, locus);
      //std::cout << "Simulated fuzzy Partiton:\n" << S_sim << std::endl;
      if(S_sim.nClusters() > 1 )
      {
        S_sim.merge_clusters(sample_partition(S_sim.nClusters(), gen));
      }
      fps.push_back(S_sim);
    }

    //std::cout << "Aggregated simulated fuzzy Partiton:\n" << S_sim << std::endl;

    return fps;

  }

  FuzzyPartition<coord_type> fuzzifie_data(GenerativeModel::dataset_type::locus_ID_type const& locus) const {
    using cluster_type = unsigned int;
    std::set<cluster_type> clusters;
    std::map<coord_type, std::vector<double>> coeffs;

    auto frequencies = m_dataset->frequencies_discarding_NA(locus);
    unsigned int n_clusters = m_dataset->allelic_richness(locus);

    // preparing coeffs clusters vectors
    // and preparing clusters set
    for(auto const& it1 : frequencies)
    {
      coeffs[it1.first].resize(n_clusters);
      for(auto const& it2: it1.second){
        clusters.insert(it2.first);
      }
    }

    // fill the coefficients
    for(auto & it1 : coeffs){
      for(auto const& it2 : frequencies[it1.first]){
        auto allele = it2.first;
        auto freq = it2.second;
        auto it_pos = std::find(clusters.cbegin(), clusters.cend(), allele);
        assert(it_pos != clusters.end());
        auto index = std::distance(clusters.begin(), it_pos);
        it1.second[index] = freq;
      }
    }

    return FuzzyPartition<coord_type>(coeffs);
  }


}; // GenerativeModel

struct Wrapper{
  // Interface for ABC module
  using param_type = GenerativeModel::param_type;
  using prior_type = GenerativeModel::prior_type;
  using result_type = GenerativeModel::result_type;
  using generator_type = GenerativeModel::generator_type;

  GenerativeModel& m_model;

  Wrapper(GenerativeModel& model): m_model(model){};

  result_type operator()(generator_type& gen, param_type const& param) const {
    return m_model(gen, param);
  }

};

int main()
{
  using result_type = GenerativeModel::result_type;

  std::mt19937 gen;
  std::string bio_file = "/home/becheler/Documents/VespaVelutina/wc2.0_10m_prec_01_europe_agg_fact_5.tif";
  GenerativeModel::landscape_type landscape(bio_file, std::vector<GenerativeModel::time_type>(1));

  std::string file = "/home/becheler/Documents/VespaVelutina/dataForAnalysis.csv";
  //std::string file = "/home/becheler/dev/quetzal/modules/genetics/microsat_test.csv";
  GenerativeModel::loader_type loader;
  auto dataset = loader.read(file);

  GenerativeModel model(landscape, dataset);

  model.introduction_point(GenerativeModel::coord_type(44.00, 0.20), 2004);
  model.sampling_time(2008);

  GenerativeModel::prior_type prior;
  Wrapper wrap(model);

  auto abc = quetzal::abc::make_ABC(wrap, prior);

  auto table = abc.sample_prior_predictive_distribution(20000, gen);

  GenerativeModel::param_type true_param;
  true_param.N0(8);
  true_param.k(250);
  true_param.r(10);
  true_param.a(500);

  std::vector<result_type> pods;

  for(int i = 0; i != 10; ++i){
    try {
      pods.push_back(model(gen, true_param));
    }catch(...){
      std::cerr << "one pod less" << std::endl;
    }
  }

  // headers
  std::cout << "pod\tr\tk\tN0\ta\t";
  auto const& loci = dataset.loci();
  std::copy(loci.begin(), loci.end(), std::ostream_iterator<std::string>(std::cout, "\t"));
  std::cout << "\n";

  auto distance = [](result_type const& o, result_type const& s){
    assert(o.size() == s.size());
    std::vector<double> v;
    for(unsigned int i = 0; i < o.size(); ++ i )
    {
      v.push_back(o.at(i).fuzzy_transfer_distance(s.at(i)));
    }
    return v;
  };


  unsigned int id = 0 ;
  for(auto const& it : pods)
  {
    auto ftds = table.compute_distance_to(it, distance);
    for(auto const& it2 : ftds)
    {
      auto const& p = it2.param();
      std::cout << id << "\t"
                << std::to_string(p.r()) << "\t"
                << std::to_string(p.k()) << "\t"
                << std::to_string(p.N0()) << "\t"
                << std::to_string(p.a()) << "\t";

      std::copy(it2.data().begin(), it2.data().end(), std::ostream_iterator<double>(std::cout, "\t"));
      std::cout << "\n";
    }
    id += 1;
  }

  return 0;
}