#include "MemoryResource.h"
#include "List.h"
#include <gtest/gtest.h>

// ============= MemoryResource Tests =============
TEST(MemoryResource, AllocDealloc) {
    MemoryResource mr(256);
    void* p = mr.allocate(64, 8);
    EXPECT_NE(p, nullptr);
    mr.deallocate(p, 64, 8);
}

TEST(MemoryResource, ReuseMemory) {
    MemoryResource mr(128);
    void* p1 = mr.allocate(64, 8);
    mr.deallocate(p1, 64, 8);
    void* p2 = mr.allocate(64, 8); // Должен переиспользовать
    EXPECT_NE(p2, nullptr);
    mr.deallocate(p2, 64, 8);
}

TEST(MemoryResource, OutOfMemory) {
    MemoryResource mr(100);
    void* p = mr.allocate(80, 8);
    EXPECT_THROW(mr.allocate(50, 8), std::bad_alloc);
    mr.deallocate(p, 80, 8);
}

TEST(MemoryResource, BadDealloc) {
    MemoryResource mr(256);
    void* p = mr.allocate(64, 8);
    mr.deallocate(p, 64, 8);
    EXPECT_THROW(mr.deallocate(p, 64, 8), std::runtime_error);
}

// ============= DoublyLinkedList Tests =============
TEST(DoublyLinkedList, EmptyList) {
    DoublyLinkedList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(DoublyLinkedList, PushPop) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list(&mr);
    
    list.push_back(1);
    list.push_back(2);
    list.push_front(0);
    
    EXPECT_EQ(list.size(), 3);
    
    list.pop_front();
    EXPECT_EQ(*list.begin(), 1);
    
    list.pop_back();
    EXPECT_EQ(list.size(), 1);
}

TEST(DoublyLinkedList, Iteration) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list(&mr);
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    int sum = 0;
    for (int val : list) sum += val;
    EXPECT_EQ(sum, 6);
}

TEST(DoublyLinkedList, Move) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list1(&mr);
    
    list1.push_back(1);
    list1.push_back(2);
    
    DoublyLinkedList<int> list2 = std::move(list1);
    EXPECT_TRUE(list1.empty());
    EXPECT_EQ(list2.size(), 2);
}

TEST(DoublyLinkedList, ComplexType) {
    MemoryResource mr(2048);
    DoublyLinkedList<std::string> list(&mr);
    
    list.push_back("Hello");
    list.push_back("World");
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(*list.begin(), "Hello");
}

TEST(DoublyLinkedList, Clear) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list(&mr);
    
    list.push_back(1);
    list.push_back(2);
    list.clear();
    
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

// ============= Integration Tests =============
TEST(Integration, SharedResource) {
    MemoryResource mr(2048);
    
    DoublyLinkedList<int> list1(&mr);
    DoublyLinkedList<std::string> list2(&mr);
    
    list1.push_back(42);
    list2.push_back("Test");
    
    EXPECT_EQ(list1.size(), 1);
    EXPECT_EQ(list2.size(), 1);
}

TEST(Integration, FullDemo) {
    MemoryResource mr(4096);
    
    // Test from assignment
    DoublyLinkedList<int> int_list(&mr);
    for (int i = 0; i < 5; ++i) int_list.push_back(i * 10);
    int_list.push_front(-10);
    
    EXPECT_EQ(int_list.size(), 6);
    
    struct ComplexType {
        int id;
        std::string name;
        double salary;
        ComplexType(int i = 0, std::string n = "", double s = 0.0)
            : id(i), name(std::move(n)), salary(s) {}
    };
    
    DoublyLinkedList<ComplexType> complex_list(&mr);
    complex_list.push_back(ComplexType(1, "Alice", 50000));
    complex_list.push_back(ComplexType(2, "Bob", 60000));
    
    EXPECT_EQ(complex_list.size(), 2);
    
    // Move test
    DoublyLinkedList<int> moved = std::move(int_list);
    EXPECT_TRUE(int_list.empty());
    EXPECT_EQ(moved.size(), 6);
    
    // Const iteration
    const DoublyLinkedList<ComplexType>& const_ref = complex_list;
    int id_sum = 0;
    for (auto it = const_ref.cbegin(); it != const_ref.cend(); ++it) {
        id_sum += it->id;
    }
    EXPECT_EQ(id_sum, 3); // 1 + 2
}

// ============= Requirements Check =============
TEST(Requirements, PolymorphicAllocator) {
    MemoryResource mr(256);
    DoublyLinkedList<int> list(&mr);
    list.push_back(42);
    EXPECT_EQ(*list.begin(), 42);
}

TEST(Requirements, ForwardIterator) {
    // Check iterator category
    using Iterator = DoublyLinkedList<int>::iterator;
    using Category = typename Iterator::iterator_category;
    static_assert(std::is_same<Category, std::forward_iterator_tag>::value,
                 "Must be forward iterator");
    EXPECT_TRUE(true); // Just to have an assertion
}

// ============= Main =============
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}