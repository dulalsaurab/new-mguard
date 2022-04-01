#include "test-common.hpp"

#include <server/util/name-tree.hpp>
#include <common.hpp>

using namespace ndn;

namespace mguard {
namespace util {
namespace nametree {
namespace tests {

BOOST_AUTO_TEST_CASE(Constructor)
{
    util::nametree::NameTree nametree;
    nametree.insertName("/aa/bb/cc");
    nametree.insertName("/aa/ff/mm/cc");
    nametree.insertName("/aa/ff/kk");
    nametree.insertName("/aa/ke");
    auto root = nametree.getTreeRoot();
    
    BOOST_CHECK(nametree.search(root, "/aa/bb/cc") != nullptr);

    BOOST_CHECK(nametree.search(root, "/aa/ke") != nullptr);

    BOOST_CHECK(nametree.search(root, "/aa/kk/ee") == nullptr);

    std::cout << "Longest prefix match for prefix: " << "/aa/ke/ff/rr "  << nametree.longestPrefixMatch("/aa/ke/ff/rr") << std::endl;

    nametree.deleteNode("/aa/ff");

    BOOST_CHECK(nametree.search(root, "/aa/ff/mm") == nullptr);
    BOOST_CHECK(nametree.search(root, "/aa/ke") != nullptr);
    BOOST_CHECK(nametree.search(root, "/aa/ff") == nullptr);
}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter
}
}
}
