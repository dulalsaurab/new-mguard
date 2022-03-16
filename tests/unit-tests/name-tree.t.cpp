#include "test-common.hpp"

#include <server/name-tree.hpp>
#include <server/util/stream.hpp>
#include <server/common.hpp>


#include <chrono>
#include <unistd.h>
#include <thread>

using namespace ndn;
using DummyClientFace = ndn::util::DummyClientFace;

namespace mguard {
namespace tests{

BOOST_FIXTURE_TEST_SUITE(TestDataAdapter, IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
 // unit test will be added later
}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter

} // tests
} // mguard
