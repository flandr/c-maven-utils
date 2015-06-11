/*
 * Copyright (©) 2015 Nate Rosenblum
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "comparable-version.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

static int list_empty(struct list_head *head) {
    return head->next == head;
}

static void list_init(struct list_head *head) {
    head->next = head->prev = head;
}

static void list_del(struct list_head *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->next = entry->prev = entry;
}

static void list_insert_front(struct list_head *head, struct list_head *entry) {
    assert(list_empty(entry));

    entry->next = head->next;
    entry->next->prev = entry;
    head->next = entry;
    entry->prev = head;
}

static void list_insert_back(struct list_head *head, struct list_head *entry) {
    assert(list_empty(entry));

    entry->prev = head->prev;
    entry->prev->next = entry;
    head->prev = entry;
    entry->next = head;
}

struct stack_elem {
    struct list_head head;
    void *value;
};

struct stack {
    struct list_head items;
};

static void stack_init(struct stack *s) {
    list_init(&s->items);
}

static void stack_push(struct stack *s, void *value) {
    struct stack_elem *elem = (struct stack_elem*) malloc(sizeof(*elem));
    list_init(&elem->head);
    elem->value = value;
    list_insert_front(&s->items, &elem->head);
}

static int stack_empty(struct stack *s) {
    return list_empty(&s->items);
}

static void* stack_pop(struct stack *s) {
    assert(!stack_empty(s));

    struct stack_elem *front = (struct stack_elem*) s->items.next;
    list_del(&front->head);
    void *ret = front->value;
    free(front);
    return ret;
}

enum item_type {
    INTEGER_ITEM,
    STRING_ITEM,
    LIST_ITEM,
};

struct item {
    struct list_head head;
    enum item_type type;
};

struct item_list {
    struct item common;
    struct list_head children;
};

struct item_integer {
    struct item common;
    int value;
};

struct item_string {
    struct item common;
    char *value;
};

struct comparable_version {
    struct item_list *items;
};

static void free_item(struct item *item);

static void free_item_list(struct item_list *list) {
    while (!list_empty(&list->children)) {
        struct list_head *child = list->children.next;
        list_del(child);
        free_item((struct item*) child);
    }
    free(list);
}

static void free_item(struct item *item) {
    assert(list_empty(&item->head));

    switch (item->type) {
    case INTEGER_ITEM:
        free((struct item_integer*) item);
        break;
    case STRING_ITEM:
        free(((struct item_string*) item)->value);
        free((struct item_string*) item);
        break;
    case LIST_ITEM:
        /* I feel pretty okay about this recursion. */
        free_item_list((struct item_list*) item);
        break;
    }
}

static int is_null(struct item *item) {
    switch (item->type) {
    case INTEGER_ITEM:
        return ((struct item_integer*) item)->value == 0;
    case STRING_ITEM:
        /* TODO: normalization and order of qualifiers. */
        return 0;
    case LIST_ITEM:
        return list_empty(&((struct item_list*) item)->children);
    }
}

static void normalize_list_item(struct item_list *list) {
    while (!list_empty(&list->children)) {
        struct item *item = (struct item*) list->children.prev;
        if (!is_null(item)) {
            break;
        }
        list_del(&item->head);
        free_item(item);
    }
}

static void init_item(struct item *item, enum item_type type) {
    list_init(&item->head);
    item->type = type;
}

static void item_list_add(struct item_list *list, struct item *item) {
    list_insert_back(&list->children, &item->head);
}

static struct item_list* mk_item_list() {
    struct item_list *ret = (struct item_list*) malloc(sizeof(*ret));
    init_item(&ret->common, LIST_ITEM);
    list_init(&ret->children);
    return ret;
}

static struct item_integer* mk_item_integer(int value) {
    struct item_integer *ret = (struct item_integer*) malloc(sizeof(*ret));
    init_item(&ret->common, INTEGER_ITEM);
    ret->value = value;
    return ret;
}

static struct item_string* mk_item_string(const char *str, size_t size) {
    /* TODO: canonicalization of alpha/beta/milestone and other aliases */
    struct item_string *ret = (struct item_string*) malloc(sizeof(*ret));
    init_item(&ret->common, STRING_ITEM);
    ret->value = strndup(str, size);
    return ret;
}

static int is_digit_p(char c) {
    return c >= '0' && c <= '9';
}

static struct item* parse_item(int is_digit, const char *buf, size_t size) {
    if (is_digit) {
        return (struct item*) mk_item_integer(strtol(buf, NULL, 10));
    }
    /* TODO: should not apply following-digit normalization */
    return (struct item*) mk_item_string(buf, size);
}

static int compare_int(int a, int b) {
    if (a == b) {
        return 0;
    } else if (a < b) {
        return -1;
    }
    return 1;
}

static int compare_item_integer(struct item_integer *a, struct item *b) {
    if (!b) {
        return a->value == 0 ? 0 : 1;
    }

    switch (b->type) {
    case INTEGER_ITEM:
        return compare_int(a->value, ((struct item_integer*) b)->value);
    case STRING_ITEM:
        return 1; /* Numeric components are always newer than qualifiers */
    case LIST_ITEM:
        return 1; /* Numeric components are always newer than sublists. */
    }
}

static int compare_item_string(struct item_string *a, struct item *b) {
    if (!b) {
        /* TODO: compare self to release version :P */
        return 0;
    }

    switch (b->type) {
    case INTEGER_ITEM:
        return -1;
    case STRING_ITEM:
        /* TODO: qualifier comparison :( */
        return strcmp(a->value, ((struct item_string*) b)->value);
    case LIST_ITEM:
        return -1;
    }
}

static int compare_item(struct item*, struct item*);

static struct item* item_list_next(struct item_list *list, struct item *cur) {
    if (list_empty(&list->children)) {
        return NULL;
    }

    if (!cur) {
        return (struct item*) list->children.next;
    }

    if (cur->head.next == &list->children) {
        return NULL;
    }

    return (struct item*) cur->head.next;
}

static int compare_item_list(struct item_list *a, struct item *b) {
    if (!b) {
        if (list_empty(&a->children)) {
            return 0;
        }
        return compare_item((struct item*)a->children.next, NULL);
    }

    switch (b->type) {
    case INTEGER_ITEM:
        return -1;
    case STRING_ITEM:
        return 1;
    case LIST_ITEM:
        break;
    }

    /* compare items in lock step */
    struct item_list *bl = (struct item_list*) b;
    struct item *left = NULL;
    struct item *right = NULL;

    while (item_list_next(a, left) || item_list_next(bl, right)) {
        left = item_list_next(a, left);
        right = item_list_next(bl, right);

        int result;
        if (!left && !right) {
            result = 0;
        } else if (!left) {
            result = -1 * compare_item(right, NULL);
        } else {
            result = compare_item(left, right);
        }

        if (result != 0) {
            return result;
        }
    }

    return 0;
}

static int compare_item(struct item *a, struct item *b) {
    switch (a->type) {
    case INTEGER_ITEM:
        return compare_item_integer((struct item_integer*) a, b);
    case STRING_ITEM:
        return compare_item_string((struct item_string*) a, b);
    case LIST_ITEM:
        return compare_item_list((struct item_list*) a, b);
    }
}

struct comparable_version* mv_internal_parse_comparable(const char *version) {
    struct stack lists; /* For post-parse normalization */
    stack_init(&lists);

    struct comparable_version *comparable =
        (struct comparable_version*) malloc(sizeof(*comparable));

    comparable->items = mk_item_list();
    struct item_list *list = comparable->items;
    stack_push(&lists, list);

    int is_digit = 0;
    int start_index = 0;
    const char *cur = version;
    int i;
    for (i = 0; *cur != '\0'; ++i, ++cur) {
        if (*cur == '.') {
            if (i == start_index) {
                item_list_add(list, (struct item*) mk_item_integer(0));
            } else {
                item_list_add(list, parse_item(is_digit, version + start_index,
                    i - start_index));
            }
            start_index = i + 1;
        } else if (*cur == '-') {
            if (i == start_index) {
                item_list_add(list, (struct item*) mk_item_integer(0));
            } else {
                item_list_add(list, parse_item(is_digit, version + start_index,
                    i - start_index));
            }
            start_index = i + 1;

            struct item_list *newlist = mk_item_list();
            item_list_add(list, (struct item*) newlist);
            list = newlist;
            stack_push(&lists, list);
        } else if (is_digit_p(*cur)) {
            if (!is_digit && i > start_index) {
                item_list_add(list, (struct item*) mk_item_string(
                    version + start_index, i - start_index));
                start_index = i;

                struct item_list *newlist = mk_item_list();
                item_list_add(list, (struct item*) newlist);
                list = newlist;
                stack_push(&lists, list);
            }
            is_digit = 1;
        } else {
            if (is_digit && i > start_index) {
                item_list_add(list, parse_item(/*is_digit=*/ 1,
                    version + start_index, i - start_index));
                start_index = i;

                struct item_list *newlist = mk_item_list();
                item_list_add(list, (struct item*) newlist);
                list = newlist;
                stack_push(&lists, list);
            }
            is_digit = 0;
        }
    }

    if (start_index < strlen(version)) {
        item_list_add(list, parse_item(is_digit, version + start_index,
            strlen(version) - start_index));
    }

    while (!stack_empty(&lists)) {
        list = (struct item_list*) stack_pop(&lists);
        normalize_list_item(list);
    }

    return comparable;
}

void mv_internal_free_comparable(struct comparable_version* comparable) {
    free_item_list(comparable->items);
    free(comparable);
}

int mv_internal_compare(struct comparable_version *a,
        struct comparable_version *b) {
    return compare_item_list(a->items, (struct item*) b->items);
}
