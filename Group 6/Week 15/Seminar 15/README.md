# Visitor, Command и Memento Design Patterns в C++

---

## Съдържание

**Visitor Design Pattern**

1. [Какво е Visitor?](#1-какво-е-visitor)
2. [Проблемът без Visitor](#2-проблемът-без-visitor)
3. [Решението — Visitor Pattern](#3-решението--visitor-pattern)
4. [Double Dispatch и Visitor](#4-double-dispatch-и-visitor)
5. [Пример — Клиенти и операции](#5-пример--клиенти-и-операции)
6. [Пример — Файлова система](#6-пример--файлова-система)
7. [Пример — A, B, C се срещат](#7-пример--a-b-c-се-срещат)

**Command Design Pattern**

8. [Какво е Command и какъв проблем решава?](#8-какво-е-command-и-какъв-проблем-решава)
9. [Прост пример — Text Editor](#9-прост-пример--text-editor)
10. [По-сложен пример — SwapCommand и SortCommand](#10-по-сложен-пример--swapcommand-и-sortcommand)
11. [Undo/Redo — двата стека](#11-undoredo--двата-стека)
12. [Memento Design Pattern](#12-memento-design-pattern)
13. [Memento с Game - пълно запазване на state](#13-memento-с-game---пълно-запазване-на-state)
14. [Подобна задача от интервю - CommandFactory и HistoryManager](#14-подобна-задача-от-интервю---commandfactory-и-historymanager)
15. [Edge Cases и капани](#15-edge-cases-и-капани)
16. [Обобщение](#16-обобщение)

---

## Основни дефиниции

> **Visitor Design Pattern** — извличаме дадена операция от класовете в отделен клас (Visitor). Всеки клас „приема" Visitor-а чрез `accept()` и му казва кой е чрез `this`. Така добавяме нови операции без да пипаме съществуващите класове.

> **Double Dispatch** — при всяко `accept()` разкриваме типа на обекта, при всяко `visitX()` разкриваме типа на операцията. Резултатът зависи от **двата** типа едновременно.

> **Command Design Pattern** — капсулираме дадено действие в обект. Позволява undo, опашка от действия и отложено изпълнение.

> **ICommand (интерфейс)** — декларира `execute()`, `undo()` и `redo()`. Всички конкретни команди го имплементират.

> **ConcreteCommand** — конкретна команда. Имплементира `execute()` и `undo()`, и пази всичко необходимо за отмяна (стари стойности, snapshot).

> **Receiver** — обектът, върху когото командата реално работи. Съдържа бизнес логиката. Например `Document`, `Vector`, `Game`. Командата извиква методи на Receiver-а.

> **Invoker** — обектът, който задейства командите. Не знае нищо за конкретните Receiver-и — работи само с `ICommand`. Пази историята (undo/redo стека). Например `TextEditor`, `CommandExecutor`.

> **Memento Design Pattern** — запазваме **целия state на обекта** преди всяко действие. При `undo` възстановяваме последния запазен state. Командите стават прости — само `execute()`, без вградена undo логика.

---

## 1. Какво е Visitor?
> **Visitor Design Pattern** — behavioral design pattern, който отделя алгоритмите от обектите върху, които те работят.

> **Проблемът** - Имаме йерархия от класове, които си комуникират по-между си и искаме да добавим нови операции върху тях, без да докосваме самите класове
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
    virtual void visitHouse(const House& h)       = 0;
    virtual void visitFactory(const Factory& f)   = 0;
    virtual void visitBank(const Bank& b)         = 0;
    virtual ~IVisitor() = default;
};

// IBuilding — само accept(), нищо повече:
class IBuilding {
public:
    virtual void accept(IVisitor& visitor) const = 0;
    virtual ~IBuilding() = default;
};

class House : public IBuilding {
public:
    void accept(IVisitor& visitor) const override {
        visitor->visitHouse(*this);
        //                   ^^^^
        //  *this е House& — конкретен тип, не IBuilding*
        //  Казваме на visitor: "аз съм House, ти решаваш какво да правиш"
    }
};

class Factory : public IBuilding {
public:
    void accept(IVisitor& visitor) const override {
        visitor->visitFactory(*this);   // this е Factory*
    }
};

class Bank : public IBuilding {
public:
    void accept(IVisitor& visitor) const override {
        visitor->visitBank(*this);   // this е Bank*
    }
};
```

```cpp
// Операция 1 — Човекът:
class PersonVisitor : public IVisitor {
public:
    void visitHouse(const House& h)       override { std::cout << "спи\n"; }
    void visitFactory(const Factory& f)   override { std::cout << "работи\n"; }
    void visitBank(const Bank& b)         override { std::cout << "тегли пари\n"; }
};

// Операция 2 — Инспекторът (нов клас, нищо не е пипнато):
class InspectorVisitor : public IVisitor {
public:
    void visitHouse(const House& h)       override { std::cout << "инспектира жилище\n"; }
    void visitFactory(const Factory& f)   override { std::cout << "инспектира фабрика\n"; }
    void visitBank(const Bank& b)         override { std::cout << "инспектира банка\n"; }
};

// Операция 3 — Пожарникарят (пак нов клас, пак нищо не е пипнато):
class FiremanVisitor : public IVisitor {
public:
    void visitHouse(const House& h)       override { std::cout << "проверява изходи\n"; }
    void visitFactory(const Factory& f)   override { std::cout << "проверява спринклери\n"; }
    void visitBank(const Bank& b)         override { std::cout << "проверява трезора\n"; }
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
        b->accept(person);

    std::cout << "\n=== Инспекторът обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(inspector);

    std::cout << "\n=== Пожарникарят обикаля ===\n";
    for (const auto& b : buildings)
        b->accept(fireman);
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


## 8. Какво е Command и какъв проблем решава?

> **Command** — капсулираме дадено **действие** в обект. Вместо да извикваш функция директно, създаваш команден обект, който носи в себе си всичко необходимо за изпълнение и отмяна на действието.

### Проблемът без Command

```cpp
// ❌ Директно извикване — Invoker знае всичко за Receiver-а:
class TextEditor {
    Document doc;
public:
    void onSaveButton()   { doc.save(); }
    void onDeleteButton() { doc.deleteLast(); }
    void onUndoButton()   { /* как? не знаем какво е последното действие */ }
};
```

Проблемите:
```
❌ Invoker (UI) е здраво вързан с Receiver (Document)
❌ Undo е невъзможен — не знаем какво е последното действие
❌ Нова операция → редактираме Invoker-а
❌ Не можем да наредим операции на опашка
```

### Решението — капсулираме действието в обект

```cpp
// ✅ Invoker не знае нищо за Document — работи само с ICommand:
class TextEditor {
    std::stack<std::unique_ptr<ICommand>> history;
public:
    void onButton(std::unique_ptr<ICommand> cmd) {
        cmd->execute();
        history.push(std::move(cmd));
    }
    void onUndoButton() {
        if (!history.empty()) {
            history.top()->undo();
            history.pop();
        }
    }
};
```

### Основните участници

```
ICommand (интерфейс)   — декларира execute(), undo(), redo()
ConcreteCommand        — имплементира конкретно действие, пази state за undo
Receiver               — обектът, върху когото действието реално се изпълнява
Invoker                — задейства командата, без да знае детайлите
```

### Защо е полезен

```
✅ Undo/Redo     — командата знае как да се отмени
✅ Опашка        — командите са обекти, могат да се наредят
✅ Логване       — всяка изпълнена команда може да се запише
✅ Разширяване   — нова операция = нов клас, нищо друго не се пипа
✅ Loose coupling — Invoker не знае нищо за Receiver-а
```

---

## 9. Прост пример — Text Editor

Класическият пример: текстов редактор с `undo`.

### Document — Receiver

```cpp
// Обектът, върху когото командите работят:
class Document {
    std::string text;
public:
    void write(const std::string& word) {
        text += word;
    }

    void erase(size_t count) {
        if (count > text.size()) count = text.size();
        text.erase(text.size() - count);
    }

    void show() const {
        std::cout << "Документ: " << text << "\n";
    }
};
```

### ICommand интерфейс

```cpp
class ICommand {
public:
    virtual void execute() = 0;
    virtual void undo()    = 0;
    virtual ~ICommand() = default;
};
```

### WriteCommand — конкретна команда

```cpp
class WriteCommand : public ICommand {
    Document&   doc;
    std::string word;   // запомняме какво сме написали — нужно за undo
public:
    WriteCommand(Document& d, const std::string& word)
        : doc(d), word(word) {}

    void execute() override { doc.write(word); }
    void undo()    override { doc.erase(word.size()); }
    // undo знае точно колко символа да изтрие — защото е запомнило word
};
```

### Invoker — управлява историята

```cpp
int main() {
    Document doc;
    std::stack<std::unique_ptr<ICommand>> history;

    auto cmd1 = std::make_unique<WriteCommand>(doc, "Hello ");
    cmd1->execute();
    history.push(std::move(cmd1));

    auto cmd2 = std::make_unique<WriteCommand>(doc, "World!");
    cmd2->execute();
    history.push(std::move(cmd2));

    doc.show();   // Документ: Hello World!

    // Undo — отменяме последното действие:
    history.top()->undo();
    history.pop();
    doc.show();   // Документ: Hello
}
```

```
Документ: Hello World!
Документ: Hello
```

---

## 10. По-сложен пример — SwapCommand и SortCommand

Пример с вектор, `undo`, `redo` и опашка от команди.

### ICommand с `redo()`

```cpp
class ICommand {
protected:
    std::vector<int>& arr;
public:
    explicit ICommand(std::vector<int>& v) : arr(v) {}

    virtual void execute() = 0;
    virtual void undo()    = 0;
    virtual void redo()    = 0;   // в повечето случаи = execute()
    virtual ~ICommand() = default;
};
```

### SwapCommand — undo е тривиален

```cpp
class SwapCommand : public ICommand {
    size_t from, to;
public:
    SwapCommand(std::vector<int>& v, size_t from, size_t to)
        : ICommand(v), from(from), to(to) {}

    void execute() override { std::swap(arr[from], arr[to]); }
    void undo()    override { std::swap(arr[to], arr[from]); }   // просто обратно
    void redo()    override { execute(); }
};
```

Swap е **обратим без snapshot** — просто разменяме обратно. Не е нужно да пазим старото state.

### SortCommand — undo изисква snapshot

```cpp
class SortCommand : public ICommand {
    std::vector<int> snapshot;   // копие ПРЕДИ сортирането

    static void sort(std::vector<int>& v) {
        // selection sort
        for (size_t i = 0; i < v.size() - 1; i++) {
            size_t minIdx = i;
            for (size_t j = i + 1; j < v.size(); j++)
                if (v[j] < v[minIdx]) minIdx = j;
            if (minIdx != i) std::swap(v[minIdx], v[i]);
        }
    }

public:
    explicit SortCommand(std::vector<int>& v) : ICommand(v) {}

    void execute() override {
        snapshot = arr;   // запомняме ПРЕДИ сортиране
        sort(arr);
    }

    void undo() override {
        arr = snapshot;   // възстановяваме от snapshot
    }

    void redo() override { execute(); }
};
```

Сортирането **не е обратимо** — не можем да пресметнем оригинала. Затова правим **snapshot преди execute()**.

### CommandExecutor — Invoker с опашка, undo и redo

```cpp
class CommandExecutor {
    std::queue<std::shared_ptr<ICommand>> toExecute;   // чакащи команди
    std::stack<std::shared_ptr<ICommand>> undoStack;   // изпълнени
    std::stack<std::shared_ptr<ICommand>> redoStack;   // отменени

public:
    void add(std::shared_ptr<ICommand> cmd) {
        toExecute.push(cmd);
        while (!redoStack.empty()) redoStack.pop();   // нова команда изчиства redo
    }

    void execute() {
        if (toExecute.empty()) return;
        auto cmd = toExecute.front();
        toExecute.pop();
        cmd->execute();
        undoStack.push(cmd);
        while (!redoStack.empty()) redoStack.pop();
    }

    void executeAll() {
        while (!toExecute.empty())
            execute();
    }

    void undo() {
        if (undoStack.empty()) return;
        auto cmd = undoStack.top();
        undoStack.pop();
        cmd->undo();
        redoStack.push(cmd);
    }

    void redo() {
        if (redoStack.empty()) return;
        auto cmd = redoStack.top();
        redoStack.pop();
        cmd->redo();
        undoStack.push(cmd);
    }
};
```

### Употреба

```cpp
int main() {
    std::vector<int> arr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    CommandExecutor ce;

    auto c1 = std::make_shared<SwapCommand>(arr, 0, 5);
    auto c2 = std::make_shared<SortCommand>(arr);
    auto c3 = std::make_shared<SwapCommand>(arr, 1, 2);

    ce.add(c1);
    ce.add(c2);
    ce.add(c3);

    // Изпълняваме c1 (swap 0↔5):
    ce.execute();
    // arr: 5 1 2 3 4 0 6 7 8 9

    // Undo на c1 (swap обратно):
    ce.undo();
    // arr: 0 1 2 3 4 5 6 7 8 9

    // Redo на c1:
    ce.redo();
    // arr: 5 1 2 3 4 0 6 7 8 9

    // Изпълняваме c2 (sort):
    ce.execute();
    // arr: 0 1 2 3 4 5 6 7 8 9

    // Undo на c2 (snapshot → преди сортиране):
    ce.undo();
    // arr: 5 1 2 3 4 0 6 7 8 9

    // Изпълняваме c3 (swap 1↔2):
    ce.execute();
    // arr: 5 2 1 3 4 0 6 7 8 9
}
```

---

## 11. Undo/Redo — двата стека

```
Начало:   undoStack = []       redoStack = []

execute(A): undoStack = [A]    redoStack = []
execute(B): undoStack = [A,B]  redoStack = []

undo():     undoStack = [A]    redoStack = [B]   ← B се отменя
undo():     undoStack = []     redoStack = [B,A] ← A се отменя

redo():     undoStack = [A]    redoStack = [B]   ← A се изпълнява отново
redo():     undoStack = [A,B]  redoStack = []    ← B се изпълнява отново

execute(C): undoStack = [A,B,C] redoStack = []   ← нова команда изчиства redo!
```

Правилото: **при нова команда redoStack се изчиства** — не можем да redo-нем нещо след нова операция.

---

## 12. Memento Design Pattern

> **Memento** — запазваме **целия state на обекта** преди всяко действие в отделен обект. При `undo` възстановяваме последния запазен state. Командите стават прости — само `execute()`, без вградена undo логика в тях.

### Кога се ползва вместо undo логика в командата

```
Command с undo() (SwapCommand, WriteCommand):
  ✅ Когато undo е тривиален — swap обратно, erase точно колкото сме insert-нали
  ✅ Командата знае точно какво да върне

Memento с clone():
  ✅ Когато операцията е сложна и undo е трудно изводим (sort, batch edit)
  ✅ Когато много команди работят върху един обект
  ✅ Командите стават прости — само execute()
  ❌ Скъпо при голям state — всяко execute() прави deep copy
```

### Структура

```
Originator  — обектът, чийто state пазим (Document, Game, Vector...)
             има clone() за deep copy на себе си
Caretaker   — пази стековете от clone()-нати обекти, извиква undo/redo
ICommand    — само execute(), без undo/redo логика
```

### Пример с `clone()`

Вместо отделен Memento клас — пазим директно deep copy на обекта чрез `clone()`:

```cpp
#include <iostream>
#include <string>
#include <stack>
#include <memory>

// ICommand — само execute(), без undo логика:
class ICommand {
public:
    virtual void execute() = 0;
    virtual ~ICommand() = default;
};

// Originator — Document с clone():
class Document {
    std::string text;
public:
    void write(const std::string& word) { text += word; }
    void show() const { std::cout << "[" << text << "]\n"; }

    // clone() прави deep copy на целия Document:
    std::unique_ptr<Document> clone() const {
        return std::make_unique<Document>(*this);
    }
};

// Конкретни команди — само execute():
class WriteCommand : public ICommand {
    Document&   doc;
    std::string word;
public:
    WriteCommand(Document& d, const std::string& w) : doc(d), word(w) {}
    void execute() override { doc.write(word); }
    // Няма undo() — Caretaker се грижи за това чрез clone()
};

// Caretaker — Invoker + undo/redo чрез clone() стекове:
class TextEditorWithMemento {
    std::unique_ptr<Document>             doc;
    std::stack<std::unique_ptr<Document>> undoStack;   // deep copies преди execute
    std::stack<std::unique_ptr<Document>> redoStack;   // deep copies преди undo

public:
    TextEditorWithMemento() : doc(std::make_unique<Document>()) {}

    void execute(std::unique_ptr<ICommand> cmd) {
        undoStack.push(doc->clone());              // deep copy ПРЕДИ execute
        while (!redoStack.empty()) redoStack.pop(); // нова команда изчиства redo
        cmd->execute();
    }

    void undo() {
        if (undoStack.empty()) {
            std::cout << "Няма какво да се отмени\n";
            return;
        }
        redoStack.push(doc->clone());              // запазваме текущия за redo
        doc = std::move(undoStack.top());          // reassign — clone-ът става текущ
        undoStack.pop();
    }

    void redo() {
        if (redoStack.empty()) {
            std::cout << "Няма какво да се повтори\n";
            return;
        }
        undoStack.push(doc->clone());
        doc = std::move(redoStack.top());
        redoStack.pop();
    }

    void show() const { doc->show(); }

    void insert(const std::string& word) {
        execute(std::make_unique<WriteCommand>(*doc, word));
    }
};

int main() {
    TextEditorWithMemento editor;

    editor.insert("Здравей");   editor.show();   // [Здравей]
    editor.insert(", свят!");   editor.show();   // [Здравей, свят!]
    editor.insert(" Край.");    editor.show();   // [Здравей, свят! Край.]

    editor.undo();              editor.show();   // [Здравей, свят!]
    editor.undo();              editor.show();   // [Здравей]

    editor.redo();              editor.show();   // [Здравей, свят!]
    editor.redo();              editor.show();   // [Здравей, свят! Край.]

    editor.insert(" Ново.");    editor.show();   // [Здравей, свят! Край. Ново.]
    editor.redo();                               // Няма какво да се повтори
}
```

```
[Здравей]
[Здравей, свят!]
[Здравей, свят! Край.]
[Здравей, свят!]
[Здравей]
[Здравей, свят!]
[Здравей, свят! Край.]
[Здравей, свят! Край. Ново.]
Няма какво да се повтори
```

### Undo/Redo потокът

```
execute("Здравей"):
  undoStack top→ [clone("")]           redoStack: []
  doc: "Здравей"

execute(", свят!"):
  undoStack top→ [clone("Здравей"), clone("")]   redoStack: []
  doc: "Здравей, свят!"

undo():
  redoStack top→ [clone("Здравей, свят!")]
  doc = move(undoStack.top())  →  doc: "Здравей"
  undoStack top→ [clone("")]

redo():
  undoStack top→ [clone("Здравей"), clone("")]
  doc = move(redoStack.top())  →  doc: "Здравей, свят!"
  redoStack: []

execute(" Ново."):          ← нова команда
  redoStack: []             ← изчистен!
  doc: "Здравей, свят! Ново."
```

### Защо `std::move` от стека

```cpp
doc = std::move(undoStack.top());   // ← move, не copy
undoStack.pop();
```

`undoStack.top()` връща reference към `unique_ptr`. `std::move` го превръща в rvalue → `doc` поема ownership. После `pop()` премахва вече празния `unique_ptr` от стека. Без `std::move` — `unique_ptr` не се копира.

### Command с undo() vs. Memento с clone()

```
┌──────────────────┬──────────────────────────┬────────────────────────────┐
│                  │ Command с undo()         │ Memento с clone()          │
├──────────────────┼──────────────────────────┼────────────────────────────┤
│ Undo логика      │ В командата              │ В Caretaker                │
│ Команди          │ execute() + undo()       │ само execute()             │
│ Памет            │ Само нужното за undo     │ Deep copy на целия обект   │
│ Нужен clone()    │ Не                       │ Да                         │
│ Кога             │ Тривиален undo (swap)    │ Сложен undo (sort, batch)  │
└──────────────────┴──────────────────────────┴────────────────────────────┘
```

---

## 13. Memento с Game - идеята

При по-сложно приложение като игра state-ът може да включва позиция, точки, карта, инвентар и много други. Всяка команда поотделно не може лесно да знае как да отмени промените върху целия state. Затова `clone()` запазва **всичко наведнъж**.

Идеята:

```
Преди всяко execute():
  undoStack.push(game->clone())   ← deep copy на ЦЕЛИЯ state

undo():
  redoStack.push(game->clone())   ← запазваме текущия за redo
  game = move(undoStack.top())    ← възстановяваме предишния
  undoStack.pop()
```

```cpp
// Game има само clone() — Caretaker се грижи за undo/redo:
class Game {
    int x, y, score;
    // ... много други полета
public:
    std::unique_ptr<Game> clone() const {
        return std::make_unique<Game>(*this);   // copy constructor копира всичко
    }
    // команди работят върху game директно:
    void moveNorth() { x--; score += 10; }
    void moveSouth() { x++; score += 10; }
};

// Caretaker — само пази стековете:
class GameManager {
    std::unique_ptr<Game>             game;
    std::stack<std::unique_ptr<Game>> undoStack;
    std::stack<std::unique_ptr<Game>> redoStack;
public:
    void execute(std::unique_ptr<ICommand> cmd) {
        undoStack.push(game->clone());              // ← запазваме ПРЕДИ
        while (!redoStack.empty()) redoStack.pop();
        cmd->execute();
    }
    void undo() {
        if (undoStack.empty()) return;
        redoStack.push(game->clone());
        game = std::move(undoStack.top());          // ← reassign указателя
        undoStack.pop();
    }
};
```

**Ключовото:** нито една команда не пише `undo()` логика. `GameManager` се грижи за всичко — командите само извикват `game.moveNorth()` и т.н.

---

## 14. Подобна задача от интервю - CommandFactory и HistoryManager
### Идеята

Всяка команда има `isModifiable()` — ако командата **променя state**, HistoryManager запазва snapshot преди изпълнение. Командите за четене (print, count) не задействат snapshot. `UndoCommand` и `RedoCommand` са просто още две команди — main loop-ът не знае нищо специално за тях.

**Дали `UndoCommand` като отделна команда е по-добра?**

```
✅ Main loop е uniform — само execute(), без if (input == "undo")
✅ UndoCommand следва същия интерфейс като всички останали
✅ Лесно добавяне на redo, history, help — всичко е команда
✅ CommandFactory е единственото място, което знае какви команди съществуват
```

---

### Концепция — NumberList

Прост списък от числа с команди: `add`, `remove`, `sort`, `print`, `count`, `undo`, `redo`.

**`NumberList.h`** — Receiver с `clone()`:

```cpp
#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <stdexcept>

class NumberList {
    std::vector<int> data;

public:
    void add(int n)    { data.push_back(n); }

    void remove(int n) {
        auto it = std::find(data.begin(), data.end(), n);
        if (it == data.end())
            throw std::invalid_argument("Числото не е намерено: " + std::to_string(n));
        data.erase(it);
    }

    void sort() { std::sort(data.begin(), data.end()); }

    void print() const {
        std::cout << "[ ";
        for (int x : data) std::cout << x << " ";
        std::cout << "]
";
    }

    size_t count() const { return data.size(); }

    // Memento — clone() за deep copy:
    std::unique_ptr<NumberList> clone() const {
        return std::make_unique<NumberList>(*this);
    }
};
```

---

**`ICommand.h`** — интерфейс с `isModifiable()`:

```cpp
#pragma once

class ICommand {
public:
    virtual void execute()          = 0;
    virtual bool isModifiable() const = 0;  // променя ли state?
    virtual ~ICommand() = default;
};
```

---

**`HistoryManager.h`** — пази state с ограничен размер:

```cpp
#pragma once
#include "NumberList.h"
#include <vector>
#include <stdexcept>

class HistoryManager {
    static constexpr size_t MAX_HISTORY = 20;

    // vector вместо stack — нужен ни е достъп до двата края за MAX_HISTORY
    std::vector<std::unique_ptr<NumberList>> undoStack;
    std::vector<std::unique_ptr<NumberList>> redoStack;

public:
    void saveState(const NumberList& current) {
        // При достигане на лимита — изтриваме най-старото (front):
        if (undoStack.size() >= MAX_HISTORY)
            undoStack.erase(undoStack.begin());

        undoStack.push_back(current.clone());

        // Нова операция изчиства redo историята:
        redoStack.clear();
    }

    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }

    // Връща предишния state, запазва текущия за redo:
    std::unique_ptr<NumberList> undo(const NumberList& current) {
        if (!canUndo())
            throw std::logic_error("Няма операции за отмяна.");

        redoStack.push_back(current.clone());

        auto prev = std::move(undoStack.back());
        undoStack.pop_back();
        return prev;
    }

    // Връща "бъдещия" state, запазва текущия за undo:
    std::unique_ptr<NumberList> redo(const NumberList& current) {
        if (!canRedo())
            throw std::logic_error("Няма операции за повтаряне.");

        undoStack.push_back(current.clone());

        auto next = std::move(redoStack.back());
        redoStack.pop_back();
        return next;
    }
};
```

---

**Конкретни команди:**

```cpp
// AddCommand.h
#pragma once
#include "ICommand.h"
#include "NumberList.h"

class AddCommand : public ICommand {
    NumberList& list;
    int         value;
public:
    static constexpr const char* NAME = "add";
    AddCommand(NumberList& l, int v) : list(l), value(v) {}
    void execute() override        { list.add(value); }
    bool isModifiable() const override { return true; }
};

// RemoveCommand.h
class RemoveCommand : public ICommand {
    NumberList& list;
    int         value;
public:
    static constexpr const char* NAME = "remove";
    RemoveCommand(NumberList& l, int v) : list(l), value(v) {}
    void execute() override        { list.remove(value); }
    bool isModifiable() const override { return true; }
};

// SortCommand.h
class SortCommand : public ICommand {
    NumberList& list;
public:
    static constexpr const char* NAME = "sort";
    explicit SortCommand(NumberList& l) : list(l) {}
    void execute() override        { list.sort(); }
    bool isModifiable() const override { return true; }
};

// PrintCommand.h — само чете, не запазва snapshot:
class PrintCommand : public ICommand {
    const NumberList& list;
public:
    static constexpr const char* NAME = "print";
    explicit PrintCommand(const NumberList& l) : list(l) {}
    void execute() override        { list.print(); }
    bool isModifiable() const override { return false; }   // ← не пипа state
};

// CountCommand.h — само чете:
class CountCommand : public ICommand {
    const NumberList& list;
public:
    static constexpr const char* NAME = "count";
    explicit CountCommand(const NumberList& l) : list(l) {}
    void execute() override {
        std::cout << "Брой: " << list.count() << "
";
    }
    bool isModifiable() const override { return false; }
};
```

---

**`UndoCommand` и `RedoCommand`** — обикновени команди, но работят с HistoryManager:

```cpp
// UndoCommand.h
#pragma once
#include "ICommand.h"
#include "HistoryManager.h"

class UndoCommand : public ICommand {
    HistoryManager&             history;
    std::unique_ptr<NumberList>& current;   // reference към текущия state
public:
    static constexpr const char* NAME = "undo";

    UndoCommand(HistoryManager& h, std::unique_ptr<NumberList>& c)
        : history(h), current(c) {}

    void execute() override {
        if (!history.canUndo())
            throw std::logic_error("Няма операции за отмяна.");
        current = history.undo(*current);   // заменяме целия state
        std::cout << "Undo успешен.";
    }

    bool isModifiable() const override { return false; }   // не запазва snapshot
};

// RedoCommand.h
class RedoCommand : public ICommand {
    HistoryManager&             history;
    std::unique_ptr<NumberList>& current;
public:
    static constexpr const char* NAME = "redo";

    RedoCommand(HistoryManager& h, std::unique_ptr<NumberList>& c)
        : history(h), current(c) {}

    void execute() override {
        if (!history.canRedo())
            throw std::logic_error("Няма операции за повтаряне.");
        current = history.redo(*current);
        std::cout << "Redo успешен.";
    }

    bool isModifiable() const override { return false; }
};
```

---

**`CommandFactory.h`** — без map, с if/else:

```cpp
#pragma once
#include "ICommand.h"
#include "NumberList.h"
#include "HistoryManager.h"
#include "AddCommand.h"
#include "RemoveCommand.h"
#include "SortCommand.h"
#include "PrintCommand.h"
#include "CountCommand.h"
#include "UndoCommand.h"
#include "RedoCommand.h"
#include <memory>
#include <string>
#include <stdexcept>

class CommandFactory {
    std::unique_ptr<NumberList>& list;
    HistoryManager&              history;

public:
    CommandFactory(std::unique_ptr<NumberList>& l, HistoryManager& h)
        : list(l), history(h) {}

    std::unique_ptr<ICommand> create(const std::string& name,
                                     const std::string& arg = "") const {
        if (name == AddCommand::NAME)
            return std::make_unique<AddCommand>(*list, std::stoi(arg));

        if (name == RemoveCommand::NAME)
            return std::make_unique<RemoveCommand>(*list, std::stoi(arg));

        if (name == SortCommand::NAME)
            return std::make_unique<SortCommand>(*list);

        if (name == PrintCommand::NAME)
            return std::make_unique<PrintCommand>(*list);

        if (name == CountCommand::NAME)
            return std::make_unique<CountCommand>(*list);

        if (name == UndoCommand::NAME)
            return std::make_unique<UndoCommand>(history, list);

        if (name == RedoCommand::NAME)
            return std::make_unique<RedoCommand>(history, list);

        throw std::invalid_argument("Непозната команда: " + name);
    }
};
```

---

**`main.cpp`** — uniform main loop:

```cpp
#include <iostream>
#include <sstream>
#include "NumberList.h"
#include "HistoryManager.h"
#include "CommandFactory.h"

int main() {
    auto list    = std::make_unique<NumberList>();
    HistoryManager history;
    CommandFactory factory(list, history);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string name, arg;
        ss >> name >> arg;

        try {
            auto cmd = factory.create(name, arg);

            if (cmd->isModifiable())
                history.saveState(*list);   // snapshot ПРЕДИ execute

            cmd->execute();

        } catch (const std::exception& e) {
            std::cout << "Грешка: " << e.what() << "
";
        }
    }
}
```

```
Вход:          Изход:
add 5          
add 3          
add 8          
print          [ 5 3 8 ]
sort           
print          [ 3 5 8 ]
undo           Undo успешен.
print          [ 5 3 8 ]
undo           Undo успешен.
print          [ 5 3 ]
redo           Redo успешен.
print          [ 5 3 8 ]
count          Брой: 3
remove 99      Грешка: Числото не е намерено: 99
```

---

### Ключовите решения

```
isModifiable() = true  → history.saveState() ПРЕДИ execute()
isModifiable() = false → директно execute() (print, count, undo, redo)

UndoCommand като отделна команда:
  ✅ Main loop е uniform — само execute()
  ✅ Не трябва if (name == "undo") в main
  ✅ UndoCommand може да хвърля изключение като всяка друга

CommandFactory без map:
  ✅ if/else → по-ясно кои команди приемат аргументи
  ✅ Няма нужда от регистрация при конструиране
  ❌ При много команди → по-дълъг factory метод
```

---

## 15. Edge Cases и капани

### Snapshot vs. Обратима операция

```
Обратима операция (swap, move):
  undo = изпълни обратното действие
  → не е нужен snapshot

Необратима операция (sort, delete, resize):
  undo = върни snapshot
  → задължително запомни state ПРЕДИ execute()
```

### Undo на undo?

В примера с опашка от команди undo не се записва в историята — само `execute()` се записва. Иначе `undo(undo(A))` = `redo(A)` — което объркващо.

### shared_ptr vs unique_ptr

```cpp
// unique_ptr — командата е в точно един стек:
std::stack<std::unique_ptr<ICommand>> history;
// При undo → трябва да се move-не в redoStack

// shared_ptr — командата може да е в undoStack и redoStack едновременно:
std::vector<std::shared_ptr<ICommand>> undoStack;
std::vector<std::shared_ptr<ICommand>> redoStack;
// По-лесно при undo/redo — просто копираме shared_ptr
```

---

## 16. Обобщение

### Visitor Pattern

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

### Command Pattern 

```
ICommand                   CommandExecutor (Invoker)
+ execute()                + add(cmd)
+ undo()                   + execute()
+ redo()                   + undo()         undoStack
     ▲                     + redo()         redoStack
     │
SwapCommand      SortCommand
execute → swap   execute → snapshot + sort
undo    → swap   undo    → arr = snapshot
redo    → swap   redo    → snapshot + sort
```

### Правила

```
VISITOR:
✅ IVisitor с visitX() за всеки тип
✅ accept(IVisitor*) в базовия клас
✅ Всеки наследник → visitor->visitX(this)
✅ vector<unique_ptr<IBase>> за колекцията
❌ При нов тип → обнови IVisitor и всички конкретни Visitor-и

COMMAND:
✅ ICommand с execute(), undo(), redo()
✅ Обратими операции → undo = обратното действие (без snapshot)
✅ Необратими операции → snapshot ПРЕДИ execute()
✅ shared_ptr при undo/redo (командата е в два стека едновременно)
✅ При нова команда → изчисти redoStack
❌ Не пази snapshot след като командата е изпълнена и undo вече не е нужен
```

> **Command:** капсулираме действие в обект. `execute()` го прави, `undo()` го отменя. Invoker-ът управлява историята, Receiver-ът съдържа бизнес логиката.
