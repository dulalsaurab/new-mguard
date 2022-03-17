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

BOOST_AUTO_TEST_CASE(Constructor)
{
    bool _run_nametree_test(){
    uint8_t nametree_buf[NDN_NAMETREE_RESERVE_SIZE(10)];
    ndn_nametree_t *nametree = (ndn_nametree_t*)nametree_buf;

    ndn_nametree_init(nametree, 10);

    // Functional Test
    // Insert
    uint8_t name1[] = "\x07\x17\x08\x03ndn\x08\x09name-tree\x08\x05test1";
    nametree_entry_t *ptr1 = ndn_nametree_find_or_insert(nametree, name1, strlen((char*)name1));
    CU_ASSERT_PTR_NOT_NULL(ptr1);

    uint8_t name2[] = "\x07\x17\x08\x03ndn\x08\x09name-tree\x08\x05test2";
    nametree_entry_t *ptr2 = ndn_nametree_find_or_insert(nametree, name2, strlen((char*)name2));
    CU_ASSERT_PTR_NOT_NULL(ptr2);

    uint8_t name3[] = "\x07\x17\x08\x03ndn\x08\x09name-tree\x08\x05test3";
    nametree_entry_t *ptr3 = ndn_nametree_find_or_insert(nametree, name3, strlen((char*)name3));
    CU_ASSERT_PTR_NOT_NULL(ptr3);

    nametree_entry_t *ptr4 = ndn_nametree_find_or_insert(nametree, name2, strlen((char*)name2));
    CU_ASSERT_PTR_EQUAL(ptr4, ptr2);

    uint8_t name5[] = "\x07\x10\x08\x03ndn\x08\x09name-tree";
    nametree_entry_t *ptr5 = ndn_nametree_find_or_insert(nametree, name5, strlen((char*)name5));
    CU_ASSERT_PTR_NOT_NULL(ptr5);
    CU_ASSERT_PTR_NOT_EQUAL(ptr5->left_child, NDN_INVALID_ID);

    // Match
    nametree_entry_t *ptr6 = ndn_nametree_prefix_match(nametree, name2, strlen((char*)name2), NDN_NAMETREE_FIB_TYPE);
    CU_ASSERT_PTR_NULL(ptr6);

    ptr5->fib_id = 1;
    ptr6 = ndn_nametree_prefix_match(nametree, name2, strlen((char*)name2), NDN_NAMETREE_FIB_TYPE);
    CU_ASSERT_PTR_EQUAL(ptr6, ptr5);

    ptr6 = ndn_nametree_prefix_match(nametree, name2, strlen((char*)name2), NDN_NAMETREE_PIT_TYPE);
    CU_ASSERT_PTR_NULL(ptr6);

    ptr5->pit_id = 1;
    ptr2->pit_id = 2;
    ptr6 = ndn_nametree_prefix_match(nametree, name2, strlen((char*)name2), NDN_NAMETREE_PIT_TYPE);
    CU_ASSERT_PTR_EQUAL(ptr6, ptr2);

    // Autoclear test
    int i;
    uint8_t name20[] = "\x07\x03\x08\x01\x00";
    ndn_nametree_init(nametree, 10);
    for(i = 0; i < 10 - 4; i ++){
        name20[4] = i;
        ptr1 = ndn_nametree_find_or_insert(nametree, name20, strlen((char*)name20));
        ptr1->fib_id = 0;
    }
    uint8_t name21[] = "\x07\x10\x08\x03ndn\x08\x09name-tree";
    ptr1 = ndn_nametree_find_or_insert(nametree, name21, strlen((char*)name21));
    CU_ASSERT_PTR_NOT_NULL(ptr1);
    ptr1->fib_id = 1;

    uint8_t name22[] = "\x07\x10\x08\x03nbn\x08\x09name-tree";
    ptr1 = ndn_nametree_find_or_insert(nametree, name22, strlen((char*)name22));
    CU_ASSERT_PTR_NULL(ptr1);

    ptr1 = ndn_nametree_find_or_insert(nametree, name21, strlen((char*)name21));
    CU_ASSERT_PTR_NOT_NULL(ptr1);
    ptr1->fib_id = NDN_INVALID_ID;

    ptr1 = ndn_nametree_find_or_insert(nametree, name22, strlen((char*)name22));
    CU_ASSERT_PTR_NOT_NULL(ptr1);

    return true;
    }

}

BOOST_AUTO_TEST_SUITE_END() //TestDataAdapter

} // tests
} // mguard
