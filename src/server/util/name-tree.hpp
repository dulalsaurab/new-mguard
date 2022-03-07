#include <ndn-cxx/name.hpp>
#include <string>
#include "../common.hpp"

#define NDN_NAME_COMPONENT_BLOCK_SIZE 38
typedef uint16_t ndn_table_id_t;


namespace mguard {
namespace util {


typedef struct nametree_entry{
  uint8_t val[NDN_NAME_COMPONENT_BLOCK_SIZE];
  struct nametree_entry* sub; /// Subtree
  struct nametree_entry* cop[2]; /// Child or parent
  ndn_table_id_t pit_id;
  ndn_table_id_t fib_id;
} nametree_entry_t;


typedef struct ndn_nametree{
  nametree_entry_t *nil, *root;
  nametree_entry_t pool[];
}ndn_nametree_t;


class NameTree()
{
  public:

  public:
  NameTree(uint8_t val[NDN_NAME_COMPONENT_BLOCK_SIZE],
            ndn_table_id_t left_child,
            ndn_table_id_t right_bro,
            ndn_table_id_t pit_id,
            ndn_table_id_t cs_id,
            ndn_table_id_t fib_i
            );


  typedef nametree_entry_t ndn_nametree_t[];

  #define NDN_NAMETREE_RESERVE_SIZE(entry_count) (sizeof(nametree_entry_t) * (entry_count))

  void
  ndn_nametree_init(void* memory, ndn_table_id_t capacity);

  nametree_entry_t*
  ndn_nametree_find_or_insert(ndn_nametree_t* nametree, uint8_t name[], size_t len);

  nametree_entry_t*
  ndn_nametree_prefix_match(
    ndn_nametree_t* nametree,
    uint8_t name[],
    size_t len,
    enum NDN_NAMETREE_ENTRY_TYPE type);

  nametree_entry_t*
  ndn_nametree_find(ndn_nametree_t *nametree, uint8_t name[], size_t len);

  nametree_entry_t*
  ndn_nametree_at(ndn_nametree_t *self, ndn_table_id_t id);

  ndn_table_id_t
  ndn_nametree_getid(ndn_nametree_t *self, nametree_entry_t* entry);

}

} // util
} // mguard