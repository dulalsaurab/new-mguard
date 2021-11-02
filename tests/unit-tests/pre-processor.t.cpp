#include <mguard/server/pre-processor.hpp>
#include "test-common.hpp"
#include <ndn-cxx/name.hpp>

namespace mguard {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestDataPreprocessor)

BOOST_AUTO_TEST_CASE(Constructor)
{
  DataPreprocessor("/Users/saurabdulal/Documents/PROJECTS/mGuard/attribute_mapping_table.info");
}

BOOST_AUTO_TEST_SUITE_END()

} // tests
} // mguard