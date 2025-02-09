#ifndef CONTAINER_H
#define CONTAINER_H

#include <memory>
#include <stdexcept>

using namespace std;

namespace utils {

// Template class for a container that holds elements of type T
template<class T>
class Container{
    public:
    // Default constructor that initializes the container
    Container();

    // Constructor that initializes the container with a specified capacity
    Container(int c);

    // Constructor that initializes the container with a specified capacity and initial value
    Container(int c, const T& in);

    // Copy constructor that creates a copy of another container
    Container(const Container& other);
     
    // Copy assignment operator that assigns values from another container
    Container& operator=(const Container& other);

    // Move constructor that transfers ownership of resources from another container
    Container(Container&& other) noexcept;

    // Move assignment operator that transfers ownership of resources from another container
    Container& operator=(Container&& other) noexcept;
   
    // Index operator to access an element by index (non-const version)
    T& operator[](int index);
    
    // Index operator to access an element by index (const version)
    T operator[](int index) const ;

    // Erase an element at the specified position
    auto erase(T* pos);

    // Add an element at the end of the container
    void push_back(const T& in);

    // Return an iterator to the beginning of the container
    auto begin();

    // Return an iterator to the end of the container
    auto end();
        
    // Return the number of elements in the container
    int size() const;

    // Clear all elements from the container
    void clear();

    private:
        unique_ptr<T[]> data; // Unique pointer to hold the data array
        int sizee; // The current size of the container
        int capacity; // The total capacity of the container
};

}

#endif
