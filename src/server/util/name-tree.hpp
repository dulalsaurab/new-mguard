/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021-2022,  The University of Memphis
 *
 * This file is part of mGuard.
 * Author: Saurab Dulal <dulal.saurab@gmail.com>
 *
 * mGuard is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mGuard is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * mGuard, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef MGUARD_UTIL_NAMETREE_HPP
#define MGUARD_UTIL_NAMETREE_HPP

#include <ndn-cxx/name.hpp>
#include <string>

namespace mguard {
namespace util {
namespace nametree {

// const ndn::Name ignoreDefaultPrefix("/mguard/ignore");

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

  /** 
   * @brief Search if a name exist in the tree, return node pointer if the name is found
   *  @param startFrom node pointer, starting node of the search
   *  @param name name prefix to search for
  */
  ndn::optional<TreeNode*>
  search(TreeNode* startFrom, ndn::Name name);

  /** 
   * @brief get all the leaf (names) of a name prefix in the tree
   *  @param name name to get leafs for
   *  for the above example, if the prefix is aa, this function will return /aa/bb,
   *  /aa/ff/kk, /aa/ff/kk/mm/cc
  */
  std::vector<ndn::Name>
  getAllLeafs(ndn::Name prefix, ndn::Name ignore = "/mguard/ignore");
  
  TreeNode*
  getParent(ndn::Name name);

  /** 
   * @brief get all the childrens (names) of a name prefix in the tree
   *  @param name name to get childrens for
   *  for the above example, if the name prefix is aa, this function will return /aa/bb,
   *  /aa/ff, /aa/ff/kk, /aa/ff/kk/mm, /aa/ff/kk/mm/cc
  */
  std::vector<ndn::Name>
  getAllChildrens(ndn::Name name);

  /* return logest match prefix of a name */
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
  getLeafs(TreeNode* startFrom, std::vector<ndn::Name>& leafs, ndn::Name ignore);

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