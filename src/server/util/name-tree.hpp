#include <ndn-cxx/name.hpp>
#include <string>
#include "../common.hpp"

namespace mguard {
namespace util {

struct node
{
 std::string name;
 node* right;
 node* left;
};

class NameTree()
{
public:
 NameTree();

 void
 insert(ndn::Name prefix);

//  void
//  delete(ndn::Name prefix);

//  void
//  search(ndn::Name prefix);

//  void
//  longestPrefixMatch();

//  void
//  isLeaf(ndn::Name prefix);

//  void
//  getAllchildren(nan::Name prefix);

private:
 node* root;
}

} // util
} // mguard