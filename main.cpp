#include <iostream>
#include <string>
#include "MemoryResource.h"
#include "List.h"

struct ComplexType
{
    int id;
    std::string name;
    double salary;

    ComplexType(int i = 0, std::string n = "", double s = 0.0)
        : id(i), name(std::move(n)), salary(s) {}

    friend std::ostream &operator<<(std::ostream &os, const ComplexType &ct)
    {
        return os << "{" << ct.id << ", \"" << ct.name << "\", " << ct.salary << "}";
    }
};

int main()
{
    try
    {
        MemoryResource mr(4096); 
        
        std::cout << "1. DoublyLinkedList<int>:" << std::endl;
        DoublyLinkedList<int> int_list(&mr);

        for (int i = 0; i < 5; ++i)
        {
            int_list.push_back(i * 10);
        }
        int_list.push_front(-10);

        std::cout << "   Элементы: ";
        for (int n : int_list)
        {
            std::cout << n << " ";
        }
        std::cout << "\n   Размер: " << int_list.size() << std::endl;
        
        int_list.pop_front();
        int_list.pop_back();

        std::cout << "\n   После удаления первого и последнего: ";
        for (int n : int_list)
        {
            std::cout << n << " ";
        }
        std::cout << "\n   Размер: " << int_list.size() << std::endl;
        
        std::cout << "\n2. DoublyLinkedList<ComplexType>:" << std::endl;
        DoublyLinkedList<ComplexType> complex_list(&mr);

        complex_list.push_back(ComplexType(1, "Alice", 50000.0));
        complex_list.push_back(ComplexType(2, "Bob", 60000.0));
        complex_list.push_front(ComplexType(0, "Admin", 100000.0));

        std::cout << "   Элементы:" << std::endl;
        for (const auto &item : complex_list)
        {
            std::cout << "   - " << item << std::endl;
        }
        
        std::cout << "\n3. Перемещение списка:" << std::endl;
        DoublyLinkedList<int> moved_list = std::move(int_list);

        std::cout << "   Перемещенный список: ";
        for (int n : moved_list)
        {
            std::cout << n << " ";
        }
        std::cout << "\n   Размер оригинального: " << int_list.size() << std::endl;
        std::cout << "   Размер перемещенного: " << moved_list.size() << std::endl;
        
        std::cout << "\n4. Const итераторы:" << std::endl;
        const DoublyLinkedList<ComplexType> &const_ref = complex_list;
        std::cout << "   Через const ссылку: ";
        for (auto it = const_ref.cbegin(); it != const_ref.cend(); ++it)
        {
            std::cout << it->id << " ";
        }
        std::cout << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\nОшибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}