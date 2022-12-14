#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include <IR.h>
#include <IR_parser.h>
#include <code_generator.h>

using namespace std;

void print_help (char *progName){
  std::cerr << "Usage: " << progName << " [-v] [-g 0|1] [-O 0|1|2] [-s] [-l] [-i] SOURCE" << std::endl;
  return ;
}

int main(
  int argc, 
  char **argv
  ){

  bool shouldPrint = true;
  // std::cout << "STARTING FROM THE VERY BEGINNING" << std::endl;
  // auto enable_code_generator = true;
  // auto spill_only = false;
  // auto interference_only = false;
  // auto liveness_only = false;
  // int32_t optLevel = 3;

  /* 
   * Check the compiler arguments.
   */
  auto verbose = false;
  if( argc < 2 ) {
    print_help(argv[0]);
    return 1;
  }
  // int32_t opt;
  // while ((opt = getopt(argc, argv, "vg:O:sli")) != -1) {
  //   switch (opt){

  //     case 'l':
  //       liveness_only = true;
  //       break ;

  //     case 'i':
  //       interference_only = true;
  //       break ;

  //     case 's':
  //       spill_only = true;
  //       break ;

  //     case 'O':
  //       optLevel = strtoul(optarg, NULL, 0);
  //       break ;

  //     case 'g':
  //       enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true ;
  //       break ;

  //     case 'v':
  //       verbose = true;
  //       break ;

  //     default:
  //       print_help(argv[0]);
  //       return 1;
  //   }
  // }

  /*
   * Parse the input file.
   */
  IR::Program p;
  if (shouldPrint) std::cout << "parse file started\n";
  p = IR::parse_file(argv[optind]);
  if (shouldPrint) std::cout << "parse file ended\n";

  /* TODO : linearize */



  /* Generate L3 code */

  if (shouldPrint) std::cout << "generate code started\n";
  IR::generate_code(p);
  if (shouldPrint) std::cout << "generate code ended\n";

  // L2::Function f;
  //auto p,f;
  // if (spill_only){

  //   /* 
  //    * Parse an L2 function and the spill arguments.
  //    */
  //   p = L2::parse_spill_file(argv[optind]);
 
  // } else if (liveness_only){

  //   /*
  //    * Parse an L2 function.
  //    */
  //   // std::cout << "Starting Parsing file now" << std::endl;
  //   p = L2::parse_function_file(argv[optind]);
  //   // std::cout << "Ending Parsing file now" << std::endl;

  // } else if (interference_only){

  //   /*
  //    * Parse an L2 function.
  //    */
  //   p = L2::parse_function_file(argv[optind]);

  // } else {

  //   /* 
  //    * Parse the L2 program.
  //    */
  //   p = L2::parse_file(argv[optind]);
  // }

  // /*
  //  * Special cases.
  //  */
  // if (spill_only){
  //   // TODO
  //   return 0;
  // }

  // /*
  //  * Liveness test.
  //  */
  // if (liveness_only){
  //   L2::create_liveness_list(p);
  //   return 0;
  // }

  // /*
  //  * Interference graph test.
  //  */
  // if (interference_only){
  //   // TODO
  //   return 0;
  // }

  // /*
  //  * Generate the target code.
  //  */
  // if (enable_code_generator){
  //   // TODO
  // }

  return 0;
}