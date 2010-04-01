#include "intstack.h"

#include <stdlib.h>

struct IntStackNode {
  int i;
  IntStackNode *next;
};

void intstack_init (IntStack *stack)
{
  stack->top = 0;
}

IntStackNode*
intstack_node_new (int i, IntStackNode *next)
{
  IntStackNode *node = malloc (sizeof (*node));
  node->i = i;
  node->next = next;
  return node;
}

void intstack_push (IntStack *stack, int i)
{
  stack->top = intstack_node_new (i, stack->top);
}

int intstack_peek (IntStack *stack)
{
  return stack->top->i;
}

int intstack_pop (IntStack *stack)
{
  int i = stack->top->i;
  IntStackNode *oldtop = stack->top;
  stack->top = oldtop->next;
  free (oldtop);
  return i;
}
