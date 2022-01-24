#ifndef MGUARD_COMMON_HPP
#define MGUARD_COMMON_HPP

#include <iostream>
#include <vector>

namespace mguard
{
  const std::string DATA_DIR = "data";
  
  struct AttributeMappingTable
  {
		std::string source, columnInSource;
		std::vector<std::string> appliedTo; //streams id
	};
}

#endif // MGUARD_COMMON_HPP
