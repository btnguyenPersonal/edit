#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

class Node {
	public:
		Node* next;
		std::string data;
		bool end;
		Node();
};

class Rope {
	public:
		Node* head;
		uint32_t length;
		const uint32_t size() const;
		Node* get(uint32_t index);
		const Node* get(uint32_t index) const;
		Node* get_before(uint32_t index);
		void insert(uint32_t index, std::string data);
		void erase(uint32_t index);
		Rope* copy();
		std::string& operator[](uint32_t index);
		const std::string& operator[](uint32_t index) const;
		void push_back(std::string s);
		Rope();
};
