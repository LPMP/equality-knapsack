#ifndef EKP_KNAPSACK_PARSER_HXX
#define EKP_KNAPSACK_PARSER_HXX

#include <config.hxx>
#include <parser_config.hxx>

namespace ekp {
  namespace parser {

    struct headline :  pegtl::seq<opt_whitespace,pegtl::string<'E','K','P'>,opt_whitespace> {};
    struct variables : positive_integer {};

    struct cost_entry : number {};
    struct costs : pegtl::seq<pegtl::plus< pegtl::seq< opt_whitespace,cost_entry,opt_whitespace > >,
			      pegtl::sor<pegtl::eol,pegtl::eof>> {};

    struct weight_entry : number {};
    struct weights : pegtl::seq<pegtl::plus< pegtl::seq< opt_whitespace,weight_entry,opt_whitespace > >,
				pegtl::sor<pegtl::eol,pegtl::eof>> {};

    struct rhs : number {};

    struct grammar : pegtl::must<
      headline,pegtl::eol,opt_invisible,
      variables,opt_invisible,
      costs,opt_invisible,
      weights,opt_invisible,
      rhs
      > {};

    template< typename Rule >
    struct action
      : pegtl::nothing< Rule >
    {};

    template <>
    struct action<variables>
    {
      template<typename input,typename G>
      static void apply(const input& in, G& v){
	       v.numberOfVariables = std::stoi(in.string());
      }
    };

    template <>
    struct action<cost_entry>
    {
      template<typename input,typename G>
      static void apply(const input& in, G& v){
        v.costs.push_back(std::stod(in.string()));
      }
    };

    template <>
    struct action<weight_entry>
    {
      template<typename input,typename G>
      static void apply(const input& in, G& v){
        REAL w = std::stod(in.string());
        INDEX wi = (INDEX) w;
        if( w != wi ){
          throw pegtl::parse_error("Only integer values are allowed for w!", in );
        }
        v.weights.push_back(wi);
      }
    };

    template <>
    struct action<rhs>
    {
      template<typename input,typename G>
      static void apply(const input& in, G& v){
        REAL b = std::stod(in.string());
        v.b = (INDEX) b;
        if( v.b != b ){
          throw pegtl::parse_error("Only integer values are allowed for b!", in );
        }
      }
    };


    struct Model {
      INDEX numberOfVariables = 0;
      INDEX b;
      std::vector<REAL> costs = {};
      std::vector<INDEX> weights = {};
    };

    inline Model GetEKPData(std::string& filename){
      Model m;
      pegtl::file_input<> in(filename);
      pegtl::parse<grammar,action>( in ,m );
      return m;
    }

  }

}
#endif // EKP_KNAPSACK_OBJECT_HXX
