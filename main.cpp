#include "Polynom.h"
#include "Input.h"
#include <fstream>
#include <list>


struct List {
    Polynom* key;
    List *next, *parent;
    List() {
        parent = nullptr;
        next = nullptr;
        key = nullptr;
    }
    List(Polynom *_key) {
        key = _key;
        next = nullptr;
        parent = nullptr;
    }
};

List *to_end(List *x) {
    while (x->next != nullptr) {
        x = x->next;
    }
    return x;
}

List *to_begin(List *x) {
    while (x->parent != nullptr) {
        x = x->parent;
    }
    return x;
}

List *base;
int size = 0;

void push(Polynom *key) {
    size++;
    List *last = new List(key);
    if (base == nullptr) {
        base = last;
        return;
    }
    List *t_base = to_end(base);
    t_base->next = last;
    last->parent = t_base;
    base = to_begin(last);
}

void erase(int id) {
    size--;
    List *t_base = base;
    for (int i = 0; i < id; ++i) {
        t_base = t_base->next;
    }
    List *to_erase = t_base;
    if (t_base->parent == nullptr) {
        base = t_base->next;
        if (base != nullptr) {
            base->parent = nullptr;
        }
    } else {
        base = t_base->parent;
        base->next = t_base->next;
        if (base->next != nullptr) {
            (base->next)->parent = base;
        }
    }
    clear(to_erase->key);
    delete to_erase;
    base = to_begin(base);
}

Polynom* at(int id) {
    List *t_base = base;
    for (int i = 0; i < id; ++i) {
        t_base = t_base->next;
    }
    return t_base->key;
}

bool loaded;

void loadFromFile(std::string file) {
    //base.clear();
    std::ifstream in("/Users/alexsus/Desktop/polynomials/" + file + ".txt");
    if (!in) {
        loaded = 0;
        return;
    }
    loaded = 1;
    std::string parse;
    while (getline(in, parse)) {
        push(read(parse).first);
    }
}

void writeToFile(std::string file) {
    std::ofstream out("/Users/alexsus/Desktop/polynomials/" + file + ".txt");
    for (int i = 0; i < size; ++i) {
        out << print(at(i)) << '\n';
    }
    return;
}

RenderWindow window(VideoMode(2000, 1500), "polynom", Style::Close | Style::Titlebar);

int size_of = 15;
int id = 0;

void toSave(Polynom *ns, std::string vr = "save") {
    RenderWindow result(VideoMode(2750, 300), "result", Style::Close | Style::Titlebar);
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(30);
    verdict.setPosition({50, 100});
    Button save;
    save.setButtonColor(Color::White);
    save.setButtonSize({200, 100});
    save.setButtonPosition({2500, 170});
    save.setTitlePosition({2547, 170 + 16});
    save.setTitle(vr, 50);
    verdict.setFillColor(Color::Black);
    std::string cur = print(ns);
    while (result.isOpen()) {
        result.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(result);
        while (result.pollEvent(event)) {
            if (event.type == Event::Closed) {
                result.close();
            }
            if (save.pressed(mouse_position, event)) {
                push(ns);
                result.close();
            }
        }
        verdict.setString(cur);
        save.draw(result);
        result.draw(verdict);
        result.display();
    }
}

void renderPlus() {
    RenderWindow plus(VideoMode(1600, 300), "plus", Style::Close | Style::Titlebar);
    InputField left, right;
    left.setOnlyNumbers(1), right.setOnlyNumbers(1);
    left.setButtonPosition({100, 100});
    right.setButtonPosition({450, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    right.setButtonSettings({250, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    right.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    right.setTextPosition({450 + 3, 100});
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({800, 100});
    Text error;
    while (plus.isOpen()) {
        plus.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(plus);
        while (plus.pollEvent(event)) {
            if (event.type == Event::Closed) {
                plus.close();
            }
            left.change(event, mouse_position);
            right.change(event, mouse_position);
        }
        left.draw(plus);
        right.draw(plus);
        if (!left.chosen && !right.chosen) {
            if (left.value != "" && right.value != "") {
                int id1 = std::stoi(left.value), id2 = std::stoi(right.value);
                try {
                    if (id1 >= size || id2 >= size) throw 1;
                    plus.close();
                    toSave(add(at(id1), at(id2)));
                } catch(int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        plus.draw(verdict);
                    }
                }
            }
        }
        plus.display();
    }
}

void renderMult() {
    RenderWindow mult(VideoMode(1600, 300), "mult", Style::Close | Style::Titlebar);
    InputField left, right;
    left.setOnlyNumbers(1), right.setOnlyNumbers(1);
    left.setButtonPosition({100, 100});
    right.setButtonPosition({450, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    right.setButtonSettings({250, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    right.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    right.setTextPosition({450 + 3, 100});
    while (mult.isOpen()) {
        mult.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(mult);
        while (mult.pollEvent(event)) {
            if (event.type == Event::Closed) {
                mult.close();
            }
            left.change(event, mouse_position);
            right.change(event, mouse_position);
        }
        left.draw(mult);
        right.draw(mult);
        if (!left.chosen && !right.chosen) {
            if (left.value != "" && right.value != "") {
                int id1 = std::stoi(left.value), id2 = std::stoi(right.value);
                Text verdict;
                verdict.setFont(font);
                verdict.setCharacterSize(70);
                verdict.setPosition({800, 100});
                try {
                    if (id1 >= size || id2 >= size) throw 1;
                    mult.close();
                    toSave(mul(at(id1), at(id2)));
                } catch(int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        mult.draw(verdict);
                    }
                }
            }
        }
        mult.display();
    }
}

void renderDivision() {
    RenderWindow division(VideoMode(1600, 300), "div", Style::Close | Style::Titlebar);
    InputField left, right;
    left.setOnlyNumbers(1), right.setOnlyNumbers(1);
    left.setButtonPosition({100, 100});
    right.setButtonPosition({450, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    right.setButtonSettings({250, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    right.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    right.setTextPosition({450 + 3, 100});
    std::string error;
    while (division.isOpen()) {
        division.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(division);
        while (division.pollEvent(event)) {
            if (event.type == Event::Closed) {
                division.close();
            }
            left.change(event, mouse_position);
            right.change(event, mouse_position);
        }
        left.draw(division);
        right.draw(division);
        if (!left.chosen && !right.chosen) {
            if (left.value != "" && right.value != "") {
                int id1 = std::stoi(left.value), id2 = std::stoi(right.value);
                Text verdict;
                verdict.setFont(font);
                verdict.setCharacterSize(70);
                verdict.setPosition({800, 100});
                try {
                    if (id1 >= size || id2 >= size) throw 1;
                    std::pair<std::string, std::pair<Polynom*, Polynom*>> to = div(at(id1), at(id2));
                    if (to.first != "success") {
                        error = to.first;
                        throw 2;
                    }
                    division.close();
                    toSave((to.second).first);
                    toSave((to.second).second);
                } catch (int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        division.draw(verdict);
                    }
                    if (e == 2) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString(error);
                        division.draw(verdict);
                    }
                }
            }
        }
        division.display();
    }
}

void renderDerivative() {
    RenderWindow dx(VideoMode(1600, 300), "dx", Style::Close | Style::Titlebar);
    InputField left, right;
    left.setOnlyNumbers(1), right.setOnlySmall(1);
    left.setButtonPosition({100, 100});
    right.setButtonPosition({450, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    right.setButtonSettings({100, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    right.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    right.setTextPosition({450 + 30, 100});
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({800, 100});
    while (dx.isOpen()) {
        dx.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(dx);
        while (dx.pollEvent(event)) {
            if (event.type == Event::Closed) {
                dx.close();
            }
            left.change(event, mouse_position);
            right.change(event, mouse_position);
        }
        left.draw(dx);
        right.draw(dx);
        if (!left.chosen && !right.chosen) {
            if (left.value != "" && right.value != "") {
                int pos = std::stoi(left.value);
                try {
                    if (pos >= size) throw 1;
                    dx.close();
                    toSave(derivative(at(pos), right.value[0] - 'a'));
                } catch (int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        dx.draw(verdict);
                    }
                }
            }
        }
        dx.display();
    }
}

void renderErase() {
    RenderWindow ers(VideoMode(1600, 300), "erase", Style::Close | Style::Titlebar);
    InputField left;
    left.setOnlyNumbers(1);
    left.setButtonPosition({100, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({500, 100});
    while (ers.isOpen()) {
        ers.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(ers);
        while (ers.pollEvent(event)) {
            if (event.type == Event::Closed) {
                ers.close();
            }
            left.change(event, mouse_position);
        }
        left.draw(ers);
        if (!left.chosen) {
            if (left.value != "") {
                int pos = std::stoi(left.value);
                try {
                    if (pos >= size) throw 1;
                    ers.close();
                    erase(pos);
                } catch (int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        ers.draw(verdict);
                    }
                }
            }
        }
        ers.display();
    }
}

void renderAdd() {
    RenderWindow input(VideoMode(2500, 500), "add", Style::Close | Style::Titlebar);
    InputField left;
    left.setButtonPosition({100, 100});
    left.setButtonSettings({2300, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 15, 100});
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({940, 300});
    std::string error;
    while (input.isOpen()) {
        input.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            left.change(event, mouse_position);
        }
        left.draw(input);
        if (!left.chosen) {
            if (left.value != "") {
                try {
                    std::pair<Polynom*, std::string> to = read(left.value);
                    if (to.second != "success") {
                        error = to.second;
                        throw 1;
                    }
                    input.close();
                    toSave(to.first);
                } catch (int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString(error);
                        input.draw(verdict);
                    }
                }
            }
        }
        input.display();
    }
}

void renderRoots() {
    RenderWindow input(VideoMode(2500, 500), "roots", Style::Close | Style::Titlebar);
    InputField left;
    left.setOnlyNumbers(1);
    left.setButtonPosition({100, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({940, 300});
    std::string error;
    while (input.isOpen()) {
        input.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            left.change(event, mouse_position);
        }
        left.draw(input);
        if (!left.chosen) {
            if (left.value != "") {
                try {
                    int pos = std::stoi(left.value);
                    if (pos >= size) throw 1;
                    std::pair<std::string, std::set<int>> to = getRoots(at(pos));
                    if (to.first != "success") {
                        error = to.first;
                        throw 2;
                    }
                    std::string res = "";
                    for (int x : to.second) {
                        res += std::to_string(x);
                        res += ' ';
                    }
                    //std::cout << res << '\n';
                    verdict.setFillColor(Color::Black);
                    verdict.setString(res);
                    input.draw(verdict);
                } catch (int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        input.draw(verdict);
                    }
                    if (e == 2) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString(error);
                        input.draw(verdict);
                    }
                }
            }
        }
        input.display();
    }
}

void renderChoose(int pos) {
    RenderWindow input(VideoMode(1000, 1500), "choose", Style::Close | Style::Titlebar);
    std::vector<Text> arr(Alp);
    std::vector<InputField> nums(Alp);
    for (int i = 0; i < Alp; ++i) {
        std::string cur;
        cur += char('a' + i);
        cur += ':';
        arr[i].setString(cur);
        arr[i].setFillColor(Color::Black);
        arr[i].setFont(font);
        arr[i].setCharacterSize(45);
        arr[i].setPosition({50, (float)i * 58});
    }
    for (int i = 0; i < Alp; ++i) {
        nums[i].setOnlyNumbers(1);
        nums[i].setButtonSettings({170, 45}, 3, Color::White);
        nums[i].setButtonPosition({90, (float)i * 58});
        nums[i].setTextSettings(40, Color::Black);
        nums[i].setTextPosition({95, (float)i * 58});
    }
    Text res;
    res.setFillColor(Color::Black);
    res.setFont(font);
    res.setCharacterSize(40);
    res.setPosition(815, 1450);
    while (input.isOpen()) {
        input.clear(Color(200, 200, 255));
        for (int i = 0; i < Alp; ++i) {
            input.draw(arr[i]);
            nums[i].draw(input);
        }
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            for (int i = 0; i < Alp; ++i) {
                nums[i].change(event, mouse_position);
            }
        }
        std::vector<float> xs(Alp, 0);
        for (int i = 0; i < Alp; ++i) {
            if (nums[i].value != "") {
                xs[i] = std::stoi(nums[i].value);
            }
        }
        float val = get(at(pos), xs);
        if (val == (int)val) {
            res.setString(std::to_string((int)val));
        } else {
            res.setString(std::to_string(val));
        }
        input.draw(res);
        input.display();
    }
}

void renderGet() {
    RenderWindow input(VideoMode(2500, 500), "get", Style::Close | Style::Titlebar);
    InputField left;
    left.setOnlyNumbers(1);
    left.setButtonPosition({100, 100});
    left.setButtonSettings({250, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 3, 100});
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({940, 300});
    std::string error;
    while (input.isOpen()) {
        input.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            left.change(event, mouse_position);
        }
        left.draw(input);
        if (!left.chosen) {
            if (left.value != "") {
                try {
                    int pos = std::stoi(left.value);
                    if (pos >= size) throw 1;
                    input.close();
                    renderChoose(pos);
                } catch (int e) {
                    if (e == 1) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString("index is out of bounds");
                        input.draw(verdict);
                    }
                    if (e == 2) {
                        verdict.setFillColor(Color::Red);
                        verdict.setString(error);
                        input.draw(verdict);
                    }
                }
            }
        }
        input.display();
    }
}

void printBase() {
    float dy = 100;
    float sz = 45;
    for (int i = id; i < std::min((int)size, id + size_of); ++i) {
        std::string now = "";
        now += std::to_string(i);
        now += ": ";
        now += print(at(i));
        Text pol;
        pol.setFillColor(Color::Black);
        pol.setFont(font);
        pol.setCharacterSize(sz);
        pol.setPosition({0, dy * (i - id)});
        pol.setString(now);
        window.draw(pol);
    }
    return;
}

void renderSave() {
    RenderWindow input(VideoMode(2750, 300), "input", Style::Close | Style::Titlebar);
    InputField left;
    left.setButtonPosition({100, 100});
    left.setButtonSettings({2300, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 15, 100});
    Button save;
    save.setButtonColor(Color::White);
    save.setButtonSize({200, 100});
    save.setButtonPosition({2500, 170});
    save.setTitlePosition({2547, 170 + 16});
    save.setTitle("save", 50);
    while (input.isOpen()) {
        input.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            left.change(event, mouse_position);
            if (save.pressed(mouse_position, event)) {
                if (left.value == "") continue;
                writeToFile(left.value);
                input.close();
            }
        }
        left.draw(input);
        save.draw(input);
        input.display();
    }
}

void renderLoad() {
    RenderWindow input(VideoMode(2750, 300), "input", Style::Close | Style::Titlebar);
    InputField left;
    left.setButtonPosition({100, 100});
    left.setButtonSettings({2300, 100}, 3, Color::White);
    left.setTextSettings(70, Color::Black);
    left.setTextPosition({100 + 15, 100});
    Button save;
    save.setButtonColor(Color::White);
    save.setButtonSize({200, 100});
    save.setButtonPosition({2500, 170});
    save.setTitlePosition({2547, 170 + 16});
    save.setTitle("load", 50);
    Text verdict;
    verdict.setFont(font);
    verdict.setCharacterSize(70);
    verdict.setPosition({1100, 200});
    bool error = 0;
    while (input.isOpen()) {
        input.clear(Color(200, 200, 255));
        Event event;
        Vector2i mouse_position = Mouse::getPosition(input);
        while (input.pollEvent(event)) {
            if (event.type == Event::Closed) {
                input.close();
            }
            left.change(event, mouse_position);
            if (save.pressed(mouse_position, event)) {
                if (left.value == "") continue;
                loadFromFile(left.value);
                try {
                    if (loaded) {
                        input.close();
                        return;
                    }
                    throw 1;
                }
                catch (int e) {
                    if (e == 1) {
                        std::cout << 1 << '\n';
                        error = 1;
                    }
                }
            }
        }
        if (error) {
            verdict.setString("no such a file");
            verdict.setFillColor(Color::Red);
            input.draw(verdict);
        }
        left.draw(input);
        save.draw(input);
        input.display();
    }
}

int main() {
    font.loadFromFile("/Users/alexsus/Desktop/polynomials/arial.ttf");
    Button plus, multiplication, derivative, division;
    plus.setButtonColor(Color::White);
    multiplication.setButtonColor(Color::White);
    division.setButtonColor(Color::White);
    derivative.setButtonColor(Color::White);

    plus.setButtonSize({100, 100});
    plus.setButtonPosition({1700, 100 - 80});
    plus.setTitlePosition({1700 + 33, 100 + 22 - 80});
    plus.setTitle("+", 50);

    multiplication.setButtonSize({100, 100});
    multiplication.setButtonPosition({1700, 250 - 80});
    multiplication.setTitlePosition({1700 + 38, 250 + 30 - 80});
    multiplication.setTitle("*", 50);

    division.setButtonSize({100, 100});
    division.setButtonPosition({1700, 400 - 80});
    division.setTitlePosition({1700 + 40, 400 + 20 - 80});
    division.setTitle("/", 50);

    derivative.setButtonSize({100, 100});
    derivative.setButtonPosition({1700, 550 - 80});
    derivative.setTitlePosition({1700 + 44, 550 + 30 - 80});
    derivative.setTitle("'", 50);

    Button save;
    save.setButtonColor(Color::White);
    save.setButtonSize({200, 100});
    save.setButtonPosition({1700, 700 - 80});
    save.setTitlePosition({1747, 700 + 16 - 80});
    save.setTitle("save", 50);

    Button add;
    add.setButtonColor(Color::White);
    add.setButtonSize({200, 100});
    add.setButtonPosition({1700, 850 - 80});
    add.setTitlePosition({1755, 850 + 16 - 80});
    add.setTitle("add", 50);

    Button load;
    load.setButtonColor(Color::White);
    load.setButtonSize({200, 100});
    load.setButtonPosition({1700, 1000 - 80});
    load.setTitlePosition({1747, 1000 + 16 - 80});
    load.setTitle("load", 50);

    Button erase;
    erase.setButtonColor(Color::White);
    erase.setButtonSize({200, 100});
    erase.setButtonPosition({1700, 1150 - 80});
    erase.setTitlePosition({1740, 1150 + 16 - 80});
    erase.setTitle("erase", 50);

    Button roots;
    roots.setButtonColor(Color::White);
    roots.setButtonSize({200, 100});
    roots.setButtonPosition({1700, 1300 - 80});
    roots.setTitlePosition({1740, 1300 + 16 - 80});
    roots.setTitle("roots", 50);

    Button get;
    get.setButtonColor(Color::White);
    get.setButtonSize({200, 100});
    get.setButtonPosition({1700, 1450 - 80});
    get.setTitlePosition({1760, 1450 + 16 - 80});
    get.setTitle("get", 50);

    while (window.isOpen()) {
        window.clear(Color(200, 200, 255));
        printBase();
        Event event;
        Vector2i mouse_position = Mouse::getPosition(window);
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } else if(event.type == Event::MouseWheelMoved) {
                std::cout << event.mouseWheel.delta << '\n';
                int nid = id + event.mouseWheel.delta / 1.5;
                id = std::max(0, nid);
                id = std::min(std::max(size - size_of, 0), id);
            }
            if (save.pressed(mouse_position, event)) {
                renderSave();
            } else if (plus.pressed(mouse_position, event)) {
                renderPlus();
            } else if (multiplication.pressed(mouse_position, event)) {
                renderMult();
            } else if (load.pressed(mouse_position, event)) {
                renderLoad();
            } else if (division.pressed(mouse_position, event)) {
                renderDivision();
            } else if (derivative.pressed(mouse_position, event)) {
                renderDerivative();
            } else if (add.pressed(mouse_position, event)) {
                renderAdd();
            } else if (erase.pressed(mouse_position, event)) {
                renderErase();
            } else if (roots.pressed(mouse_position, event)) {
                renderRoots();
            } else if (get.pressed(mouse_position, event)) {
                renderGet();
            }
        }
        load.draw(window);
        plus.draw(window);
        multiplication.draw(window);
        division.draw(window);
        derivative.draw(window);
        save.draw(window);
        add.draw(window);
        erase.draw(window);
        roots.draw(window);
        get.draw(window);
        window.display();
    }
    return 0;
}
