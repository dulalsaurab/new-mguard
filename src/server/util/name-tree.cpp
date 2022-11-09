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
 
#include "name-tree.hpp"
#include "common.hpp"

#include <ndn-cxx/util/logger.hpp>

#include <string>
#include <utility>

NDN_LOG_INIT(mguard.nametree);

namespace mguard{
namespace util {
namespace nametree {

NameTree::NameTree()
{
  m_root = new TreeNode();
  m_root->m_nodeId = "/";
  m_root->m_fullName = ndn::Name("/");
}

void
NameTree::insertName(ndn::Name name)
{
  // check if the name already exist in the tree
  NDN_LOG_INFO("Trying to insert name: " << name);
  std::pair<TreeNode*, ndn::Name> info = getLongestMatchedName(m_root, name);
  auto prefixNotIn = info.second;
  auto parent = info.first;
  NDN_LOG_DEBUG("prefix not in tree: " << prefixNotIn << " parent: " << parent->m_fullName);
  if (prefixNotIn.toUri() == "/") {
    NDN_LOG_INFO("pointer id:" << parent->m_nodeId << " name: " << name << " already present in the tree");
  }
  else {
    auto fullName = ndn::Name(parent->m_fullName);
    auto startFrom = parent;
    for (auto it_name = prefixNotIn.begin(); it_name != prefixNotIn.end(); ++it_name)
    {
      NDN_LOG_INFO("Inserting name component: " << it_name->toUri());
      // auto m_nodeId = it_name->value();
      std::string nodeId(it_name->toUri());
      fullName = fullName.append(nodeId);
      auto child = createNode(nodeId, fullName);
      startFrom->m_children.push_back(child);
      startFrom = child;
    }
  }
}

TreeNode*
NameTree::createNode(std::string nodeId, const ndn::Name& fullName)
{
  std::cout << "create node " << fullName << std::endl;
  auto *parent_node = new TreeNode;
  parent_node->m_nodeId = std::move(nodeId);
  parent_node->m_fullName = fullName;
  return parent_node;
}

ndn::optional<TreeNode*>
NameTree::getNode(TreeNode* startFrom, ndn::Name name)
{
  std::pair<TreeNode*, ndn::Name> info = getLongestMatchedName(startFrom, name);
  return (info.second == "/") ? info.first : nullptr; // return the pointer that has the name
}

std::pair<TreeNode*, ndn::Name>
NameTree::getLongestMatchedName(TreeNode* startFrom, ndn::Name& namePrefix)
{
  NDN_LOG_INFO("Searching name prefix: " << namePrefix.toUri());
  if (namePrefix.toUri() == "/") 
    return std::make_pair(startFrom, namePrefix);

  for (auto it_name = namePrefix.begin(); it_name != namePrefix.end(); ++it_name) {
    NDN_LOG_INFO("Searching name component: " << it_name->toUri() << " : " << (*startFrom).m_nodeId);
    
    if ((*startFrom).m_children.empty()) {// this is leaf
      return std::make_pair(startFrom, namePrefix);
    }

    for (auto & it_nt : (*startFrom).m_children) {
      if (it_nt->m_nodeId == it_name->toUri()) {
        namePrefix = namePrefix.getSubName(1);
        return getLongestMatchedName(it_nt, namePrefix);
      }
    }
 
    return std::make_pair(startFrom, namePrefix);
  }
}

ndn::Name
NameTree::longestPrefixMatch(ndn::Name name)
{
  std::pair<TreeNode*, ndn::Name> info = getLongestMatchedName(m_root, name);
  auto lmp = info.first->m_fullName; // lmp longest matched prefix
  NDN_LOG_INFO("Longest matched prefix: " << lmp);
  return lmp;
}

void
NameTree::getAllLeaves(TreeNode* startFrom, std::vector<ndn::Name>& leafs, const ndn::Name& ignore)
{
  if ((*startFrom).m_children.empty())
    return;

  for (auto & it_nt : (*startFrom).m_children) {
    if(it_nt->m_children.empty()) // if no children then this is the leaf
        leafs.push_back(it_nt->m_fullName);

    if (it_nt->m_fullName != ignore)
        getAllLeaves(it_nt, leafs, ignore);
  }
}

std::vector<ndn::Name>
NameTree::getAllLeaves(ndn::Name prefix, const ndn::Name& ignore)
{
  std::vector<ndn::Name> leafs;
  auto node_ptr = getNode(m_root, std::move(prefix));
  if (node_ptr == nullptr) {
    NDN_LOG_INFO("prefix not in the tree");
    return leafs;
  }
    getAllLeaves((*node_ptr), leafs, ignore);
  return leafs;
}

void
NameTree::getAllChildren(TreeNode* startFrom, std::vector<ndn::Name>& childrens)
{
  if ((*startFrom).m_children.empty())
    return;
  
  for (auto & it_nt : (*startFrom).m_children) {
      childrens.push_back(it_nt->m_fullName);
      getAllChildren(it_nt, childrens);
  }
}

std::vector<ndn::Name>
NameTree::getAllChildren(ndn::Name name)
{
  std::vector<ndn::Name> children;
  auto node_ptr = getNode(m_root, std::move(name));
  if (node_ptr == nullptr) {
    NDN_LOG_INFO("prefix not in the tree");
    return children;
  }
    getAllChildren((*node_ptr), children);
  return children;
}

TreeNode*
NameTree::getParent(ndn::Name name)
{
  // parent of name (full name) "/aa/bb/cc" will be of full name "/aa/bb"
  auto parentPrefix = name.getPrefix(1);
  auto parent_ptr = getNode(m_root, parentPrefix);
  NDN_LOG_INFO("parent of prefix: " << name << " = " << (*parent_ptr)->m_nodeId);
  return *parent_ptr;
}

bool
NameTree::isChild(TreeNode *node, const ndn::Name& leaf) {
    return node == getParent(leaf);
}

void
NameTree::_delete(TreeNode* startFrom)
{
  if ((*startFrom).m_children.empty()) {
    delete startFrom;
    return;
  }
    /* first delete all the subtrees */
  for (auto & it_nt : (*startFrom).m_children) {
    _delete(it_nt);
  } 
  /* finally delete the parent node */
  delete startFrom;
}

void
NameTree::deleteNode(const ndn::Name& prefix) {
  auto node_ptr = getNode(m_root, prefix);
  NDN_LOG_INFO("Deleting from start: " << (*node_ptr)->m_nodeId);
  _delete(*node_ptr);

  auto parent = getParent(prefix);
  auto index = std::find((*parent).m_children.begin(), (*parent).m_children.end(), *node_ptr);

  if (index != (*parent).m_children.end()) {
    (*parent).m_children.erase(index);
  }
}

void
NameTree::_printTree(TreeNode* startFrom)
{ 
  NDN_LOG_INFO("node: " << (*startFrom).m_nodeId);
  if ((*startFrom).m_children.empty()) {
    return;
  }
  for (auto & it_nt : (*startFrom).m_children) {
    _printTree(it_nt);
  }
}

} // nametree
} // util
} // mguard