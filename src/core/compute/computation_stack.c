#include "computation_stack.h"
#include <stdlib.h>
#include <string.h>

void comp_stack_init(ComputationStack *stack) { stack->top = NULL; }

void comp_stack_add(ComputationStack *s, ComputationLayer *l) {
    if (!s->top || l->priority > s->top->priority) {
        l->next = s->top;
        s->top = l;
        return;
    }
    ComputationLayer *it = s->top;
    while (it->next && it->next->priority >= l->priority)
        it = it->next;
    l->next = it->next;
    it->next = l;
}

void comp_stack_remove(ComputationStack *stack, const char *name) {
    ComputationLayer *cur = stack->top, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev)
                prev->next = cur->next;
            else
                stack->top = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void comp_stack_execute(const ComputationStack *stack, GameHandle *gh) {
    if (!stack)
        return;
    for (ComputationLayer *it = stack->top; it; it = it->next)
        it->fn(gh);
}

void comp_stack_destroy(ComputationStack *stack) {
    ComputationLayer *cur = stack->top;
    while (cur) {
        ComputationLayer *next = cur->next;
        free(cur->name);
        free(cur);
        cur = next;
    }
    stack->top = NULL;
}

/* helper to allocate + push */
void push_layer(GameHandle *gh, const char *name, ComputationFn fn, int prio) {
    ComputationLayer *cl = calloc(1, sizeof *cl);
    cl->name = strdup(name);
    cl->priority = prio;
    cl->fn = fn;
    comp_stack_add(gh->stack, cl);
}