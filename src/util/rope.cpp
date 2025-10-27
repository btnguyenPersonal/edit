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

const uint32_t Rope::size() const
{
	return this->length;
}

Node* Rope::get_before(uint32_t index)
{
	Node* c = this->head;
	if (index < 0) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	if (index >= this->length) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	for (uint32_t i = 0; i + 1 < index && i + 1 < this->length; i++) {
		c = c->next;
	}
	return c;
}

Node* Rope::get(uint32_t index)
{
	Node* c = this->head;
	if (index < 0) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	if (index >= this->length) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	for (uint32_t i = 0; i < index && i + 1 < this->length; i++) {
		c = c->next;
	}
	return c;
}

const Node* Rope::get(uint32_t index) const
{
	Node* c = this->head;
	if (index < 0) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	if (index >= this->length) {
		throw std::out_of_range("Rope: Index out of bounds");
	}
	for (uint32_t i = 0; i < index && i + 1 < this->length; i++) {
		c = c->next;
	}
	return c;
}

void Rope::insert(uint32_t index, std::string data)
{
	Node* b = this->get_before(index);
	Node* n = new Node();
	n->data = data;
	n->next = b->next;
	b->next = n;
	this->length++;
}

void Rope::erase(uint32_t index)
{
	Node* b = this->get_before(index);
	if (!b->next->end) {
		auto temp = b->next->next;
		delete b->next;
		b->next = temp;
	}
	this->length--;
}

const std::string& Rope::operator[](uint32_t index) const
{
	return (this->get(index))->data;
}

std::string& Rope::operator[](uint32_t index)
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
