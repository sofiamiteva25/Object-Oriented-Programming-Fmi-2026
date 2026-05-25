# Visitor и Command Design Patterns в C++

---

##  Съдържание

**Visitor Design Pattern**

1. [Какво е Visitor?](#1-какво-е-visitor)
2. [Проблемът без Visitor](#2-проблемът-без-visitor)
3. [Решението — Visitor Pattern](#3-решението--visitor-pattern)
4. [Double Dispatch и Visitor](#4-double-dispatch-и-visitor)
5. [Пример — Клиенти и операции](#5-пример--клиенти-и-операции)
6. [Пример — Файлова система](#6-пример--файлова-система)
7. [Пример — A, B, C се срещат](#7-пример--a-b-c-се-срещат)

**Command Design Pattern**

8. [Какво е Command?](#8-какво-е-command)
9. [Проблемът без Command](#9-проблемът-без-command)
10. [Решението — Command Pattern](#10-решението--command-pattern)
11. [Undo/Redo с Command](#11-undoredo-с-command)
12. [Edge Cases и капани](#12-edge-cases-и-капани)
13. [Обобщение](#13-обобщение)

---

## Основни дефиниции

> **Visitor Design Pattern** — извличаме дадена операция от класовете в отделен клас (Visitor). Всеки клас „приема" Visitor-а чрез `accept()` и му казва кой е чрез `this`. Така добавяме нови операции без да пипаме съществуващите класове.

> **Double Dispatch** — при всяко `accept()` разкриваме типа на обекта, при всяко `visitX()` разкриваме типа на операцията. Резултатът зависи от **двата** типа едновременно.

> **Command Design Pattern** — капсулираме дадено действие в обект. Позволява undo, опашка от действия и отложено изпълнение.

---

## 1. Какво е Visitor?

### Идеята с примера

Имаме човек, който всеки ден обикаля различни сгради:

- В **Къща** → спи
- В **Фабрика** → работи
- В **Банка** → тегли пари

Имаме списък от сгради `vector<unique_ptr<IBuilding>>`. Проблемът — зад всеки `IBuilding*` може да стои различен тип. Как да разберем кой?

### ❌ Вариант 1 — dynamic_cast (instanceof)

Първата идея, която идва на ум: проверяваме ръчно типа с `dynamic_cast`.

```cpp
#include <iostream>
#include <vector>
#include <memory>

class IBuilding {
public:
    virtual ~IBuilding() = default;
};

class House   : public IBuilding {};
class Factory : public IBuilding {};
class Bank    : public IBuilding {};

void personVisits(const IBuilding* building) {
    if (dynamic_cast<const House*>(building)) {
        std::cout << "спи\n";
    } else if (dynamic_cast<const Factory*>(building)) {
        std::cout << "работи\n";
    } else if (dynamic_cast<const Bank*>(building)) {
        std::cout << "тегли пари\n";
    }
}

int main() {
    std::vector<std::unique_ptr<IBuilding>> buildings;
    buildings.push_back(std::make_unique<House>());
    buildings.push_back(std::make_unique<Factory>());
    buildings.push_back(std::make_unique<Bank>());

    for (const auto& b : buildings)
        personVisits(b.get());
}
```

```
спи
работи
тегли пари
```

Работи. Но сега искаме **инспектор**, който обикаля същите сгради и прави различни неща:

```cpp
void inspectorVisits(const IBuilding* building) {
    if (dynamic_cast<const House*>(building)) {
        std::cout << "инспектира жилище\n";
    } else if (dynamic_cast<const Factory*>(building)) {
        std::cout << "инспектира фабрика\n";
    } else if (dynamic_cast<const Bank*>(building)) {
        std::cout << "инспектира банка\n";
    }
}
```

И сега добавяме нова сграда — **Hotel**:

```cpp
class Hotel : public IBuilding {};
```

Трябва да намерим **всеки** `if-else` блок в целия проект и да добавиш `Hotel` навсякъде:

```cpp
// personVisits    → добавяш else if (Hotel)
// inspectorVisits → добавяш else if (Hotel)
// firemanVisits   → добавяш else if (Hotel)
// taxVisits       → добавяш else if (Hotel)
// ...
```

```
❌ Компилаторът не те предупреждава ако забравиш някой if-else блок
❌ Всяка нова сграда → търсиш и редактираш навсякъде в кода
❌ Всяка нова операция → нова функция с пълен if-else блок
❌ dynamic_cast е бавен — проверява типа по runtime всеки път
```

---

### ❌ Вариант 2 — метод в базовия клас за всяка операция

Втората идея: слагаш операцията директно в `IBuilding`.

```cpp
class IBuilding {
public:
    virtual void personVisit()    const = 0;   // ← човекът
    virtual void inspectorVisit() const = 0;   // ← инспекторът
    virtual void firemanVisit()   const = 0;   // ← пожарникарят (нова заявка)
    virtual ~IBuilding() = default;
};

class House : public IBuilding {
public:
    void personVisit()    const override { std::cout << "спи\n"; }
    void inspectorVisit() const override { std::cout << "инспектира жилище\n"; }
    void firemanVisit()   const override { std::cout << "проверява изходи\n"; }
};

// Factory и Bank — аналогично, всеки с 3 метода
```

При нова операция — `taxCollectorVisit()`:

```cpp
// Добавяш в IBuilding → чисто нов виртуален метод
// Добавяш в House     → имплементираш
// Добавяш в Factory   → имплементираш
// Добавяш в Bank      → имплементираш
```

```
❌ Всяка нова операция → редактираш IBuilding + всеки наследник
❌ IBuilding знае за хора, инспектори, пожарникари — не е негова работа
❌ При 5 операции × 4 сгради = 20 метода разпръснати из класовете
❌ Нарушава SRP — сградата отговаря за всичко
```

---

### ✅ Вариант 3 — Visitor Pattern

**не питаме сградата какво да прави — казваме й кой сме ние и тя ни пуска вътре**.

Всяка сграда има само един метод: `accept(visitor)`. Вътре в него казва на посетителя "аз съм House/Factory/Bank" чрез `this`. Посетителят сам решава какво да прави.

```cpp
#include <iostream>
#include <vector>
#include <memory>

// Forward declarations — само за да знае IVisitor за конкретните типове:
class House;
class Factory;
class Bank;

// IVisitor — интерфейс с метод за всеки тип сграда:
class IVisitor {
public:
    virtual void visitHouse(const House* h)       = 0;
    virtual void visitFactory(const Factory* f)   = 0;
    virtual void visitBank(const Bank* b)         = 0;
    virtual ~IVisitor() = default;
};

// IBuilding — само accept(), нищо повече:
class IBuilding {
public:
    virtual void accept(IVisitor* visitor) const = 0;
    virtual ~IBuilding() = default;
};

class House : public IBuilding {
public:
    void accept(IVisitor* visitor) const override {
        visitor->visitHouse(this);
        //                   ^^^^
        //  this е House* — конкретен тип, не IBuilding*
        //  Казваме на visitor: "аз съм House, ти решаваш какво да правиш"
    }
};

class Factory : public IBuilding {
public:
    void accept(IVisitor* visitor) const override {
        visitor->visitFactory(this);   // this е Factory*
    }
};

class Bank : public IBuilding {
public:
    void accept(IVisitor* visitor) const override {
        visitor->visitBank(this);   // this е Bank*
    }
};
```

```cpp
// Операция 1 — Човекът:
class PersonVisitor : public IVisitor {
public:
    void visitHouse(const House* h)       override { std::cout << "спи\n"; }
    void visitFactory(const Factory* f)   override { std::cout << "работи\n"; }
    void visitBank(const Bank* b)         override { std::cout << "тегли пари\n"; }
};

// Операция 2 — Инспекторът (нов клас, нищо не е пипнато):
class InspectorVisitor : public IVisitor {
public:
    void visitHouse(const House* h)       override { std::cout << "инспектира жилище\n"; }
    void visitFactory(const Factory* f)   override { std::cout << "инспектира фабрика\n"; }
    void visitBank(const Bank* b)         override { std::cout << "инспектира банка\n"; }
};

// Операция 3 — Пожарникарят (пак нов клас, пак нищо не е пипнато):
class FiremanVisitor : public IVisitor {
public:
    void visitHouse(const House* h)       override { std::cout << "проверява изходи\n"; }
    void visitFactory(const Factory* f)   override { std::cout << "проверява спринклери\n"; }
    void visitBank(const Bank* b)         override { std::cout << "проверява трезора\n"; }
};

int main() {
    std::vector<std::unique_ptr<IBuilding>> buildings;
    buildings.push_back(std::make_unique<House>());
    buildings.push_back(std::make_unique<Factory>());
    buildings.push_back(std::make_unique<Bank>());

    PersonVisitor    person;
    InspectorVisitor inspector;
    FiremanVisitor   fireman;

    std::cout << "=== Човекът обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(&person);

    std::cout << "\n=== Инспекторът обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(&inspector);

    std::cout << "\n=== Пожарникарят обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(&fireman);
}
```

```
=== Човекът обикаля ===
спи
работи
тегли пари

=== Инспекторът обикаля ===
инспектира жилище
инспектира фабрика
инспектира банка

=== Пожарникарят обикаля ===
проверява изходи
проверява спринклери
проверява трезора
```

**Добавихме 3 операции без да пипнем House, Factory, Bank нито веднъж.**

---

### Сравнение на трите варианта

```
                    dynamic_cast     метод в класа     Visitor
─────────────────────────────────────────────────────────────────
Нова операция    → нова функция     редактираш        нов клас
                   с if-else        всеки клас         само      ✅
                   блок
Нова сграда      → добавяш в        добавяш в         добавяш в
                   всяка функция    базовия клас      IVisitor +
                                    + всеки клас      всеки Visitor
Компилаторът     → не те            не те             те предупреж-
те предупреждава   предупреждава    предупреждава     дава ако     ✅
при пропуск                                           забравиш
                                                      visitX()
Скорост          → бавен            бърз              бърз         ✅
                   (runtime check)  (vtable)          (vtable)
SRP              → нарушен          нарушен           спазен       ✅
```

---

## 2. Проблемът без Visitor

```cpp
// Имаме работеща йерархия:
class IClient {
public:
    virtual std::string getName() const = 0;
    virtual ~IClient() = default;
};

class Bank : public IClient {
public:
    std::string getName() const override { return "National Bank"; }
};

class Company : public IClient {
public:
    std::string getName() const override { return "Company FMI.com"; }
};

class Restaurant : public IClient {
public:
    std::string getName() const override { return "Restaurant FMIlicious"; }
};
```

Мениджърът иска нова функционалност — изпращане на имейли:

```cpp
// ❌ Нарушава SRP и OCP:
class IClient {
public:
    virtual std::string getName()  const = 0;
    virtual void sendMail()        const = 0;   // ← добавяме
    virtual void generateReport()  const = 0;   // ← пак добавяме
    virtual ~IClient() = default;
};
// При нова операция → редактираме Bank, Company, Restaurant!
```

С `dynamic_cast` — също лошо:

```cpp
// ❌ При нов тип → нов else if навсякъде:
void sendMail(IClient* client) {
    if (Bank* b = dynamic_cast<Bank*>(client))
        std::cout << "Имейл до банка: " << b->getName() << "\n";
    else if (Company* c = dynamic_cast<Company*>(client))
        std::cout << "Имейл до компания: " << c->getName() << "\n";
}
```

```
❌ SRP нарушен — IClient отговаря за данни И за имейли И за репорти
❌ OCP нарушен — при нова операция редактираме всеки клас
❌ При 5 операции × 5 типа = 25 места за промяна
```

---

## 3. Решението — Visitor Pattern

### Стъпка 1 — IVisitor интерфейс

```cpp
// Forward declarations:
class Bank;
class Company;
class Restaurant;

class IVisitor {
public:
    virtual void visitBank(const Bank* bank)           = 0;
    virtual void visitCompany(const Company* company)  = 0;
    virtual void visitRestaurant(const Restaurant* r)  = 0;
    virtual ~IVisitor() = default;
};
```

### Стъпка 2 — `accept()` в базовия клас

```cpp
class IClient {
public:
    virtual std::string getName()            const = 0;
    virtual void accept(IVisitor* visitor)         = 0;
    virtual ~IClient() = default;
};
```

### Стъпка 3 — Всеки наследник се „представя"

```cpp
class Bank : public IClient {
public:
    std::string getName() const override { return "National Bank"; }

    void accept(IVisitor* visitor) override {
        visitor->visitBank(this);   // this е Bank*
    }
};

class Company : public IClient {
public:
    std::string getName() const override { return "Company FMI.com"; }

    void accept(IVisitor* visitor) override {
        visitor->visitCompany(this);   // this е Company*
    }
};

class Restaurant : public IClient {
public:
    std::string getName() const override { return "Restaurant FMIlicious"; }

    void accept(IVisitor* visitor) override {
        visitor->visitRestaurant(this);   // this е Restaurant*
    }
};
```

### Стъпка 4 — Конкретни Visitor-и

```cpp
class InsuranceMailVisitor : public IVisitor {
public:
    void visitBank(const Bank* b) override {
        std::cout << "Застрахователен имейл до банка: " << b->getName() << "\n";
    }
    void visitCompany(const Company* c) override {
        std::cout << "Застрахователен имейл до компания: " << c->getName() << "\n";
    }
    void visitRestaurant(const Restaurant* r) override {
        std::cout << "Застрахователен имейл до ресторант: " << r->getName() << "\n";
    }
};

// Нова операция — само нов клас, нищо не е пипнато!
class MonthlyReportVisitor : public IVisitor {
public:
    void visitBank(const Bank* b) override {
        std::cout << "Месечен отчет за банка: " << b->getName() << "\n";
    }
    void visitCompany(const Company* c) override {
        std::cout << "Месечен отчет за компания: " << c->getName() << "\n";
    }
    void visitRestaurant(const Restaurant* r) override {
        std::cout << "Месечен отчет за ресторант: " << r->getName() << "\n";
    }
};
```

### Стъпка 5 — Употреба с `vector<unique_ptr<IClient>>`

```cpp
int main() {
    std::vector<std::unique_ptr<IClient>> clients;
    clients.push_back(std::make_unique<Bank>());
    clients.push_back(std::make_unique<Company>());
    clients.push_back(std::make_unique<Restaurant>());

    InsuranceMailVisitor  mailVisitor;
    MonthlyReportVisitor  reportVisitor;

    std::cout << "=== Застрахователни имейли ===\n";
    for (const auto& c : clients)
        c->accept(&mailVisitor);

    std::cout << "\n=== Месечни отчети ===\n";
    for (const auto& c : clients)
        c->accept(&reportVisitor);
}
```

```
=== Застрахователни имейли ===
Застрахователен имейл до банка: National Bank
Застрахователен имейл до компания: Company FMI.com
Застрахователен имейл до ресторант: Restaurant FMIlicious

=== Месечни отчети ===
Месечен отчет за банка: National Bank
Месечен отчет за компания: Company FMI.com
Месечен отчет за ресторант: Restaurant FMIlicious
```

---

## 4. Double Dispatch и Visitor

### Single dispatch — стандартното виртуално извикване

В C++ едно виртуално извикване разкрива **един** тип — типа на обекта, върху който се вика:

```cpp
Shape* s = new Circle(...);
s->getArea();   // → Circle::getArea()
//  ^^^^^^
//  знаем само типа на s — Circle
```

Това е *single dispatch* — резултатът зависи само от **един** тип.

### Кога single dispatch не стига?

Имаме конкретен проблем: **пресичане на фигури**.

```cpp
Shape* a = new Circle(...);
Shape* b = new Rectangle(...);

a->intersects(b);   // ← каква формула трябва да се изпълни?
```

Проблемът е, че правилната формула зависи от **два** типа едновременно:

```
Circle    ∩ Circle    → разстояние между центровете
Circle    ∩ Rectangle → напълно различна формула
Triangle  ∩ Rectangle → пак различна
```

С едно виртуално извикване знаеш само типа на `a`. Типът на `b` си остава скрит зад `Shape*`.

### ❌ Наивното решение — dynamic_cast

```cpp
bool Circle::intersects(const Shape* other) const {
    if (const Rectangle* r = dynamic_cast<const Rectangle*>(other)) {
        std::cout << "Circle ∩ Rectangle\n";
        return true;
    }
    if (const Triangle* t = dynamic_cast<const Triangle*>(other)) {
        std::cout << "Circle ∩ Triangle\n";
        return true;
    }
    // ...
}
```

```
❌ dynamic_cast е бавен — проверява типа по runtime
❌ При нова фигура Pentagon → намираш всеки if-else и го редактираш
❌ Компилаторът не те предупреждава ако пропуснеш някой случай
```

### ✅ Double dispatch — две виртуални извиквания

Вместо да питаме `b` кой е чрез `dynamic_cast`, нека `b` сам се представи — точно както при Visitor.

Идеята: правим **две** виртуални извиквания. Всяко разкрива по един тип:

```
Извикване 1: a->intersectsWith(b)
             → разкрива типа на a (Circle)
             → вътре казваме на b: "аз съм Circle"

Извикване 2: b->intersectsWithCircle(a)
             → разкрива типа на b (Rectangle)
             → знаем и двата типа → правилната формула ✅
```

### Имплементацията с фигурите

```cpp
// Shape.h
class Triangle;
class Rectangle;
class Circle;

class Shape {
public:
    // Dispatch #1 — всяка фигура казва "аз съм ТО" на другата:
    virtual bool intersectsWith(const Shape* other) const = 0;

    // Dispatch #2 — конкретната формула за всяка комбинация:
    virtual bool intersectsWithCircle(const Circle* other)     const = 0;
    virtual bool intersectsWithRect(const Rectangle* other)    const = 0;
    virtual bool intersectsWithTriangle(const Triangle* other) const = 0;
};
```

```cpp
// Circle.cpp

// Dispatch #1 — Circle се представя на other:
bool Circle::intersectsWith(const Shape* other) const {
    return other->intersectsWithCircle(this);
    //                                 ^^^^
    //  this е Circle* — конкретен тип, не Shape*!
    //  Казваме на other: "аз съм Circle, ти реши какво да правиш"
}

// Dispatch #2 — формулите когато другата фигура се е представила:
bool Circle::intersectsWithCircle(const Circle* other) const {
    std::cout << "Circle ∩ Circle: разстояние между центровете\n";
    return true;
}
bool Circle::intersectsWithRect(const Rectangle* other) const {
    std::cout << "Circle ∩ Rectangle: формула\n";
    return true;
}
bool Circle::intersectsWithTriangle(const Triangle* other) const {
    std::cout << "Circle ∩ Triangle: формула\n";
    return true;
}
```

```cpp
// Rectangle.cpp — аналогично, но се представя като Rectangle:
bool Rectangle::intersectsWith(const Shape* other) const {
    return other->intersectsWithRect(this);   // this е Rectangle*
}

bool Rectangle::intersectsWithCircle(const Circle* other) const {
    std::cout << "Rectangle ∩ Circle: формула\n";
    return true;
}
bool Rectangle::intersectsWithRect(const Rectangle* other) const {
    std::cout << "Rectangle ∩ Rectangle: формула\n";
    return true;
}
bool Rectangle::intersectsWithTriangle(const Triangle* other) const {
    std::cout << "Rectangle ∩ Triangle: формула\n";
    return true;
}
```

### Проследи конкретно извикване: Circle ∩ Rectangle

```cpp
Shape* a = new Circle(0, 0, 5);
Shape* b = new Rectangle(1, 1, 4, 4);

a->intersectsWith(b);
```

```
Стъпка 1: a->intersectsWith(b)
          └─ a е Circle → Circle::intersectsWith се извиква
                вътре: b->intersectsWithCircle(this)
                                               ^^^^
                                               Circle* — конкретен тип

Стъпка 2: b->intersectsWithCircle(circle_ptr)
          └─ b е Rectangle → Rectangle::intersectsWithCircle се извиква
                знаем: b е Rectangle, аргументът е Circle
                → "Rectangle ∩ Circle: формула"  ✅
```

Визуално — всяка двойка типове е отделна клетка в матрицата:

```
         a \ b  │  Circle       Rectangle    Triangle
        ────────┼──────────────────────────────────────
         Circle │  C∩C          C∩R          C∩T
      Rectangle │  R∩C          R∩R          R∩T
       Triangle │  T∩C          T∩R          T∩T
```

Всяка клетка се достига чрез точно два виртуални извиквания — по едно за всяка ос.

### Защо `this` е ключово

```cpp
bool Circle::intersectsWith(const Shape* other) const {
    other->intersectsWithCircle(this);
    //                          ^^^^
    //  this е Circle*, НЕ Shape*!
    //
    //  Компилаторът вижда конкретния тип на this в compile time
    //  → избира правилния метод intersectsWithCircle()
    //
    //  Ако кастнехме: other->intersectsWithCircle((Shape*)this)
    //  → Shape няма intersectsWithCircle(Shape*) → не компилира
    //  → целта е изгубена
}
```

---

## 5. Пример — Клиенти и операции

Вече показан в секции 2-3. Ключовото:

```
Нова операция  → нов Visitor клас     (IClient не се пипа)
Нов тип клиент → нов IClient наследник + обнови всички Visitor-и
```

---

## 6. Пример — Файлова система

```cpp
class IFile;
class IDirectory;

class IFileSystemVisitor {
public:
    virtual void visitFile(const IFile* f)           = 0;
    virtual void visitDirectory(const IDirectory* d) = 0;
    virtual ~IFileSystemVisitor() = default;
};

class IFileSystemEntity {
protected:
    std::string name;
public:
    explicit IFileSystemEntity(const std::string& n) : name(n) {}
    const std::string& getName() const { return name; }

    virtual void accept(IFileSystemVisitor* v) const = 0;
    virtual ~IFileSystemEntity() = default;
};

class IFile : public IFileSystemEntity {
public:
    explicit IFile(const std::string& n) : IFileSystemEntity(n) {}

    void accept(IFileSystemVisitor* v) const override {
        v->visitFile(this);   // this е IFile*
    }
};

class IDirectory : public IFileSystemEntity {
    std::vector<std::unique_ptr<IFileSystemEntity>> children;
public:
    explicit IDirectory(const std::string& n) : IFileSystemEntity(n) {}

    void addChild(std::unique_ptr<IFileSystemEntity> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<IFileSystemEntity>>& getChildren() const {
        return children;
    }

    void accept(IFileSystemVisitor* v) const override {
        v->visitDirectory(this);   // this е IDirectory*
    }
};

// Visitor 1 — принтира структурата:
class PrintVisitor : public IFileSystemVisitor {
    mutable int indent = 0;

    void printIndent() const {
        for (int i = 0; i < indent; i++) std::cout << "  ";
    }
public:
    void visitFile(const IFile* f) const override {
        printIndent();
        std::cout << "F " << f->getName() << "\n";
    }

    void visitDirectory(const IDirectory* d) const override {
        printIndent();
        std::cout << "D " << d->getName() << "\n";
        indent++;
        for (const auto& child : d->getChildren())
            child->accept(const_cast<PrintVisitor*>(this));
        indent--;
    }
};

// Visitor 2 — брои файловете:
class CountFilesVisitor : public IFileSystemVisitor {
    mutable int count = 0;
public:
    void visitFile(const IFile* f) const override { count++; }
    void visitDirectory(const IDirectory* d) const override {
        for (const auto& child : d->getChildren())
            child->accept(const_cast<CountFilesVisitor*>(this));
    }
    int getCount() const { return count; }
};

int main() {
    auto root = std::make_unique<IDirectory>("src/");
    root->addChild(std::make_unique<IFile>("main.cpp"));
    root->addChild(std::make_unique<IFile>("README.md"));

    auto utils = std::make_unique<IDirectory>("utils/");
    utils->addChild(std::make_unique<IFile>("helper.h"));
    utils->addChild(std::make_unique<IFile>("helper.cpp"));
    root->addChild(std::move(utils));

    PrintVisitor      printer;
    CountFilesVisitor counter;

    root->accept(&printer);
    root->accept(&counter);
    std::cout << "Общо файлове: " << counter.getCount() << "\n";
}
```

```
D src/
  F main.cpp
  F README.md
  D utils/
    F helper.h
    F helper.cpp
Общо файлове: 4
```

---

## 7. Пример — A, B, C се срещат

Visitor при **взаимодействие между обекти** — резултатът зависи от типовете и на **двамата**:

```cpp
class A;
class B;
class C;

class IBase {
public:
    virtual void meet(const IBase* other) const = 0;

    virtual void meetA(const A* other) const = 0;
    virtual void meetB(const B* other) const = 0;
    virtual void meetC(const C* other) const = 0;

    virtual ~IBase() = default;
};

class A : public IBase {
public:
    void meet(const IBase* other) const override {
        other->meetA(this);   // казва на other: "аз съм A"
    }
    void meetA(const A* other) const override { std::cout << "A среща A\n"; }
    void meetB(const B* other) const override { std::cout << "B среща A\n"; }
    void meetC(const C* other) const override { std::cout << "C среща A\n"; }
};

class B : public IBase {
public:
    void meet(const IBase* other) const override {
        other->meetB(this);   // казва на other: "аз съм B"
    }
    void meetA(const A* other) const override { std::cout << "A среща B\n"; }
    void meetB(const B* other) const override { std::cout << "B среща B\n"; }
    void meetC(const C* other) const override { std::cout << "C среща B\n"; }
};

class C : public IBase {
public:
    void meet(const IBase* other) const override {
        other->meetC(this);   // казва на other: "аз съм C"
    }
    void meetA(const A* other) const override { std::cout << "A среща C\n"; }
    void meetB(const B* other) const override { std::cout << "B среща C\n"; }
    void meetC(const C* other) const override { std::cout << "C среща C\n"; }
};

int main() {
    std::vector<std::unique_ptr<IBase>> objects;
    objects.push_back(std::make_unique<A>());
    objects.push_back(std::make_unique<B>());
    objects.push_back(std::make_unique<C>());

    // Всеки среща всеки:
    for (const auto& x : objects)
        for (const auto& y : objects)
            x->meet(y.get());
}
```

```
A среща A
A среща B
A среща C
B среща A
B среща B
B среща C
C среща A
C среща B
C среща C
```

**Стъпките при `a.meet(&b)`:**

```
1. a.meet(&b) → A::meet
   this е A* → извикваме b->meetA(this)

2. b->meetA(&a) → B::meetA
   знаем: this е B, other е A → "A среща B"
```

---

## 8. Какво е Command?

> Command капсулира **дадено действие** в обект. Вместо да извикваш функция директно, създаваш обект, който описва какво трябва да се направи и как да се отмени.

```
✅ Undo/Redo
✅ Опашка от действия — изпълняват се по-късно
✅ Логване на всички действия
```

---

## 9. Проблемът без Command

```cpp
// ❌ Директно извикване — не може undo:
class TextEditor {
    std::string text;
public:
    void insertText(const std::string& str) { text += str; }
    void deleteLastN(size_t n) {
        if (n <= text.size()) text.erase(text.size() - n, n);
    }
    void print() const { std::cout << "[" << text << "]\n"; }
};

int main() {
    TextEditor editor;
    editor.insertText("Здравей, свят!");
    // Как да отменим? Невъзможно!
}
```

---

## 10. Решението — Command Pattern

### ICommand интерфейс

```cpp
class ICommand {
public:
    virtual void execute() = 0;
    virtual void undo()    = 0;
    virtual ~ICommand() = default;
};
```

### Document — обектът, върху който се работи

```cpp
class Document {
    std::string text;
public:
    void insert(const std::string& str) { text += str; }

    void deleteLastN(size_t n) {
        if (n <= text.size()) text.erase(text.size() - n, n);
    }

    const std::string& getText() const { return text; }
    void print() const { std::cout << "[" << text << "]\n"; }
};
```

### Конкретни команди

```cpp
class InsertCommand : public ICommand {
    Document&   doc;
    std::string inserted;
public:
    InsertCommand(Document& d, const std::string& text)
        : doc(d), inserted(text) {}

    void execute() override { doc.insert(inserted); }
    void undo()    override { doc.deleteLastN(inserted.size()); }
};

class DeleteCommand : public ICommand {
    Document&   doc;
    std::string deleted;   // запомняме за undo
    size_t      count;
public:
    DeleteCommand(Document& d, size_t n) : doc(d), count(n) {}

    void execute() override {
        const auto& t = doc.getText();
        if (count <= t.size())
            deleted = t.substr(t.size() - count);
        doc.deleteLastN(count);
    }

    void undo() override { doc.insert(deleted); }
};
```

### TextEditor — Invoker

```cpp
class TextEditor {
    Document                              doc;
    std::stack<std::unique_ptr<ICommand>> history;

public:
    void execute(std::unique_ptr<ICommand> cmd) {
        cmd->execute();
        history.push(std::move(cmd));
    }

    void undo() {
        if (history.empty()) {
            std::cout << "Няма какво да се отмени\n";
            return;
        }
        history.top()->undo();
        history.pop();
    }

    void insert(const std::string& text) {
        execute(std::make_unique<InsertCommand>(doc, text));
    }

    void deleteLast(size_t n) {
        execute(std::make_unique<DeleteCommand>(doc, n));
    }

    void print() const { doc.print(); }
};

int main() {
    TextEditor editor;

    editor.insert("Здравей");    editor.print();   // [Здравей]
    editor.insert(", свят!");    editor.print();   // [Здравей, свят!]
    editor.undo();               editor.print();   // [Здравей]
    editor.deleteLast(3);        editor.print();   // [Здра]
    editor.undo();               editor.print();   // [Здравей]
    editor.undo();               editor.print();   // []
    editor.undo();                                 // Няма какво да се отмени
}
```

```
[Здравей]
[Здравей, свят!]
[Здравей]
[Здра]
[Здравей]
[]
Няма какво да се отмени
```

---

## 11. Undo/Redo с Command

```cpp
class TextEditorWithRedo {
    Document doc;
    std::stack<std::unique_ptr<ICommand>> undoStack;
    std::stack<std::unique_ptr<ICommand>> redoStack;

public:
    void execute(std::unique_ptr<ICommand> cmd) {
        cmd->execute();
        undoStack.push(std::move(cmd));
        while (!redoStack.empty()) redoStack.pop();   // нова команда изчиства redo
    }

    void undo() {
        if (undoStack.empty()) return;
        undoStack.top()->undo();
        redoStack.push(std::move(undoStack.top()));
        undoStack.pop();
    }

    void redo() {
        if (redoStack.empty()) return;
        redoStack.top()->execute();
        undoStack.push(std::move(redoStack.top()));
        redoStack.pop();
    }
};
```

---

## 12. Edge Cases и капани

### Visitor — нов тип изисква обновяване на всички Visitor-и

```cpp
// Добавяме Hotel → трябва да обновиш:
// IVisitor    → добавяш visitHotel()
// InsuranceMailVisitor  → добавяш visitHotel()
// MonthlyReportVisitor  → добавяш visitHotel()
// ...всички конкретни Visitor-и

// Visitor е удобен когато типовете са стабилни, операциите се добавят.
```

---

### Visitor — циклични зависимости

```cpp
// ❌ IVisitor.h включва Bank.h, Bank.h включва IVisitor.h → цикъл!

// ✅ Решение: forward declarations в IVisitor.h:
class Bank;
class Company;
class Restaurant;

class IVisitor { ... };
```

---

### Command — запомняй state преди execute()

```cpp
// ❌ Undo е невъзможен без старото состояние:
class ResizeCommand : public ICommand {
    Shape& shape;
    double newScale;
public:
    void execute() override { shape.setScale(newScale); }
    void undo()    override { /* какво да сложим? */ }   // ❌
};

// ✅ Запомни преди промяна:
class ResizeCommand : public ICommand {
    Shape& shape;
    double newScale;
    double oldScale;   // ← задължително
public:
    void execute() override {
        oldScale = shape.getScale();   // запомняме преди!
        shape.setScale(newScale);
    }
    void undo() override { shape.setScale(oldScale); }
};
```

---

## 13. Обобщение

### Visitor Pattern — за дъска

```
IVisitor                     IClient
+ visitBank()                + accept(IVisitor*)
+ visitCompany()                    │
+ visitRestaurant()          ┌──────┴──────┐
       ▲                   Bank        Company
       │               accept() →    accept() →
InsuranceMailVisitor   visitBank(this) visitCompany(this)
MonthlyReportVisitor
```

```
Нова операция  → нов IVisitor клас    (IClient не се пипа ✅)
Нов тип клиент → нов наследник +      обнови всички Visitor-и
```

### Command Pattern — за дъска

```
ICommand               TextEditor (Invoker)
+ execute()            + execute(ICommand)
+ undo()               + undo()
     ▲                 + stack<ICommand>
     │
InsertCommand   DeleteCommand
execute() → insert    execute() → delete + запомни
undo()    → delete    undo()    → insert запомненото
```

### Правила

```
VISITOR:
✅ IVisitor с visitX() за всеки тип
✅ accept(IVisitor*) в базовия клас
✅ Всеки наследник → visitor->visitX(this)
✅ vector<unique_ptr<IBase>> за колекцията
✅ Forward declarations за циклични зависимости
❌ При нов тип → обнови IVisitor и всички конкретни Visitor-и

COMMAND:
✅ ICommand с execute() и undo()
✅ Запомни старото state ПРЕДИ execute()
✅ stack<unique_ptr<ICommand>> за history
✅ При нова команда — изчисти redo стека
```

> **Visitor:** извличаме операция в отделен клас. `accept()` + `visitX(this)` → двойно виртуално извикване → правилното действие за правилния тип.
>
> **Command:** капсулираме действие в обект → undo, опашка, логване.
