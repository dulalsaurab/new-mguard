
#ifndef MGUARD_COMMON_HPP
#define MGUARD_COMMON_HPP

#include <ndn-cxx/face.hpp>

#include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/time.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <vector>

namespace mguard
{
  const std::string DATA_DIR = "/Users/saurabdulal/Documents/PROJECTS/mGuard/data";
  
  struct AttributeMappingTable
  {
		std::string source, columnInSource;
		std::vector<std::string> appliedTo; //streams id
	};
}

#endif // MGUARD_COMMON_HPP
