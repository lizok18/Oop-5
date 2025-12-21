#include "MemoryResource.h"
#include "List.h"
#include <gtest/gtest.h>
#include <type_traits>
#include <iostream>

TEST(MemoryResource, BasicAllocation) {
    MemoryResource mr(256);
    
    void* p1 = mr.allocate(32, 8);
    ASSERT_NE(p1, nullptr);
    
    void* p2 = mr.allocate(64, 8);
    ASSERT_NE(p2, nullptr);
    
    mr.deallocate(p1, 32, 8);
    mr.deallocate(p2, 64, 8);
}

TEST(MemoryResource, OutOfMemory) {
    MemoryResource mr(100);
    
    void* p1 = mr.allocate(80, 8);
    ASSERT_NE(p1, nullptr);
    
    
    EXPECT_THROW({
        void* p2 = mr.allocate(50, 8);
        (void)p2; 
    }, std::bad_alloc);
    
    mr.deallocate(p1, 80, 8);
}

TEST(MemoryResource, MemoryReuse) {
    MemoryResource mr(128);
    
    void* p1 = mr.allocate(32, 8);
    mr.deallocate(p1, 32, 8);
    
    // Должна переиспользоваться освобожденная память
    void* p2 = mr.allocate(32, 8);
    ASSERT_EQ(p1, p2); 
    
    mr.deallocate(p2, 32, 8);
}

TEST(Requirements, ForwardIterator) {
    // Проверяем, что итератор действительно является forward_iterator
    using Iterator = DoublyLinkedList<int>::iterator;
    using ConstIterator = DoublyLinkedList<int>::const_iterator;
    
    // Проверка категории итератора
    using Category = typename std::iterator_traits<Iterator>::iterator_category;
    static_assert(std::is_same<Category, std::forward_iterator_tag>::value,
                  "Must be forward iterator");
    
    using ConstCategory = typename std::iterator_traits<ConstIterator>::iterator_category;
    static_assert(std::is_same<ConstCategory, std::forward_iterator_tag>::value,
                  "Must be forward iterator");
    
    SUCCEED();
}

TEST(DoublyLinkedList, BasicOperations) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list(&mr);
    
    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    ASSERT_EQ(list.size(), 3);
    ASSERT_EQ(list.front(), 1);
    ASSERT_EQ(list.back(), 3);
    
    list.pop_front();
    ASSERT_EQ(list.front(), 2);
    ASSERT_EQ(list.size(), 2);
    
    list.clear();
    ASSERT_TRUE(list.empty());
}

TEST(DoublyLinkedList, Iterators) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list(&mr);
    
    for (int i = 0; i < 5; ++i) {
        list.push_back(i * 10);
    }
    
    // Forward iteration
    int expected = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, expected);
        expected += 10;
    }
    
    // Range-based for loop
    expected = 0;
    for (int value : list) {
        EXPECT_EQ(value, expected);
        expected += 10;
    }
    
    // Const iterators
    const auto& const_list = list;
    expected = 0;
    for (auto it = const_list.begin(); it != const_list.end(); ++it) {
        EXPECT_EQ(*it, expected);
        expected += 10;
    }
}

TEST(DoublyLinkedList, ComplexType) {
    struct TestStruct {
        int a;
        double b;
        std::string c;
        
        TestStruct(int a, double b, std::string c) : a(a), b(b), c(std::move(c)) {}
        
        bool operator==(const TestStruct& other) const {
            return a == other.a && b == other.b && c == other.c;
        }
    };
    
    MemoryResource mr(2048);
    DoublyLinkedList<TestStruct> list(&mr);
    
    list.push_back(TestStruct(1, 1.1, "first"));
    list.push_back(TestStruct(2, 2.2, "second"));
    list.push_back(TestStruct(3, 3.3, "third"));
    
    ASSERT_EQ(list.size(), 3);
    
    TestStruct expected(2, 2.2, "second");
    auto it = list.begin();
    ++it; // Второй элемент
    EXPECT_TRUE(*it == expected);
}

TEST(DoublyLinkedList, InsertErase) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list(&mr);
    
    list.push_back(10);
    list.push_back(30);
    
    auto it = list.begin();
    ++it; // Позиция перед 30
    
    it = list.insert(it, 20); // Вставляем между 10 и 30
    
    EXPECT_EQ(list.size(), 3);
    
    // Проверяем порядок
    auto check_it = list.begin();
    EXPECT_EQ(*check_it, 10);
    ++check_it;
    EXPECT_EQ(*check_it, 20);
    ++check_it;
    EXPECT_EQ(*check_it, 30);
    
    // Удаляем средний элемент
    it = list.begin();
    ++it;
    it = list.erase(it);
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(*list.begin(), 10);
    EXPECT_EQ(*++list.begin(), 30);
}

TEST(DoublyLinkedList, MoveOperations) {
    MemoryResource mr(1024);
    DoublyLinkedList<int> list1(&mr);
    
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    // Move constructor
    DoublyLinkedList<int> list2(std::move(list1));
    
    EXPECT_TRUE(list1.empty());
    EXPECT_EQ(list2.size(), 3);
    
    // Move assignment
    DoublyLinkedList<int> list3(&mr);
    list3 = std::move(list2);
    
    EXPECT_TRUE(list2.empty());
    EXPECT_EQ(list3.size(), 3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}