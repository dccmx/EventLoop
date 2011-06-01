#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

template<typename T, typename Compare>
RBTree<T, Compare>::RBTree() {
  root_ = NULL;
  count_ = 0;
  generation_ = 0;
}

template<typename T, typename Compare>
RBTree<T, Compare>::~RBTree() {
  Node *p, *q;

  for (p = root_; p != NULL; p = q)
    if (p->link[0] == NULL) {
      q = p->link[1];
      // if (destroy != NULL && p->data != NULL) destroy (p->data, rb_param);
      delete p;
    } else {
      q = p->link[0];
      p->link[0] = q->link[1];
      q->link[1] = p;
    }
}

template<typename T, typename Compare>
T RBTree<T, Compare>::Insert(T item) {
  T *p = Probe(item);
  return p == NULL || *p == item ? NULL : *p;
}

template<typename T, typename Compare>
T *RBTree<T, Compare>::Probe(T item) {
  Node *pa[RB_MAX_HEIGHT]; /* Nodes on stack. */
  unsigned char da[RB_MAX_HEIGHT];   /* Directions moved from stack nodes. */
  int k;                             /* Stack height. */

  Node *p; /* Traverses tree looking for insertion point. */
  Node *n; /* Newly inserted node. */

  assert (item != NULL);

  pa[0] = (Node *) &root_;
  da[0] = 0;
  k = 1;
  for (p = root_; p != NULL; p = p->link[da[k - 1]]) {
    int cmp = Compare(item, p->data);
    if (cmp == 0) return &p->data;
    pa[k] = p;
    da[k++] = cmp > 0;
  }

  n = pa[k - 1]->link[da[k - 1]] = new Node**;
  if (n == NULL)
    return NULL;

  n->data = item;
  n->link[0] = n->link[1] = NULL;
  n->color = RED;
  count_++;
  generation_++;

  while (k >= 3 && pa[k - 1]->color == RED) {
    if (da[k - 2] == 0) {
      Node *y = pa[k - 2]->link[1];
      if (y != NULL && y->color == RED) {
        pa[k - 1]->color = y->color = BLACK;
        pa[k - 2]->color = RED;
        k -= 2;
      } else {
        Node *x;

        if (da[k - 1] == 0) {
          y = pa[k - 1];
        } else {
          x = pa[k - 1];
          y = x->link[1];
          x->link[1] = y->link[0];
          y->link[0] = x;
          pa[k - 2]->link[0] = y;
        }

        x = pa[k - 2];
        x->color = RED;
        y->color = BLACK;

        x->link[0] = y->link[1];
        y->link[1] = x;
        pa[k - 3]->link[da[k - 3]] = y;
        break;
      }
    } else {
      Node *y = pa[k - 2]->link[0];
      if (y != NULL && y->color == RED) {
        pa[k - 1]->color = y->color = BLACK;
        pa[k - 2]->color = RED;
        k -= 2;
      } else {
        Node *x;

        if (da[k - 1] == 1) {
          y = pa[k - 1];
        } else {
          x = pa[k - 1];
          y = x->link[0];
          x->link[0] = y->link[1];
          y->link[1] = x;
          pa[k - 2]->link[1] = y;
        }

        x = pa[k - 2];
        x->color = RED;
        y->color = BLACK;

        x->link[1] = y->link[0];
        y->link[0] = x;
        pa[k - 3]->link[da[k - 3]] = y;
        break;
      }
    }
  }

  root_->color = BLACK;

  return &n->data;
}

template<typename T, typename Compare>
T RBTree<T, Compare>::Delete(T item) {
  Node *pa[RB_MAX_HEIGHT]; /* Nodes on stack. */
  unsigned char da[RB_MAX_HEIGHT];   /* Directions moved from stack nodes. */
  int k;                             /* Stack height. */
  Node *p;    /* The node to delete, or a node part way to it. */
  int cmp;              /* Result of comparison between |item| and |p|. */

  assert(item != NULL);

  k = 0;
  p = (Node *) &root_;
  for (cmp = -1; cmp != 0; cmp = Compare(item, p->data)) {
    int dir = cmp > 0;

    pa[k] = p;
    da[k++] = dir;

    p = p->link[dir];
    if (p == NULL) return NULL;
  }

  item = p->data;

  if (p->link[1] == NULL) {
    pa[k - 1]->link[da[k - 1]] = p->link[0];
  } else {
    Color t;
    Node *r = p->link[1];

    if (r->link[0] == NULL) {
      r->link[0] = p->link[0];
      t = r->color;
      r->color = p->color;
      p->color = t;
      pa[k - 1]->link[da[k - 1]] = r;
      da[k] = 1;
      pa[k++] = r;
    } else {
      Node *s;
      int j = k++;

      while (true) {
        da[k] = 0;
        pa[k++] = r;
        s = r->link[0];
        if (s->link[0] == NULL) break;
        r = s;
      }

      da[j] = 1;
      pa[j] = s;
      pa[j - 1]->link[da[j - 1]] = s;

      s->link[0] = p->link[0];
      r->link[0] = s->link[1];
      s->link[1] = p->link[1];

      t = s->color;
      s->color = p->color;
      p->color = t;
    }
  }

  if (p->color == BLACK) {
    while (true) {
      Node *x = pa[k - 1]->link[da[k - 1]];
      if (x != NULL && x->color == RED) {
        x->color = BLACK;
        break;
      }

      if (k < 2) break;

      if (da[k - 1] == 0) {
        Node *w = pa[k - 1]->link[1];

        if (w->color == RED) {
          w->color = BLACK;
          pa[k - 1]->color = RED;

          pa[k - 1]->link[1] = w->link[0];
          w->link[0] = pa[k - 1];
          pa[k - 2]->link[da[k - 2]] = w;

          pa[k] = pa[k - 1];
          da[k] = 0;
          pa[k - 1] = w;
          k++;

          w = pa[k - 1]->link[1];
        }

        if ((w->link[0] == NULL || w->link[0]->color == BLACK)
            && (w->link[1] == NULL || w->link[1]->color == BLACK)) {
          w->color = RED;
        } else {
          if (w->link[1] == NULL || w->link[1]->color == BLACK) {
            Node *y = w->link[0];
            y->color = BLACK;
            w->color = RED;
            w->link[0] = y->link[1];
            y->link[1] = w;
            w = pa[k - 1]->link[1] = y;
          }

          w->color = pa[k - 1]->color;
          pa[k - 1]->color = BLACK;
          w->link[1]->color = BLACK;

          pa[k - 1]->link[1] = w->link[0];
          w->link[0] = pa[k - 1];
          pa[k - 2]->link[da[k - 2]] = w;
          break;
        }
      } else {
        Node *w = pa[k - 1]->link[0];

        if (w->color == RED) {
          w->color = BLACK;
          pa[k - 1]->color = RED;

          pa[k - 1]->link[0] = w->link[1];
          w->link[1] = pa[k - 1];
          pa[k - 2]->link[da[k - 2]] = w;

          pa[k] = pa[k - 1];
          da[k] = 1;
          pa[k - 1] = w;
          k++;

          w = pa[k - 1]->link[0];
        }

        if ((w->link[0] == NULL || w->link[0]->color == BLACK)
            && (w->link[1] == NULL || w->link[1]->color == BLACK)) {
          w->color = RED;
        } else {
          if (w->link[0] == NULL || w->link[0]->color == BLACK) {
            Node *y = w->link[1];
            y->color = BLACK;
            w->color = RED;
            w->link[1] = y->link[0];
            y->link[0] = w;
            w = pa[k - 1]->link[0] = y;
          }

          w->color = pa[k - 1]->color;
          pa[k - 1]->color = BLACK;
          w->link[0]->color = BLACK;

          pa[k - 1]->link[0] = w->link[1];
          w->link[1] = pa[k - 1];
          pa[k - 2]->link[da[k - 2]] = w;
          break;
        }
      }

      k--;
    }

  }

  delete p;
  count_--;
  generation_++;
  return item;
}

