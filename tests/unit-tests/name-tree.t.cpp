#include "../test-common.hpp"

#include <server/util/name-tree.hpp>
#include <common.hpp>

using namespace ndn;

namespace mguard {
namespace util {
namespace nametree {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(TestNameTree, mguard::tests::IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
    util::nametree::NameTree nametree;
    nametree.insertName("/aa/bb/cc");
    nametree.insertName("/aa/bb/ee");
    nametree.insertName("/aa/ff/mm/cc");
    nametree.insertName("/aa/ff/kk");
    nametree.insertName("/aa/ke");

   /* 
   BOOST_CHECK(nametree.search(root, "/aa/bb/cc") != nullptr);
   BOOST_CHECK(nametree.search(root, "/aa/ke") != nullptr);
   BOOST_CHECK(nametree.search(root, "/aa/kk/ee") == nullptr);
   */

    std::cout << "Longest prefix match for prefix: " << "/aa/ff/mm/cc/oo "  << nametree.longestPrefixMatch("/aa/ff/mm/cc/oo") << std::endl;
    std::vector<ndn::Name> v;
    auto vec = nametree.getLeaves("/aa/ff", v);
    for (auto& v: vec)
        std::cout << v << std::endl;
    
    v.push_back("/aa/ff");
    vec = nametree.getLeaves("/aa", v);
    std::cout << "next round" << std::endl;
    for (auto& v: vec)
        std::cout << v << std::endl;

    nametree.deleteNode("/aa/ff");

    /*  
    BOOST_CHECK(nametree.search(root, "/aa/ff/mm") == nullptr);
    BOOST_CHECK(nametree.search(root, "/aa/ke") != nullptr);
    BOOST_CHECK(nametree.search(root, "/aa/ff") == nullptr);
    */

}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter
}
}
}
}
