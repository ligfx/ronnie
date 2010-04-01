#ifndef INTSTACK_H
#define INTSTACK_H

typedef struct IntStackNode IntStackNode;
typedef struct IntStack {
  IntStackNode *top;
} IntStack;


void intstack_init (IntStack *stack);
void intstack_destroy (IntStack *stack); // TODO
void intstack_push (IntStack *stack, int i);
int intstack_peek (IntStack *stack);
int intstack_pop (IntStack *stack);

#endif
