#include "./rope.h"

Node::Node()
{
	this->end = false;
	this->next = nullptr;
	this->data = "";
}

Rope::Rope()
{
	Node* last = new Node();
	last->end = true;
	Node* head = new Node();
	head->next = last;
	this->head = head;
	this->length = 0;
}

uint32_t Rope::size()
{
	return this->length;
}

Node* Rope::get(uint32_t index)
{
	Node* c = this->head;
	if (index >= this->length) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	for (uint32_t i = 0; i < index && i + 1 < this->length; i++) {
		c = c->next;
	}
	return c;
}

std::string Rope::operator[](uint32_t index)
{
	return (this->get(index))->data;
}

void Rope::push_back(std::string s)
{
	Node* c = this->head;
	for (uint32_t i = 0; i + 1 < this->length; i++) {
		c = c->next;
	}
	Node* n = new Node();
	n->next = c->next;
	n->data = s;
	c->next = n;
	this->length++;
}
