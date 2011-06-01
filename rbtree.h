#ifndef RB_TREE_H_
#define RB_TREE_H_
#include <stdint.h>

template<typename T, typename Compare>
class RBTree {
 public:
  enum Color {
    BLACK,
    RED
  };
  enum { RB_MAX_HEIGHT = 48 };

  struct Node {
    Node *link[2];
    T data;
    unsigned char color;
  };

  struct Traverser {
    RBTree *tree;        /* Tree being traversed. */
    Node *node;          /* Current node in tree. */
    Node *stack[RB_MAX_HEIGHT];
    uint32_t height;                  /* Number of nodes in |rb_parent|. */
    unsigned long generation;       /* Generation number. */
  };

 public:
  RBTree();
  ~RBTree();

 public:
  T Insert(T item);
  T Delete(const T item);

 private:
  T *Probe(T item);

 private:
  Node *root_;
  uint32_t count_;
  unsigned long generation_;
};

#endif // RB_TREE_H_
