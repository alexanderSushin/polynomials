#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <cmath>

const int Alp = 26; // число переменных

struct Polynom { // инвариант: многочлены упорядочены лексиграфически по степеням
    float cf; // коэффициент
    std::vector<int> degs; // степени
    Polynom *child, *parent;
    Polynom(std::vector<int> _degs, float _cf) {
        degs = _degs, cf = _cf;
        parent = child = nullptr;
    }
};

Polynom *toBegin(Polynom *to) { // перевод многочлена в начало
    if (to == nullptr) return nullptr;
    while (to->parent != nullptr) {
        to = to->parent;
    }
    return to;
}

Polynom *toEnd(Polynom *to) {
    if (to == nullptr) return nullptr;
    while (to->child != nullptr) {
        to = to->child;
    }
    return to;
}

void set(Polynom *&child, Polynom *&parent) { // перевод указателей
    child->parent = parent;
    parent->child = child;
    return;
}

void clear(Polynom *&x) {
    if (x == nullptr) return;
    clear(x->child);
    x->parent = x->child = nullptr;
    delete x;
    x = nullptr;
    return;
}

Polynom *add(Polynom *left, Polynom *right) { // сумма
    std::map<std::vector<int>, float> keys;
    while (left != nullptr) {
        keys[left->degs] += left->cf;
        left = left->child;
    }
    while (right != nullptr) {
        keys[right->degs] += right->cf;
        right = right->child;
    }
    Polynom *res = nullptr;
    for (auto [_degs, _cf] : keys) { // поддержание инварианта вследствие свойств map
        if (_cf == 0) continue;
        Polynom *to = new Polynom(_degs, _cf);
        if (res != nullptr) {
            set(to, res);
        }
        res = to;
    }
    return toBegin(res);
}

Polynom *diff(Polynom *left, Polynom *right) { // вычитание
    if (right == nullptr) return left;
    while (true) {
        right->cf *= -1;
        if (right->child == nullptr) {
            break;
        }
        right = right->child;
    }
    right = toBegin(right);
    return add(left, right);
}

Polynom *mul(Polynom *left, Polynom *right) { // умножение
    std::map<std::vector<int>, float> keys;
    while (left) {
        Polynom *to = right;
        while (to) {
            std::vector<int> sum(Alp, 0);
            for (int i = 0; i < Alp; ++i) {
                sum[i] = left->degs[i] + to->degs[i];
            }
            keys[sum] += left->cf * to->cf;
            to = to->child;
        }
        left = left->child;
    }
    Polynom *res = nullptr;
    for (auto [_degs, _cf] : keys) { // поддержание инварианта вследствие свойст map
        if (_cf == 0) continue;
        Polynom *to = new Polynom(_degs, _cf);
        if (res != nullptr) {
            set(to, res);
        }
        res = to;
    }
    return toBegin(res);
}

std::pair<std::string, std::pair<Polynom *, Polynom *>> div(Polynom *left, Polynom *right) { // деление
    std::string verdict = "success";
    if (right == nullptr) {
        return {"undefined", {nullptr, nullptr}};
    } // не определено деление
    if (left == nullptr) return {"success", {nullptr, nullptr}};
    std::set<int> pos1, pos2;
    while (true) {
        for (int i = 0; i < Alp; ++i) {
            if (left->degs[i] != 0) pos1.insert(i);
        }
        if (left->child == nullptr) break;
        left = left->child;
    }
    while (true) {
        for (int i = 0; i < Alp; ++i) {
            if (right->degs[i] != 0) pos2.insert(i);
        }
        if (right->child == nullptr) break;
        right = right->child;
    }
    try {
        if (pos1.size() > 1 || pos2.size() > 1) {
            throw 1;
        }
        int it;
        if (pos1.size() && pos2.size()) {
            int it1 = *pos1.begin();
            int it2 = *pos2.begin();
            if (it1 != it2) {
                throw 2;
            }
            it = it1;
        } else if (pos1.size()) {
            it = *pos1.begin();
        } else if (pos2.size()) {
            it = *pos2.begin();
        } else {
            it = 0;
        }
        Polynom *res = nullptr;
        while (left != nullptr && left->degs[it] >= right->degs[it]) {
            double ncf = left->cf / right->cf;
            int df = left->degs[it] - right->degs[it];
            std::vector<int> dg(Alp, 0);
            dg[it] = df;
            Polynom *to = new Polynom(dg, ncf);
            Polynom *cur = mul(toBegin(right), to);
            if (res != nullptr) {
                set(res, to);
            }
            res = to;
            left = toEnd(diff(toBegin(left), cur));
            clear(cur);
        }
        return {verdict, {toBegin(res), toBegin(left)}};
    }
    catch (int e) {
        if (e == 1) {
            verdict = "too many arguments";
            return {verdict, {nullptr, nullptr}};
        }
        if (e == 2) {
            verdict = "different arguments";
            return {verdict, {nullptr, nullptr}};
        }
    }
}

float get(Polynom *p, std::vector<float> &pp) { // получение значения в точке
    double val = 0;
    while (p) {
        int key = p->cf;
        for (int i = 0; i < Alp; ++i) {
            key *= pow(pp[i], p->degs[i]);
        }
        val += key;
        p = p->child;
    }
    return val;
}

Polynom *derivative(Polynom *p, int pos) { // получение частной производной
    Polynom *res = nullptr;
    while (p) {
        std::vector<int> keys = p->degs;
        if (keys[pos] != 0) {
            double ncf = p->cf * keys[pos];
            --keys[pos];
            Polynom *dp = new Polynom(keys, ncf);
            if (res) {
                set(dp, res);
            }
            res = dp;
        }
        p = p->child;
    }
    return toBegin(res);
}
std::pair<std::string, std::set<int>> getRoots(Polynom *p) { // получение целых корней
    std::string verdict = "success";
    std::set<int> s;
    std::set<int> cfs;
    Polynom *tp = p;
    bool check = 1;
    while (tp) {
        for (int i = 0; i < Alp; ++i) {
            if (tp->degs[i] != 0) s.insert(i);
        }
        check &= (p->cf == (int)p->cf);
        if (!check) break;
        cfs.insert(abs(p->cf));
        tp = tp->child;
    }
    try {
        if (check == 0) {
            throw 1;
        }
        if (s.size() == 0) {
            throw 2;
        }
        if (s.size() > 2) {
            throw 3;
        } else {
            int pos = *s.begin();
            std::set<int> ans;
            std::vector<float> num(Alp);
            if (get(p, num) == 0) ans.insert(0);
            for (int x : cfs) {
                for (int j = 1; j * j <= x; ++j) {
                    if (x % j) continue;
                    std::vector<int> keys = {j, -j, x / j, -x / j};
                    for (int r : keys) {
                        num[pos] = r;
                        if (get(p, num) == 0) {
                            ans.insert(r);
                        }
                    }
                }
            }
            return {verdict, ans};
        }
    }
    catch(int e) {
        if (e == 1) {
            verdict = "float coefficient";
            return {verdict, {}};
        }
        if (e == 2) {
            verdict = "no arguments";
            return {verdict, {}};
        }
        if (e == 3) {
            verdict = "too many arguments";
            return {verdict, {}};
        }
    }
}

bool is_letter(char ch) {
    return ch >= 'a' && ch <= 'z';
}

bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

std::pair<Polynom *, std::string> read(std::string s) {
    bool numandspace = 0;
    for (int i = 0; i < s.size();) {
        if (is_digit(s[i]) && i + 1 < s.size()) {
            ++i;
            bool bad = s[i] == ' ';
            if (bad) {
                while (i < s.size() && s[i] == ' ') {
                    ++i;
                }
                if (i < s.size() && is_digit(s[i])) {
                    return {nullptr, "space between numbers"};
                }
            }
        } else {
            ++i;
        }
    }
    std::string cur = "";
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == ' ') continue;
        cur += s[i];
    }
    s = cur;
    std::string verdict = "success";
    int was = -1;
    int sign = 1; // по дефолту знак +
    int n = s.size();
    std::map<std::vector<int>, double> keys; // поддeржка инварианта
    for (int i = 0; i < n;) {
        if (is_digit(s[i]) || is_letter(s[i])) {
            try {
                if (was == 1) throw 1; // наличие существенных пробелов
            }
            catch (int e) {
                if (e == 1) {
                    verdict = "no sign after a term";
                    return {nullptr, verdict};
                }
            }
            was = 1;
            double cf;
            if (s[i] >= '0' && s[i] <= '9') {
                cf = 0;
                while (i < n && is_digit(s[i])) {
                    cf *= 10;
                    cf += s[i] - '0';
                    ++i;
                }
            } else {
                cf = 1;
            }
            cf *= sign;
            std::vector<int> degs(Alp, 0);
            while (i < n && is_letter(s[i])) {
                int pos = s[i] - 'a';
                ++i;
                int deg;
                if (i < n && s[i] == '^') {
                    bool is = 0;
                    deg = 0;
                    ++i;
                    while (i < n && is_digit(s[i])) {
                        is = 1;
                        deg *= 10;
                        deg += s[i] - '0';
                        ++i;
                    }
                    try {
                        if (!is) {
                            throw 1;
                        }
                    }
                    catch (int e) {
                        verdict = "no pow after ^";
                        return {nullptr, verdict};
                    }
                } else {
                    deg = 1;
                }
                degs[pos] += deg;
            }
            keys[degs] += cf;
            continue;
        }
        if (s[i] == '+' || s[i] == '-') {
            try {
                if (was == 0) {
                    throw 1;
                }
            }
            catch (int e) {
                if (e == 1) {
                    verdict = "too many signs";
                    return {nullptr, verdict};
                }
            }
            sign = (s[i] == '+' ? 1 : -1);
            was = 0;
            ++i;
            continue;
        }
        verdict = "unexpected character";
        return {nullptr, verdict};
    }
    try {
        if (was == 0) {
            throw 1;
        }
    }
    catch(int e) {
        if (e == 1) {
            verdict = "no terms after a sign";
            return {nullptr, verdict};
        }
    }
    Polynom *res = nullptr;
    for (auto [_degs, _cf] : keys) { // поддержание инварианта вследствие свойст map
        if (_cf == 0) continue;
        Polynom *to = new Polynom(_degs, _cf);
        if (res != nullptr) {
            set(to, res);
        }
        res = to;
    }
    return {toBegin(res), verdict};
}

std::string print(Polynom *s, float cf = 1) {
    if (s == nullptr) return "";
    std::string cur = "";
    int val = cf * s->cf;
    if (val == cf * s->cf) {
        cur += std::to_string(val);
    } else {
        cur += std::to_string(cf * s->cf);
    }
    for (int i = 0; i < Alp; ++i) {
        if (s->degs[i] != 0) {
            cur += char('a' + i);
            cur += '^';
            cur += std::to_string(s->degs[i]);
        }
    }
    if (s->child) {
        if (s->child->cf > 0) {
            cur += " + ";
            cur += print(s->child, 1);
        } else {
            cur +=  " - ";
            cur += print(s->child, -1);
        }
    }
    return cur;
}
