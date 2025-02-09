#include "container.h"

namespace utils {

    // Default constructor initializing the container with zero capacity and size
    template<class T>
    Container<T>::Container() : capacity(0), sizee(0) {}

    // Constructor initializing the container with a specified capacity
    template<class T>
    Container<T>::Container(int c) : capacity(c), sizee(c) {
        data = make_unique<T[]>(capacity);  // Allocating memory for the container's elements
    }

    // Constructor initializing the container with a specified capacity and an initial value
    template<class T>
    Container<T>::Container(int c, const T& in) : capacity(c), sizee(c) {
        data = make_unique<T[]>(capacity);
        for (int i = 0; i < sizee; i++) {
            data[i] = in;  // Initializing all elements with the provided value
        }
    }

    // Copy constructor creating a new container by copying data from another container
    template<class T>
    Container<T>::Container(const Container& other) : sizee(other.sizee), capacity(other.capacity) {
        data = make_unique<T[]>(capacity);
        for (int i = 0; i < sizee; i++) {
            data[i] = other.data[i];  // Copying each element from the other container
        }
    }

    // Move constructor transferring ownership of data from another container
    template<class T>
    Container<T>::Container(Container&& other) noexcept : sizee(other.sizee), capacity(other.capacity) {
        data = move(other.data);  // Moving the data pointer
        other.sizee = 0;  // Resetting the moved container
        other.capacity = 0;  // Resetting the moved container
    }

    // Copy assignment operator that copies elements from another container
    template<class T>
    Container<T>& Container<T>::operator=(const Container& other) {
        if (this != &other) {  // Check for self-assignment
            sizee = other.sizee;
            capacity = other.capacity;
            data = make_unique<T[]>(capacity);  // Allocate new memory for data
            for (int i = 0; i < sizee; i++) {
                data[i] = other.data[i];  // Copy each element
            }
        }
        return *this;  // Returning the updated container
    }

    // Move assignment operator transferring ownership of resources from another container
    template<class T>
    Container<T>& Container<T>::operator=(Container&& other) noexcept {
        if (this != &other) {  // Check for self-assignment
            data = move(other.data);  // Move data ownership
            sizee = other.sizee;
            capacity = other.capacity;
            other.sizee = 0;  // Resetting the moved container
            other.capacity = 0;  // Resetting the moved container
        }
        return *this;  // Returning the updated container
    }

    // Overloading the [] operator to access an element at the given index (non-const version)
    template<class T>
    T& Container<T>::operator[](int index) {
        if (index < 0 || index >= sizee) {  // Validate the index
            throw out_of_range("");  // Throw an exception if the index is out of range
        }
        return data[index];  // Return the element at the specified index
    }

    // Overloading the [] operator to access an element at the given index (const version)
    template<class T>
    T Container<T>::operator[](int index) const {
        if (index < 0 || index >= sizee) {  // Validate the index
            throw out_of_range("");  // Throw an exception if the index is out of range
        }
        return data[index];  // Return the element at the specified index
    }

    // Function to erase an element at a specified position
    template <class T>
    auto Container<T>::erase(T* pos) {
        if (pos == end())  // If the position is at the end, return immediately
            return pos;
        auto next = pos + 1;
        while (next != end()) {  // Move elements one position forward to overwrite the erased element
            *pos = std::move(*next);
            ++pos;
            ++next;
        }
        sizee--;
        return pos;  // Return the position of the erased element
    }

    // Function to add an element at the end of the container
    template<class T>
    void Container<T>::push_back(const T& value) {
        if (sizee == capacity) {  // If the container is full, double the capacity
            capacity = (capacity == 0) ? 1 : capacity * 2;
            auto newData = make_unique<T[]>(capacity);  // Allocate a new array with the new capacity
            copy(data.get(), data.get() + sizee, newData.get());  // Copy existing elements to the new array
            data = move(newData);  // Move the new data to the container
        }
        data[sizee++] = value;  // Add the new element to the end
    }

    // Function to return an iterator to the beginning of the container
    template<class T>
    auto Container<T>::begin() {
        return data.get();  // Return the raw pointer to th e beginning of the data
    }

    // Function to return an iterator to the end of the container
    template<class T>
    auto Container<T>::end() {
        return data.get() + sizee;  // Return the raw pointer to the end of the data
    }

    // Function to return the number of elements in the container
    template<class T>
    int Container<T>::size() const {
        return sizee;  // Return the size of the container
    }

    // Function to clear the container (remove all elements)
    template<class T>
    void Container<T>::clear() {
        sizee = 0;  // Reset the size to 0, effectively clearing the container
    }

}
