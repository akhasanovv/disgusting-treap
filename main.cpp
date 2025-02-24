#pragma GCC optimize("O3")
#include <bits/stdc++.h>

std::mt19937 rnd(std::chrono::high_resolution_clock::now().time_since_epoch().count());

enum class PushType {
    Add, Set, Nothing
};

struct Node {
    Node *l;
    Node *r;
    int64_t y;
    int64_t sz;
    bool rev;
    int64_t value;
    int64_t sum;
    int64_t lmost, rmost;
    int64_t inc, dec;
    int64_t push_value;
    PushType push_type;
    Node(int64_t _value) {
        y = rnd();
        sz = 1;
        l = r = nullptr;
        rev = 0;
        value = sum = _value;
        lmost = rmost = value;
        push_value = 0;
        push_type = PushType::Nothing;
        inc = dec = 0;
    }
};

int get_size(Node *v) {
    if (!v) {
        return 0;
    }
    return v->sz;
}

void apply_rev(Node *v) {
    if (!v) {
        return ;
    }
    std::swap(v->l, v->r);
    std::swap(v->lmost, v->rmost);
    std::swap(v->inc, v->dec);
    v->rev ^= 1;
}

void propagate_push(Node* v) {
    if (!v) {
        return ;
    }

    if (v->rev) {
        apply_rev(v->l);
        apply_rev(v->r);
        v->rev ^= 1;
    }

    if (v->push_type == PushType::Add) {
        if (v->l) {
            if (v->l->push_type == PushType::Nothing) {
                v->l->push_type = v->push_type;
                v->l->push_value = v->push_value;
            } else {
                v->l->push_value += v->push_value;
            }
            v->l->sum += v->l->sz * v->push_value;
            v->l->value += v->push_value;
            v->l->lmost += v->push_value;
            v->l->rmost += v->push_value;
        }

        if (v->r) {
            if (v->r->push_type == PushType::Nothing) {
                v->r->push_type = v->push_type;
                v->r->push_value = v->push_value;
            } else {
                v->r->push_value += v->push_value;
            }
            v->r->sum += v->r->sz * v->push_value;
            v->r->value += v->push_value;
            v->r->lmost += v->push_value;
            v->r->rmost += v->push_value;
        }
    }

    if (v->push_type == PushType::Set) {
        if (v->l) {
            v->l->push_type = v->push_type;
            v->l->push_value = v->push_value;
            
            v->l->sum = v->l->sz * v->push_value;
            v->l->value = v->push_value;
            v->l->lmost = v->push_value;
            v->l->rmost = v->push_value;

            v->l->inc = 0;
            v->l->dec = 0;
        }

        if (v->r) {
            v->r->push_type = v->push_type;
            v->r->push_value = v->push_value;
            
            v->r->sum = v->r->sz * v->push_value;
            v->r->value = v->push_value;
            v->r->lmost = v->push_value;
            v->r->rmost = v->push_value;

            v->r->inc = 0;
            v->r->dec = 0;
        }
    }

    v->push_type = PushType::Nothing;
    v->push_value = 0;
}

void update_node(Node *v) {
    if (!v) {
        return ;
    }
    v->sz = get_size(v->l) + 1 + get_size(v->r);

    v->sum = v->value;
    if (v->l) {
        v->sum += v->l->sum;
    }
    if (v->r) {
        v->sum += v->r->sum;
    }

    v->lmost = v->value;
    v->rmost = v->value;
    if (v->l) {
        v->lmost = v->l->lmost;
    }
    if (v->r) {
        v->rmost = v->r->rmost;
    }

    v->inc = 0;
    v->dec = 0;
    if (v->l) {
        v->inc += v->l->inc;
        v->dec += v->l->dec;
        v->inc += (v->l->rmost < v->value);
        v->dec += (v->l->rmost > v->value);
    }
    if (v->r) {
        v->inc += v->r->inc;
        v->dec += v->r->dec;
        v->inc += (v->value < v->r->lmost);
        v->dec += (v->value > v->r->lmost);
    }
}

void print(Node* v) {
    if (!v) {
        return ;
    }
    propagate_push(v);
    print(v->l);
    std::cout << v->value << ' ';
    print(v->r);
}

Node* merge(Node* a, Node* b) {
    if (!a) {
        return b;
    } else if (!b) {
        return a;
    }
    if (a->y > b->y) {
        propagate_push(a);
        a->r = merge(a->r, b);
        update_node(a);
        return a;
    }
    propagate_push(b);
    b->l = merge(a, b->l);
    update_node(b);
    return b;
}

std::pair<Node*, Node*> split(Node *a, int x) {
    if (!a) {
        return {nullptr, nullptr};
    }
    propagate_push(a);
    int s = get_size(a->l) + 1;
    if (x >= s) {
        auto [l, r] = split(a->r, x - s);
        a->r = l;
        update_node(a);
        return {a, r};
    }
    auto [l, r] = split(a->l, x);
    a->l = r;
    update_node(a);
    return {l, a};
}

Node* root = nullptr;

int64_t query_sum(int l, int r) {
    auto [L, R] = split(root, r);
    auto [L1, L2] = split(L, l - 1);
    int64_t res = L2->sum;
    root = merge(L1, merge(L2, R));
    return res;
}

void insert(int x, int pos) {
    auto [L, R] = split(root, pos - 1);
    Node* new_node = new Node(x);
    root = merge(L, merge(new_node, R));
}

void remove(int pos) {
    auto [L, R] = split(root, pos);
    auto [L1, L2] = split(L, pos - 1);
    root = merge(L1, R);
}

void add_segment(int l, int r, int64_t x) {
    auto [L, R] = split(root, r);
    auto [L1, L2] = split(L, l - 1);

    L2->sum += x * (r - l + 1);
    L2->push_value += x;
    if (L2->push_type == PushType::Nothing) {
        L2->push_type = PushType::Add;
    }
    L2->value += x;

    L2->lmost += x;
    L2->rmost += x;

    root = merge(L1, merge(L2, R));
}

void set_segment(int l, int r, int64_t x) {
    auto [L, R] = split(root, r);
    auto [L1, L2] = split(L, l - 1);

    L2->sum = x * (r - l + 1);
    L2->push_value = x;
    L2->push_type = PushType::Set;
    L2->value = x;

    L2->lmost = x;
    L2->rmost = x;

    L2->inc = 0;
    L2->dec = 0;

    root = merge(L1, merge(L2, R));
}

std::pair<Node*, Node*> value_split1(Node *a, int64_t x) {
    if (!a) {
        return {nullptr, nullptr};
    }
    propagate_push(a);
    if (a->value > x) {
        auto [l, r] = value_split1(a->r, x);
        a->r = l;
        update_node(a);
        return {a, r};
    }
    auto [l, r] = value_split1(a->l, x);
    a->l = r;
    update_node(a);
    return {l, a};
}

void next_permutation(int l, int r) {
    auto [_, __] = split(root, r);
    auto [___, v] = split(_, l - 1);

    if (!v->inc) {
        apply_rev(v);
        root = merge(___, merge(v, __));
        return ;
    }

    int lft = 0;
    Node* cur = v;
    while (cur) {
        propagate_push(cur);
        if (cur->r && cur->r->inc) {
            lft += 1 + get_size(cur->l);
            cur = cur->r;
            continue;
        }
        if (cur->r && cur->value < cur->r->lmost) {
            lft += get_size(cur->l);
            break;
        }
        if (cur->l && cur->l->rmost < cur->value) {
            lft += get_size(cur->l) - 1;
            break;
        }
        cur = cur->l;
    }

    auto [l1, r1] = split(v, ql);
    auto [swap1, other] = split(r1, 1);

    auto [l_other, r_other] = value_split1(other, swap1->value);
    auto [l_other1, swap2] = split(l_other, l_other->sz - 1);

    std::swap(swap1, swap2);
    l_other = merge(l_other1, swap2);
    other = merge(l_other, r_other);

    apply_rev(other);

    r1 = merge(swap1, other);
    v = merge(l1, r1);

    root = merge(___, merge(v, __));
}

std::pair<Node*, Node*> value_split2(Node *a, int64_t x) {
    if (!a) {
        return {nullptr, nullptr};
    }
    propagate_push(a);
    if (a->value < x) {
        auto [l, r] = value_split2(a->r, x);
        a->r = l;
        update_node(a);
        return {a, r};
    }
    auto [l, r] = value_split2(a->l, x);
    a->l = r;
    update_node(a);
    return {l, a};
}

void prev_permutation(int l, int r) {
    auto [_, __] = split(root, r);
    auto [___, v] = split(_, l - 1);

    if (!v->dec) {
        apply_rev(v);
        root = merge(___, merge(v, __));
        return ;
    }

    int lft = 0;
    Node* cur = v;
    while (cur) {
        propagate_push(cur);
        if (cur->r && cur->r->dec) {
            lft += 1 + get_size(cur->l);
            cur = cur->r;
            continue;
        }
        if (cur->r && cur->value > cur->r->lmost) {
            lft += get_size(cur->l);
            break;
        }
        if (cur->l && cur->l->rmost > cur->value) {
            lft += get_size(cur->l) - 1;
            break;
        }
        cur = cur->l;
    }

    auto [l1, r1] = split(v, lft);
    auto [swap1, other] = split(r1, 1);

    auto [l_other, r_other] = value_split2(other, swap1->value);
    auto [l_other1, swap2] = split(l_other, l_other->sz - 1);

    std::swap(swap1, swap2);
    l_other = merge(l_other1, swap2);
    other = merge(l_other, r_other);

    apply_rev(other);

    r1 = merge(swap1, other);
    v = merge(l1, r1);

    root = merge(___, merge(v, __));
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    for (int i = 1; i <= n; ++i) {
        int x;
        std::cin >> x;
        insert(x, i);
    }

    int q;
    std::cin >> q;

    while (q--) {
        int qt;
        std::cin >> qt;
        if (qt == 1) {
            int l, r;
            std::cin >> l >> r;
            ++l, ++r;
            std::cout << query_sum(l, r) << '\n';
        } else if (qt == 2) {
            int x, pos;
            std::cin >> x >> pos;
            ++pos;
            insert(x, pos);
        } else if (qt == 3) {
            int pos;
            std::cin >> pos;
            ++pos;
            remove(pos);
        } else if (qt == 4) {
            int x, l, r;
            std::cin >> x >> l >> r;
            ++l, ++r;
            set_segment(l, r, x);
        } else if (qt == 5) {
            int x, l, r;
            std::cin >> x >> l >> r;
            ++l, ++r;
            add_segment(l, r, x);
        } else if (qt == 6) {
            int l, r;
            std::cin >> l >> r;
            ++l, ++r;
            next_permutation(l, r);
        } else {
            int l, r;
            std::cin >> l >> r;
            ++l, ++r;
            prev_permutation(l, r);
        }
    }

    print(root);
    std::cout << '\n';
}
