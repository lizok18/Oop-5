#include "MemoryResource.h"
#include "List.h"
#include <iostream>
#include <string>

// Сложный тип для демонстрации
struct ComplexType {
    int id;
    std::string name;
    double value;
    
    ComplexType(int i, std::string n, double v) : id(i), name(std::move(n)), value(v) {}
    
    friend std::ostream& operator<<(std::ostream& os, const ComplexType& ct) {
        return os << "ComplexType{id=" << ct.id << ", name=\"" << ct.name 
                  << "\", value=" << ct.value << "}";
    }
};

void demonstrate_with_int() {
    std::cout << "\n=== Demonstrating with int ===" << std::endl;
    
    // Создаем MemoryResource с 1024 байтами
    MemoryResource mr(1024);
    
    // Создаем список с использованием MemoryResource
    DoublyLinkedList<int> list(&mr);
    
    // Добавляем элементы
    for (int i = 0; i < 5; ++i) {
        list.push_back(i * 10);
    }
    
    // Используем итераторы (только forward)
    std::cout << "List contents (forward iteration): ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // Используем range-based for loop
    std::cout << "List contents (range-based for): ";
    for (const auto& val : list) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Удаляем элементы
    list.pop_front();
    list.pop_back();
    
    std::cout << "After pop_front and pop_back: ";
    for (const auto& val : list) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    mr.dump();
}

void demonstrate_with_complex_type() {
    std::cout << "\n=== Demonstrating with ComplexType ===" << std::endl;
    
    MemoryResource mr(2048);
    DoublyLinkedList<ComplexType> list(&mr);
    
    // Добавляем сложные объекты
    list.push_back(ComplexType(1, "First", 1.1));
    list.push_back(ComplexType(2, "Second", 2.2));
    list.push_back(ComplexType(3, "Third", 3.3));
    
    // Используем range-based for loop
    std::cout << "List contents:" << std::endl;
    for (const auto& item : list) {
        std::cout << "  " << item << std::endl;
    }
    
    // Демонстрация вставки и удаления
    auto it = list.begin();
    ++it; // Второй элемент
    
    it = list.insert(it, ComplexType(99, "Inserted", 9.9));
    std::cout << "\nAfter insertion:" << std::endl;
    for (const auto& item : list) {
        std::cout << "  " << item << std::endl;
    }
    
    list.erase(it);
    std::cout << "\nAfter erasure:" << std::endl;
    for (const auto& item : list) {
        std::cout << "  " << item << std::endl;
    }
    
    mr.dump();
}

void demonstrate_iterator_operations() {
    std::cout << "\n=== Demonstrating iterator operations (forward only) ===" << std::endl;
    
    MemoryResource mr(512);
    DoublyLinkedList<std::string> list(&mr);
    
    list.push_back("One");
    list.push_back("Two");
    list.push_back("Three");
    list.push_back("Four");
    list.push_back("Five");
    
    // Forward iteration с использованием преинкремента
    std::cout << "Forward iteration (pre-increment): ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // Forward iteration с использованием постинкремента
    std::cout << "Forward iteration (post-increment): ";
    auto it = list.begin();
    while (it != list.end()) {
        std::cout << *it << " ";
        it++;
    }
    std::cout << std::endl;
    
    // Демонстрация сравнения итераторов
    std::cout << "Iterator comparison: ";
    auto it1 = list.begin();
    auto it2 = list.begin();
    ++it2;
    
    if (it1 != it2) {
        std::cout << "it1 != it2 (correct)" << std::endl;
    }
    
    if (it1 == list.begin()) {
        std::cout << "it1 == list.begin() (correct)" << std::endl;
    }
    
    // Доступ через оператор ->
    std::cout << "Access via operator->: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << it->size() << " "; // размер строки
    }
    std::cout << std::endl;
}

void demonstrate_memory_reuse() {
    std::cout << "\n=== Demonstrating memory reuse ===" << std::endl;
    
    MemoryResource mr(256);
    mr.dump();
    
    // Аллоцируем несколько блоков
    void* p1 = mr.allocate(32, 8);
    void* p2 = mr.allocate(64, 8);
    void* p3 = mr.allocate(16, 8);
    
    mr.dump();
    
    // Освобождаем блоки в разном порядке
    mr.deallocate(p2, 64, 8);
    mr.dump();
    
    mr.deallocate(p1, 32, 8);
    mr.dump();
    
    mr.deallocate(p3, 16, 8);
    mr.dump();
    
    // Показываем, что память переиспользуется
    std::cout << "\nAllocating again to show reuse:" << std::endl;
    void* p4 = mr.allocate(100, 8);
    mr.dump();
    mr.deallocate(p4, 100, 8);
}

void demonstrate_container_with_different_allocators() {
    std::cout << "\n=== Demonstrating container with different allocators ===" << std::endl;
    
    // Создаем два разных MemoryResource
    MemoryResource mr1(512);
    MemoryResource mr2(512);
    
    std::cout << "\nList 1 using MemoryResource 1:" << std::endl;
    DoublyLinkedList<int> list1(&mr1);
    for (int i = 0; i < 3; ++i) {
        list1.push_back(i * 100);
    }
    
    std::cout << "List 1 contents: ";
    for (const auto& val : list1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nList 2 using MemoryResource 2:" << std::endl;
    DoublyLinkedList<int> list2(&mr2);
    for (int i = 0; i < 3; ++i) {
        list2.push_back(i * 200);
    }
    
    std::cout << "List 2 contents: ";
    for (const auto& val : list2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nMemoryResource 1 status:" << std::endl;
    mr1.dump();
    
    std::cout << "\nMemoryResource 2 status:" << std::endl;
    mr2.dump();
}

int main() {
    try {
        std::cout << "=== MemoryResource and DoublyLinkedList Demo ===" << std::endl;
        
        demonstrate_with_int();
        demonstrate_with_complex_type();
        demonstrate_iterator_operations();
        demonstrate_memory_reuse();
        demonstrate_container_with_different_allocators();
        
        std::cout << "\n=== All demonstrations completed successfully ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}