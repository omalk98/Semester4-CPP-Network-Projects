#include <iostream>
#include <string>
#include "vectors.h"

//constructor for an empty Vector
Vector::Vector() {
	this->name = "";
	this->next = nullptr;
	this->size = 0;
	this->elements = nullptr;
}

//constructor for a named Vector
Vector::Vector(std::string name) {
	this->name = name;
	this->next = nullptr;
	std::cout << "Setting vector: " << name << std::endl;
	std::cout << "Enter vector size: ";
	std::cin >> this->size;
	this->elements = new int[this->size];
	for (int i = 0; i < this->size; i++) {
		std::cout << "Enter element " << i << " : ";
		std::cin >> this->elements[i];
	}
}

//destructor for Vector, note that it frees up *elements
Vector::~Vector() {
	std::cout << "Deleted vector: " << this->name << std::endl;
	delete[] this->elements;
}

//constructor for List
List::List() {
	head = nullptr;
	current = nullptr;
}

List::~List() {
	current = head;
	while (current)
	{
		head = head->next;
		delete current;
		current = head;
	}
}

void List::add_vector(std::string name) {
	if (!head) {
		head = new Vector(name);
	}
	else if (!head->next) {
		head->next = new Vector(name);
	}
	else {
		current = head;
		while (current)
		{
			if (!current->next) {
				current->next = new Vector(name);
				break;
			}
			current = current->next;
		}
	}
}

bool List::remove_vector(std::string name) {
	if (!head) return false;
	current = head;
	Vector *vec = current;
	while (current) {
		if (current->name == name) {
			if (current->name == head->name)
				head = head->next;
			else
				vec->next = current->next;
			delete current;
			current = nullptr;
			return true;
		}
		vec = current;
		current = current->next;
	}
	return false;
}

bool List::append_vector(std::string name) {
	if (!head) return false;
	int newSize = 0;
	int* newElements = nullptr;
	current = head;
	while (current) {
		if (current->name == name) {
			std::cout << "Appending to vector: " << name << std::endl;
			std::cout << "Enter vector size to add: ";
			std::cin >> newSize;
			newElements = new int[newSize + current->size];

			for (int i = 0; i < current->size; i++)
				newElements[i] = current->elements[i];

			for (int i = current->size; i < newSize + current->size; i++) {
				std::cout << "Enter element " << i << " : ";
				std::cin >> newElements[i];
			}
			delete[] current->elements;
			current->elements = newElements;
			current->size += newSize;
			return true;
		}
		current = current->next;
	}
	return false;
}

bool List::print_vector(std::string name) {
	if (!head) return false;
	current = head;
	while (current) {
		if (current->name == name) {
			std::cout << current->name << " : ";
			for(int i = 0; i < current->size; ++i)
				std::cout << current->elements[i] << ((i == current->size - 1) ? "" : ", ");
			std::cout << std::endl;
			return true;
		}
		current = current->next;
	}
	return false;
}