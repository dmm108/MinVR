#include <map>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "config/VRDataIndex.h"
#include "config/VRDataQueue.h"

#define HELPMESSAGE std::cout << "usage: MVRLookup file.xml vname [ n ]" << std::endl; \
  std::cout << "  file.xml - a file containing a VRDataIndex" << std::endl; \
  std::cout << "  vname - the name of the datum to retrieve" << std::endl;  \
  std::cout << "  n - if vname is an array, return the nth entry (default: 0)" << std::endl;

// This program takes a config file, and a data name, and prints out
// the value corresponding to that name on standard output.  If the
// value is an array, print out the nth entry in the array if n is
// specified, the 0th otherwise.  It does not print the \n at the end
// of a line, so is suitable to use in a script.  To return the value
// of /MVR/RootNode/XPos in the Desktop.xml file, use it like this:
//
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/XPos`
//
// For an array value, try one of these:
//
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/HeadMatrix 0`
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/HeadMatrix 1`
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/HeadMatrix 2`
// 
int main(int argc, char **argv) {

  VRDataIndex *index = new VRDataIndex;

  // These will be extracted from argv.
  int nth = 0;

  try {
  
    // Extracting...
    switch(argc) {

    case 1:
      HELPMESSAGE;
      return EXIT_SUCCESS;
    
    case 2:
      if (argv[1][0] == '-') {
        HELPMESSAGE;
        return EXIT_SUCCESS;
      }
      throw std::runtime_error(std::string("Need a data name."));
      break;

    case 3:
      // Don't really need to do anything here except avoid throwing error.
      break;
    
    case 4:
      // If there is a fourth arg, hope it's an integer.
      sscanf(argv[3], "%d", &nth);
      break;
    
    default:
      throw std::runtime_error(std::string("Too many arguments"));
      break;
    }

    index->processXMLFile(argv[1], std::string("/"));

    VRCORETYPE_ID tp = index->getType(argv[2]);

    // std::cout << "datumName:" << datumName << " N:" << nth << " tp:" << tp << std::endl;

    // Now print out the values, according to the data type.
    switch(tp) {

    case VRCORETYPE_INT: 
      std::cout << (VRInt)index->getValue(argv[2]);
      break;

    case VRCORETYPE_DOUBLE: 
      std::cout << (VRDouble)index->getValue(argv[2]);
      break;

    case VRCORETYPE_STRING:
      std::cout << (VRString)index->getValue(argv[2]);
      break;

    case VRCORETYPE_INTARRAY: {
      VRIntArray ia = index->getValue(argv[2]);
      if (nth >= ia.size())
        throw std::runtime_error(std::string("N too large for array"));
      std::cout << ia[nth];
      break;
    }
    
    case VRCORETYPE_DOUBLEARRAY: {
      VRDoubleArray ia = index->getValue(argv[2]);
      if (nth >= ia.size())
        throw std::runtime_error(std::string("N too large for array"));
      std::cout << ia[nth];
      break;
    }

    case VRCORETYPE_STRINGARRAY: {
      VRStringArray ia = index->getValue(argv[2]);
      if (nth >= ia.size())
        throw std::runtime_error(std::string("N too large for array"));
      std::cout << ia[nth];
      break;
    }

    case VRCORETYPE_CONTAINER:
      std::cout << index->serialize(argv[2]);
      break;
    
    default:
      throw std::runtime_error(std::string("Not supported for that type."));
      break;
    }
  } catch (const std::exception& e) {

    std::cerr << "Oopsy-daisy: " << e.what() << std::endl;
  }
}
