#include "../test-common.hpp"

#include <server/util/name-tree.hpp>

using namespace ndn;

namespace mguard {
namespace util {
namespace nametree {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(TestNameTree, mguard::tests::IdentityTimeFixture)

BOOST_AUTO_TEST_CASE(Constructor)
{
    util::nametree::NameTree nameTree;

    // build the tree
    nameTree.insertName("/aa/bb/cc");
    nameTree.insertName("/aa/bb/ee");
    nameTree.insertName("/aa/ff/mm/cc");
    nameTree.insertName("/aa/ff/kk");
    nameTree.insertName("/aa/ke");

    // testing the tree

    // getTreeRoot
    BOOST_CHECK(nameTree.getTreeRoot()->m_fullName == "/");

    // getNode
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/bb").value()->m_fullName == "/aa/bb");
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/cc").value() == nullptr);

    // getLeaves
    for (const ndn::Name &name : nameTree.getLeaves("/aa/ff", {})) {
        BOOST_CHECK(name == (ndn::Name)"/aa/ff/mm/cc" || name == (ndn::Name)"/aa/ff/kk");
    }

    // getParent
    BOOST_CHECK(nameTree.getParent("/aa/ke")->m_fullName == "/aa");
    BOOST_CHECK(nameTree.getParent("/aa/ff/mm/cc")->m_fullName == "/aa/ff/mm");

    // getChildren

    // longestPrefixMatch
    // deleteNode
    // isChild
    // findNode

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter
}
}
}
}
