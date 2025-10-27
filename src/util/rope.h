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
		uint32_t size();
		Node* get(uint32_t index);
		std::string operator[](uint32_t index);
		void push_back(std::string s);
		Rope();
};
