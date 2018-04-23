#ifndef EKP_PARSER_CONFIG_HXX
#define EKP_PARSER_CONFIG_HXX
#include <tao/pegtl.hpp>
#include <config.hxx>

namespace pegtl = tao::pegtl;

namespace ekp {
  /**
  * This namespace collects general parser rules from PEGTL
  */
  namespace parser {
    /**
    * \brief at least one blank
    */
    struct mand_whitespace : pegtl::plus< pegtl::blank > {}; 
    struct opt_whitespace : pegtl::star< pegtl::blank > {}; 
    struct opt_invisible : pegtl::star< pegtl::sor< pegtl::blank, pegtl::eol > > {};
    struct mand_invisible : pegtl::plus< pegtl::sor< pegtl::blank, pegtl::eol > > {};
 
    struct positive_integer : pegtl::plus< pegtl::digit > {};
    struct real_number_standard : pegtl::sor<
        pegtl::seq< pegtl::opt< pegtl::one<'+','-'> >, pegtl::plus<pegtl::digit>,
             pegtl::opt< pegtl::seq< pegtl::string<'.'>, pegtl::star<pegtl::digit> > > >,
        pegtl::string<'I','n','f'>,
        pegtl::string<'i','n','f'>
        > {}; 
    struct real_number_exponential : pegtl::seq< pegtl::opt< pegtl::one<'+','-'> >, pegtl::star< pegtl::digit >,
                                          pegtl::opt<pegtl::seq< pegtl::string<'.'>, pegtl::star< pegtl::digit>>>, pegtl::string<'e'>,
                                          pegtl::opt< pegtl::one<'+','-'> >, pegtl::plus< pegtl::digit > > {};
    /**
    * \brief detects a number in order exponential form, real form or integer form
    */
    struct number : pegtl::sor<real_number_exponential,
                                real_number_standard,
                                positive_integer> {};
    
  }
  
}

#endif // EKP_PARSER_CONFIG_HXX
