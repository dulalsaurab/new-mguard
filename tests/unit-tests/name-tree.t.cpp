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
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/bb/cc").value()->m_fullName == "/aa/bb/cc");
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/cc") == nullptr);
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/ff/mm") != nullptr);
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/ke") != nullptr);
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/ff") != nullptr);
    BOOST_CHECK(nameTree.getNode(nameTree.getTreeRoot(), "/aa/ff/kk/bb") == nullptr);

    // findNode
    BOOST_CHECK(nameTree.findNode("bb")->m_fullName.equals("/aa/bb"));
    BOOST_CHECK(nameTree.findNode("ff")->m_fullName.equals("/aa/ff"));
    BOOST_CHECK(nameTree.findNode("mm")->m_fullName.equals("/aa/ff/mm"));
    BOOST_CHECK(nameTree.findNode("ke")->m_fullName.equals("/aa/ke"));

    // getLeaves
    for (const ndn::Name &name : nameTree.getLeaves("/aa/ff", {})) {
        BOOST_CHECK(
                name.equals("/aa/ff/mm/cc") ||
                name.equals("/aa/ff/kk")
                );
    }

    // getParent
    BOOST_CHECK(((ndn::Name)"/aa").equals(nameTree.getParent("/aa/ke")->m_fullName));
    BOOST_CHECK(((ndn::Name)"/aa/ff/mm").equals(nameTree.getParent("/aa/ff/mm/cc")->m_fullName));
    BOOST_CHECK(((ndn::Name)"/aa/ff").equals(nameTree.getParent("/aa/ff/mm")->m_fullName));

    // getChildren
    for (const ndn::Name &child : nameTree.getChildren("/aa")) {
        BOOST_CHECK(
                child.equals("/aa/bb") ||
                child.equals("/aa/ff") ||
                child.equals("/aa/ke")
                );
    }

    // longestPrefixMatch
    BOOST_CHECK(nameTree.getLongestPrefixMatch("/aa/bb/aa").equals("/aa/bb"));
    BOOST_CHECK(nameTree.getLongestPrefixMatch("/aa/bb/aa/bb").equals("/aa/bb"));
    BOOST_CHECK(nameTree.getLongestPrefixMatch("/aa/ff/mm/cc/dd").equals("/aa/ff/mm/cc"));

    // isDirectChild
    BOOST_CHECK(nameTree.isDirectChild(nameTree.getTreeRoot(), "/aa"));
    BOOST_CHECK(nameTree.isDirectChild(nameTree.getNode(nameTree.getTreeRoot(), "/aa/bb").value(), "/aa/bb/cc"));
    BOOST_CHECK(nameTree.isDirectChild(nameTree.getNode(nameTree.getTreeRoot(), "/aa/bb").value(), "/aa/bb/dd"));

    // deleteNode
    nameTree.deleteNode("/aa/ke");
    for (const ndn::Name &name : nameTree.getLeaves("/aa", {})) {
        BOOST_CHECK(
                !name.equals("/aa/ke")
        );
    }
    nameTree.insertName("/aa/ke");

    nameTree.deleteNode("/aa/bb");
    for (const ndn::Name &name : nameTree.getLeaves("/aa", {})) {
        BOOST_CHECK(
                name.equals("/aa/ff/mm/cc") ||
                name.equals("/aa/ff/kk") ||
                name.equals("/aa/ke")
        );
    }

}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter
}
}
}
}
