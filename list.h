#ifndef __LIST_H
#define __LIST_H

// ListHead structure to keep track of the first and last nodes
template <typename T> struct ListHead {
  T *first;
  T *last;

  ListHead() : first(nullptr), last(nullptr) {}
};

// Node structure for doubly linked list
template <typename T> struct Node {
  T data;
  Node *prev;
  Node *next;

  Node(const T &data) : data(data), prev(nullptr), next(nullptr) {}
};

// DoubleLinkedList class definition
template <typename T> struct DoubleLinkedList {
  ListHead<Node<T>> head;

  DoubleLinkedList() = default;
  ~DoubleLinkedList();

  Node<T> *push_front(const T &data);
  Node<T> *push_back(const T &data);
  bool empty();
  void delete_node(Node<T> *node);
};

#define FOR_EACH_NODE(list, node)                                              \
  for (auto node = list.head.first; node != nullptr; node = node->next)

template <typename T> DoubleLinkedList<T>::~DoubleLinkedList() {
  Node<T> *current = head.first;
  Node<T> *next;
  while (current != nullptr) {
    next = current->next;
    delete current;
    current = next;
  }
}

template <typename T> Node<T> *DoubleLinkedList<T>::push_front(const T &data) {
  Node<T> *newNode = new Node<T>(data);
  newNode->next = head.first;
  if (head.first != nullptr) {
    head.first->prev = newNode;
  } else {
    head.last = newNode;
  }
  head.first = newNode;
  return newNode;
}

template <typename T> Node<T> *DoubleLinkedList<T>::push_back(const T &data) {
  Node<T> *newNode = new Node<T>(data);
  newNode->prev = head.last;
  if (head.last != nullptr) {
    head.last->next = newNode;
  } else {
    head.first = newNode;
  }
  head.last = newNode;
  return newNode;
}

template <typename T> bool DoubleLinkedList<T>::empty() {
  return head.first == nullptr and head.last == nullptr;
}

template <typename T> void DoubleLinkedList<T>::delete_node(Node<T> *node) {
  if (node == nullptr)
    return;

  if (node->prev != nullptr) {
    node->prev->next = node->next;
  } else if (head.first == node) {
    head.first = node->next;
  }

  if (node->next != nullptr) {
    node->next->prev = node->prev;
  } else if (head.last == node) {
    head.last = node->prev;
  }

  delete node;
}

#endif
