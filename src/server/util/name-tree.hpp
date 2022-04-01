/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022-
 *
 * This implementation is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Author: Saurab Dulal <dulal.saurab@gmail.com>
 */

#ifndef MGUARD_UTIL_NAMETREE_HPP
#define MGUARD_UTIL_NAMETREE_HPP

#include <ndn-cxx/name.hpp>
#include <string>

namespace mguard {
namespace util {
namespace nametree {

struct TreeNode
{
  std::vector<TreeNode*> m_children;
  std::string m_nodeId; // this is the last name component e.g. for name /aa/bb/cc this will be cc
  ndn::Name m_fullName; // this is complete name from root /aa/bb/cc
};

/*
  
  m-arrary tree or general tree, used to store hierarchical ndn names
  e.g. inserting names /aa/bb/cc, /aa/ff/kk, /aa/ff/mm/cc will result in the following tree structure

                                        / -> root of the tree
                                        |
                                        aa  (e.g. m_fullName = /aa)
                                  ++++++++++++++
                                  |            |
                                  bb           ff
                                        +++++++++++++++
                                        |             |
                                        kk            mm
                                                       |
                                                       cc (e.g. m_fullname = /aa/ff/mm/cc)


*/
class NameTree
{
public:
  NameTree();
  
  TreeNode*
  getTreeRoot()
  {
    return m_root;
  }

  void
  insertName(ndn::Name name);

  ndn::optional<TreeNode*>
  search(TreeNode* startFrom, ndn::Name name);

  std::vector<ndn::Name>
  getAllLeafs(ndn::Name prefix);
  
  TreeNode*
  getParent(ndn::Name name);

  std::vector<ndn::Name>
  getAllChildrens(ndn::Name name);

  ndn::Name
  longestPrefixMatch(ndn::Name name);

  void
  deleteNode(ndn::Name prefix);

  void
  _printTree(TreeNode* startFrom);

private:
  TreeNode*
  createNode(std::string nodeId, ndn::Name fullName);

  void
  getLeafs(TreeNode* startFrom, std::vector<ndn::Name>& leafs);

  std::pair<TreeNode*, ndn::Name>
  getLongestMatchedName(TreeNode* startFrom, ndn::Name& namePrefix);

  void
  getChildrens(TreeNode* startFrom, std::vector<ndn::Name>& leafs);

  void
  _delete(TreeNode* nodeptr);

private:
  TreeNode* m_root;
};
} // nametree
} // util
} // mguard

#endif