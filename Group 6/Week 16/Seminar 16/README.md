# SOLID Принципи, Builder и Strategy Design Patterns в C++

---

## Съдържание

**SOLID Принципи**

1. [Какво е SOLID?](#1-какво-е-solid)
2. [S — Single Responsibility Principle](#2-s--single-responsibility-principle)
3. [O — Open/Closed Principle](#3-o--openclosed-principle)
4. [L — Liskov Substitution Principle](#4-l--liskov-substitution-principle)
5. [I — Interface Segregation Principle](#5-i--interface-segregation-principle)
6. [D — Dependency Inversion Principle](#6-d--dependency-inversion-principle)
7. [Edge Cases и капани (SOLID)](#7-edge-cases-и-капани)
8. [Обобщение (SOLID)](#8-обобщение)

**Builder Design Pattern**

9. [Какво е Builder?](#9-какво-е-builder)
10. [Проблемът без Builder](#10-проблемът-без-builder)
11. [Решението — Builder Pattern](#11-решението--builder-pattern)
12. [Пример — URL Builder](#12-пример--url-builder)
13. [Пример — HTTP Request](#13-пример--http-request)

**Strategy Design Pattern**

14. [Какво е Strategy?](#14-какво-е-strategy)
15. [Проблемът без Strategy](#15-проблемът-без-strategy)
16. [Решението — Strategy Pattern](#16-решението--strategy-pattern)
17. [Пример — Search Strategy с шаблони](#17-пример--search-strategy-с-шаблони)
18. [Edge Cases и капани (Builder/Strategy)](#18-edge-cases-и-капани)
19. [Обобщение (Builder/Strategy)](#19-обобщение)

---

## Основни дефиниции

> **SOLID** — набор от пет принципа за обектно-ориентиран дизайн. Целта им е код, който е лесен за разбиране, разширяване и поддръжка.

> **Single Responsibility Principle (SRP)** — клас трябва да има **само една причина за промяна**. Всеки клас отговаря за една конкретна задача.

> **Open/Closed Principle (OCP)** — класовете трябва да са **отворени за разширяване**, но **затворени за промяна**. Нова функционалност се добавя чрез наследяване, не чрез редактиране на съществуващ код.

> **Liskov Substitution Principle (LSP)** — наследник трябва да може да **замести** базовия си клас навсякъде, без да счупи логиката на програмата.

> **Interface Segregation Principle (ISP)** — по-добре **много малки интерфейси** отколкото един голям. Класовете не трябва да имплементират методи, от които не се нуждаят.

> **Dependency Inversion Principle (DIP)** — класовете трябва да зависят от **абстракции**, не от конкретни имплементации.

> **Builder Design Pattern** — creational design pattern, при който конструирането на сложен обект се разделя от неговото представяне. Задължителните параметри се подават в конструктора на Builder-а, незадължителните — чрез именовани методи. Накрая `build()` създава финалния обект.

> **Strategy Design Pattern** — behavioral design pattern, при който група алгоритми се капсулират в отделни класове с общ интерфейс. Алгоритъмът може да се сменя по runtime без промяна на класа, който го използва.

---

# Част I: SOLID Принципи

## 1. Какво е SOLID?

SOLID е акроним от петте принципа:

```
S — Single Responsibility  →  Един клас, една отговорност
O — Open/Closed            →  Отворен за разширяване, затворен за промяна
L — Liskov Substitution    →  Наследникът замества базовия клас без изненади
I — Interface Segregation  →  Много малки интерфейси вместо един голям
D — Dependency Inversion   →  Зависи от абстракции, не от конкретни типове
```

### Защо е важно

Без SOLID кодът:
- Трудно се разбира — всеки клас прави твърде много неща
- Трудно се разширява — добавянето на нова функционалност чупи съществуващото
- Трудно се тества — зависимостите са твърдо вградени

Принципите не са задължителни правила — те са **насоки за добър дизайн**. Понякога малко нарушение е оправдано. Но познаването им помага да се взимат съзнателни решения.

---

## 2. S — Single Responsibility Principle

> Клас трябва да има **само една причина за промяна**.

### Нарушение на SRP

```cpp
// ❌ Клас с твърде много отговорности:
class Student {
    std::string name;
    double      grade;

public:
    Student(const std::string& n, double g) : name(n), grade(g) {}

    // Отговорност 1: данни за студента
    std::string getName()  const { return name; }
    double      getGrade() const { return grade; }

    // Отговорност 2: форматиране за печат
    std::string toHTML() const {
        return "<p>" + name + ": " + std::to_string(grade) + "</p>";
    }

    // Отговорност 3: записване в база данни
    void saveToDatabase() {
        // SQL логика тук...
        std::cout << "Записвам " << name << " в БД\n";
    }

    // Отговорност 4: изпращане на имейл
    void sendGradeEmail() {
        std::cout << "Изпращам имейл до " << name << "\n";
    }
};
```

Проблемите:
- При промяна на HTML формата → трябва да пипаме `Student`
- При смяна на базата данни → трябва да пипаме `Student`
- При смяна на имейл сървъра → пак `Student`

### Решение — разделяме отговорностите

```cpp
// ✅ Всеки клас има една отговорност:

// Отговорност 1: данни за студента
class Student {
    std::string name;
    double      grade;

public:
    Student(const std::string& n, double g) : name(n), grade(g) {}

    const std::string& getName()  const { return name; }
    double             getGrade() const { return grade; }
};

// Отговорност 2: форматиране
class StudentFormatter {
public:
    static std::string toHTML(const Student& s) {
        return "<p>" + s.getName() + ": " +
               std::to_string(s.getGrade()) + "</p>";
    }

    static std::string toCSV(const Student& s) {
        return s.getName() + "," + std::to_string(s.getGrade());
    }
};

// Отговорност 3: работа с база данни
class StudentRepository {
public:
    void save(const Student& s) {
        std::cout << "Записвам " << s.getName() << " в БД\n";
    }

    Student findByName(const std::string& name) {
        // Логика за търсене...
        return Student(name, 0.0);
    }
};

// Отговорност 4: известяване
class StudentNotifier {
public:
    void sendGradeEmail(const Student& s) {
        std::cout << "Изпращам имейл до " << s.getName() << "\n";
    }
};
```

```cpp
int main() {
    Student s("Иван", 5.5);

    StudentFormatter  formatter;
    StudentRepository repo;
    StudentNotifier   notifier;

    std::cout << StudentFormatter::toHTML(s) << "\n";
    repo.save(s);
    notifier.sendGradeEmail(s);
}
```

### Как да разпознаеш нарушение на SRP

Ако можеш да опишеш класа с **"и"** — нарушава SRP:

```
"Student съхранява данни И форматира И записва в БД И изпраща имейл"
                          ^^^          ^^^               ^^^
                          Всяко "И" е отделна отговорност
```

---

## 3. O — Open/Closed Principle

> Класовете трябва да са **отворени за разширяване**, но **затворени за промяна**.

Нова функционалност се добавя чрез наследяване и полиморфизъм — без да се редактира вече работещ код.

### Нарушение на OCP

```cpp
// ❌ При всяка нова фигура трябва да редактираме тази функция:
double totalArea(const std::vector<Shape*>& shapes) {
    double total = 0;
    for (Shape* s : shapes) {
        if (s->type == "circle")
            total += 3.14159 * s->radius * s->radius;
        else if (s->type == "rectangle")
            total += s->width * s->height;
        else if (s->type == "triangle")
            total += 0.5 * s->base * s->height;
        // При нова фигура → трябва да добавяме нов if тук!
    }
    return total;
}
```

### Решение — отворен за разширяване чрез полиморфизъм

```cpp
// ✅ Базовият клас е затворен за промяна.
//    Нови фигури се добавят без да пипаме totalArea():

class Shape {
public:
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    explicit Circle(double r) : radius(r) {}
    double area() const override { return 3.14159 * radius * radius; }
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h) : w(w), h(h) {}
    double area() const override { return w * h; }
};

// ✅ Тази функция НИКОГА не се променя — независимо колко фигури добавим:
double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) {
    double total = 0;
    for (const auto& s : shapes)
        total += s->area();
    return total;
}

// Добавяме нова фигура — totalArea() не се пипа!
class Triangle : public Shape {
    double base, height;
public:
    Triangle(double b, double h) : base(b), height(h) {}
    double area() const override { return 0.5 * base * height; }
};

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 8.0));

    std::cout << "Обща площ: " << totalArea(shapes) << "\n";
}
```

### Пример с Factory и OCP

```cpp
// ✅ Factory + OCP — добавяш нов тип само на едно място:
class ShapeFactory {
public:
    static std::unique_ptr<Shape> create(const std::string& type, double p1, double p2 = 0) {
        if (type == "circle")    return std::make_unique<Circle>(p1);
        if (type == "rectangle") return std::make_unique<Rectangle>(p1, p2);
        if (type == "triangle")  return std::make_unique<Triangle>(p1, p2);
        return nullptr;
    }
};
```

---

## 4. L — Liskov Substitution Principle

> Наследникът трябва да може да **замести базовия клас навсякъде**, без да счупи логиката на програмата.

Ако функция работи с `Base*`, тя трябва да работи правилно и с всеки `Derived*`.

### Нарушение на LSP

Класическият пример — квадрат и правоъгълник:

```cpp
// ❌ Квадратът е правоъгълник в математиката, но не в кода:
class Rectangle {
protected:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}

    virtual void setWidth(double w)  { width = w; }
    virtual void setHeight(double h) { height = h; }

    double area() const { return width * height; }
};

class Square : public Rectangle {
public:
    Square(double side) : Rectangle(side, side) {}

    // Квадратът трябва да пази ширина == височина:
    void setWidth(double w)  override { width = height = w; }   // ← проблем!
    void setHeight(double h) override { width = height = h; }
};

// Тази функция очаква Rectangle поведение:
void makeWide(Rectangle& r) {
    r.setHeight(5.0);
    r.setWidth(10.0);
    // Очакваме площ = 5 * 10 = 50
    assert(r.area() == 50.0);   // ❌ При Square: площ = 100! (setWidth промени и height)
}

int main() {
    Rectangle rect(3.0, 4.0);
    Square    sq(3.0);

    makeWide(rect);   // ✅ работи
    makeWide(sq);     // ❌ assertion fails — LSP нарушен!
}
```

### Решение — не наследявай ако не можеш да замениш

```cpp
// ✅ Вариант 1: Отделни класове без наследяване:
class Rectangle {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    void setWidth(double w)  { width = w; }
    void setHeight(double h) { height = h; }
    double area() const { return width * height; }
};

class Square {
    double side;
public:
    explicit Square(double s) : side(s) {}
    void setSide(double s) { side = s; }
    double area() const { return side * side; }
};

// ✅ Вариант 2: Само readonly базов клас:
class Shape {
public:
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h) : w(w), h(h) {}
    double area() const override { return w * h; }
    // Без setter-и в интерфейса → Square може да наследява без проблем
};

class Square : public Shape {
    double side;
public:
    explicit Square(double s) : side(s) {}
    double area() const override { return side * side; }
};
```

### Правила за спазване на LSP

```
✅ Наследникът може да разширява поведението на базовия клас
✅ Наследникът може да приема по-общи параметри (contravariance)
✅ Наследникът може да връща по-специфични резултати (covariance)

❌ Наследникът не трябва да нарушава инварианти на базовия клас
❌ Наследникът не трябва да хвърля изключения, които базовият не хвърля
❌ Наследникът не трябва да прави методи на базовия безсмислени
```

### Прост тест за LSP

> „Ако заменя `Base*` с `Derived*` навсякъде в кода — ще продължи ли да работи правилно?"

Ако отговорът е „не" — LSP е нарушен.

---

## 5. I — Interface Segregation Principle

> По-добре **много малки интерфейси** отколкото един голям. Класовете не трябва да имплементират методи, от които не се нуждаят.

### Нарушение на ISP

```cpp
// ❌ Огромен интерфейс — не всеки клас има нужда от всичко:
class IWorker {
public:
    virtual void work()   = 0;
    virtual void eat()    = 0;
    virtual void sleep()  = 0;
    virtual void manage() = 0;   // само мениджъри управляват
    virtual void code()   = 0;   // само разработчици кодират
    virtual ~IWorker() = default;
};

class Developer : public IWorker {
public:
    void work()   override { std::cout << "Разработвам\n"; }
    void eat()    override { std::cout << "Ям\n"; }
    void sleep()  override { std::cout << "Спя\n"; }
    void code()   override { std::cout << "Пиша код\n"; }
    void manage() override { }   // ❌ безсмислена имплементация!
};

class Manager : public IWorker {
public:
    void work()   override { std::cout << "Управлявам\n"; }
    void eat()    override { std::cout << "Ям\n"; }
    void sleep()  override { std::cout << "Спя\n"; }
    void manage() override { std::cout << "Провеждам срещи\n"; }
    void code()   override { }   // ❌ мениджърът не кодира!
};
```

### Решение — разделяме на малки интерфейси

```cpp
// ✅ Малки, конкретни интерфейси:
class IBasicNeeds {
public:
    virtual void eat()   = 0;
    virtual void sleep() = 0;
    virtual ~IBasicNeeds() = default;
};

class IWorkable {
public:
    virtual void work() = 0;
    virtual ~IWorkable() = default;
};

class ICodable {
public:
    virtual void code() = 0;
    virtual ~ICodable() = default;
};

class IManageable {
public:
    virtual void manage() = 0;
    virtual ~IManageable() = default;
};

// Всеки клас имплементира само нужните интерфейси:
class Developer : public IBasicNeeds, public IWorkable, public ICodable {
public:
    void eat()   override { std::cout << "Ям\n"; }
    void sleep() override { std::cout << "Спя\n"; }
    void work()  override { std::cout << "Разработвам\n"; }
    void code()  override { std::cout << "Пиша код\n"; }
    // Без manage() — не е нужно!
};

class Manager : public IBasicNeeds, public IWorkable, public IManageable {
public:
    void eat()    override { std::cout << "Ям\n"; }
    void sleep()  override { std::cout << "Спя\n"; }
    void work()   override { std::cout << "Управлявам\n"; }
    void manage() override { std::cout << "Провеждам срещи\n"; }
    // Без code() — не е нужно!
};

// Функция, която работи само с IWorkable:
void makeWork(IWorkable& w) {
    w.work();
}

// Функция, която работи само с ICodable:
void doCodeReview(ICodable& c) {
    c.code();
}

int main() {
    Developer dev;
    Manager   mgr;

    makeWork(dev);      // ✅
    makeWork(mgr);      // ✅
    doCodeReview(dev);  // ✅
    // doCodeReview(mgr); // ❌ не се компилира — Manager не е ICodable
}
```

### Как да разпознаеш нарушение на ISP

```
❌ Клас имплементира метод с празно тяло       → нарушение
❌ Клас хвърля "Not implemented" в метод        → нарушение
❌ Клас имплементира метод, който не има смисъл → нарушение
```

---

## 6. D — Dependency Inversion Principle

> Класовете от високо ниво не трябва да зависят от класовете от ниско ниво. И двата трябва да зависят от **абстракции**.

### Нарушение на DIP

```cpp
// ❌ OrderProcessor зависи директно от конкретен EmailSender:
class EmailSender {
public:
    void send(const std::string& message) {
        std::cout << "Изпращам имейл: " << message << "\n";
    }
};

class OrderProcessor {
    EmailSender sender;   // ← твърда зависимост от конкретен тип!

public:
    void process(const std::string& order) {
        std::cout << "Обработвам поръчка: " << order << "\n";
        sender.send("Поръчката е обработена: " + order);
        // Ако искаме SMS вместо Email → трябва да редактираме OrderProcessor!
    }
};
```

### Решение — зависи от абстракция

```cpp
// ✅ Интерфейс за изпращане на известия:
class INotifier {
public:
    virtual void send(const std::string& message) = 0;
    virtual ~INotifier() = default;
};

// Конкретни имплементации:
class EmailNotifier : public INotifier {
public:
    void send(const std::string& message) override {
        std::cout << "[Имейл] " << message << "\n";
    }
};

class SMSNotifier : public INotifier {
public:
    void send(const std::string& message) override {
        std::cout << "[SMS] " << message << "\n";
    }
};

class PushNotifier : public INotifier {
public:
    void send(const std::string& message) override {
        std::cout << "[Push] " << message << "\n";
    }
};

// ✅ OrderProcessor зависи само от INotifier:
class OrderProcessor {
    std::unique_ptr<INotifier> notifier;

public:
    // Dependency Injection — подаваме зависимостта отвън:
    explicit OrderProcessor(std::unique_ptr<INotifier> n)
        : notifier(std::move(n)) {}

    void process(const std::string& order) {
        std::cout << "Обработвам поръчка: " << order << "\n";
        notifier->send("Поръчката е обработена: " + order);
    }
};

int main() {
    // Лесна смяна на начина на известяване:
    OrderProcessor emailProcessor(std::make_unique<EmailNotifier>());
    emailProcessor.process("#1001");

    OrderProcessor smsProcessor(std::make_unique<SMSNotifier>());
    smsProcessor.process("#1002");

    OrderProcessor pushProcessor(std::make_unique<PushNotifier>());
    pushProcessor.process("#1003");
}
```

```
[Имейл] Поръчката е обработена: #1001
[SMS] Поръчката е обработена: #1002
[Push] Поръчката е обработена: #1003
```

### Dependency Injection — трите начина

```cpp
// 1. Constructor Injection (препоръчително):
class Service {
    std::unique_ptr<INotifier> notifier;
public:
    explicit Service(std::unique_ptr<INotifier> n) : notifier(std::move(n)) {}
};

// 2. Setter Injection:
class Service {
    std::unique_ptr<INotifier> notifier;
public:
    void setNotifier(std::unique_ptr<INotifier> n) { notifier = std::move(n); }
};

// 3. Method Injection:
class Service {
public:
    void process(const std::string& data, INotifier& notifier) {
        notifier.send(data);
    }
};
```

### Ключовата идея

```
Без DIP:  OrderProcessor → EmailSender     (конкретна зависимост)
С DIP:    OrderProcessor → INotifier ← EmailSender  (зависи от абстракция)
                                      ← SMSNotifier
                                      ← PushNotifier
```

---

## 7. Edge Cases и капани

### SRP — прекалено много малки класове

```cpp
// ❌ Прекалено гранулиране — безсмислено:
class StudentNameGetter { };
class StudentGradeGetter { };
class StudentNameSetter { };
// Твърде много класове за проста задача

// ✅ Балансирано — един клас за свързани данни:
class Student {
    std::string name;
    double      grade;
    // Getter-и и setter-и тук са OK — всичко е за Student данни
};
```

---

### OCP — не означава "никога не редактирай"

```cpp
// OCP не е абсолютно правило:
// При намерен бъг в базовия клас → трябва да го редактираш
// При смяна на бизнес изисквания → може да е нужна промяна

// OCP важи за разширяване с нова функционалност,
// не за поправяне на грешки
```

---

### LSP — `dynamic_cast` като сигнал за нарушение

```cpp
void process(Shape* s) {
    if (Circle* c = dynamic_cast<Circle*>(s)) {
        // специална логика за кръг
    }
    // Ако трябва dynamic_cast → вероятно LSP е нарушен
    // Решение: добави virtual метод в базовия клас
}
```

---

### ISP — не разбивай прекалено много

```cpp
// ❌ Прекалено:
class INameGettable    { virtual std::string getName() = 0; };
class IGradeGettable   { virtual double getGrade() = 0; };
class INameSettable    { virtual void setName(const std::string&) = 0; };

// ✅ Разумно:
class IReadableStudent { virtual std::string getName() = 0; virtual double getGrade() = 0; };
class IWritableStudent { virtual void setName(const std::string&) = 0; };
```

---

### DIP — не означава "всичко трябва да е интерфейс"

```cpp
// ❌ Прекалено — std::string не се нуждае от интерфейс:
class IString { virtual std::string getValue() = 0; };

// ✅ DIP важи за нестабилни зависимости:
// Файлова система, база данни, мрежа, external API → интерфейс
// std::string, std::vector, прости типове → директно
```

---

## 8. Обобщение

### Петте принципа накратко

```
S — SRP: Един клас, една отговорност
         Тест: "Мога ли да опиша класа без 'и'?"

O — OCP: Разширявай чрез наследяване, не чрез редактиране
         Тест: "Трябва ли да пипам съществуващ клас за нова функционалност?"

L — LSP: Наследникът замества базовия без изненади
         Тест: "Ако заменя Base* с Derived* — кодът пак ли работи?"

I — ISP: Много малки интерфейси > един голям
         Тест: "Има ли празни имплементации или 'not implemented'?"

D — DIP: Зависи от абстракции, не от конкретни типове
         Тест: "Мога ли да сменя имплементацията без да пипам клиента?"
```

### Как принципите си взаимодействат

```
SRP  → класовете са фокусирани
OCP  → използва полиморфизъм (наследяване + virtual)
LSP  → гарантира, че полиморфизмът работи правилно
ISP  → интерфейсите са малки и конкретни
DIP  → класовете зависят от интерфейсите (ISP), не от конкретни типове
```

### Правила

```
✅ Всеки клас има ясна, единствена отговорност
✅ Нова функционалност → нов клас/наследник, не редактиране
✅ Наследниците спазват договора на базовия клас
✅ Интерфейсите са малки и конкретни
✅ Зависимостите се подават отвън (Dependency Injection)
✅ unique_ptr за Dependency Injection при единствен собственик

❌ Клас с "и" в описанието → нарушава SRP
❌ if/else по тип вместо virtual → нарушава OCP
❌ Наследник, който прави методи безсмислени → нарушава LSP
❌ Клас с празни имплементации на методи → нарушава ISP
❌ Твърдо вградени зависимости (new ConcreteClass()) → нарушава DIP
```

> **Основен извод:** SOLID принципите не са цел сама по себе си — те са инструмент за писане на код, който е лесен за разбиране, тестване и разширяване. Спазвайте ги там, където носят реална стойност, не механично навсякъде.

---

# Част II: Builder и Strategy Design Patterns

## 9. Какво е Builder?

Представете си клас с 10 полета, от които само 2 са задължителни. Останалите 8 са опционални с различни стойности по подразбиране.

```cpp
// ❌ Конструктор с много параметри — кой е кой?
Computer c(512, 16, true, false, 0, "Intel", "Windows 11", true, false, 8);
```

Проблемите:
```
❌ Не се разбира кой параметър какво означава
❌ Лесно се объркват параметри от един тип (bool, bool, bool...)
❌ Не можеш да пропуснеш среден параметър без да подадеш стойност
❌ При нов параметър → трябва нов конструктор или промяна навсякъде
```

**Builder решава това** — задължителните параметри веднъж в Builder конструктора, незадължителните чрез ясно именовани методи.

---

## 10. Проблемът без Builder

```cpp
class Computer {
    size_t      ssd, ram;
    bool        hasGPU, hasBluetooth;
    size_t      diskId;
    std::string cpu;
    std::string os;

public:
    // ❌ Всички параметри наведнъж — объркващо:
    Computer(size_t ssd, size_t ram,
             bool hasGPU        = false,
             bool hasBluetooth  = false,
             size_t diskId      = 0,
             const std::string& cpu = "",
             const std::string& os  = "")
        : ssd(ssd), ram(ram), hasGPU(hasGPU),
          hasBluetooth(hasBluetooth), diskId(diskId),
          cpu(cpu), os(os) {}

    // Rule of Zero — string и size_t управляват паметта автоматично
};

int main() {
    // ❌ Кое е кое? Трябва да броим параметрите:
    Computer c1(512, 16, true, false, 3, "Intel i9", "Windows 11");
    Computer c2(256, 8, false, true);   // cpu и os са празни — за какво?
}
```

---

## 11. Решението — Builder Pattern

### Структура

```
Computer (продуктът — данните)
    └── Builder (вложен клас — строителят)
            ├── public: ssd, ram, hasGPU...  ← данни
            ├── public: Builder(ssd, ram)      ← задължителни
            ├── public: setGPU(bool)           ← незадължителни
            ├── public: setBluetooth(bool)     ← незадължителни
            ├── public: setCPU(string)         ← незадължителни
            ├── public: setOS(string)          ← незадължителни
            └── public: build() → Computer    ← конструира финалния обект
```

### Имплементация

```cpp
#include <iostream>
#include <string>
#include <stdexcept>

class Computer {
    // Данните — Rule of Zero (string, size_t):
    size_t      ssd, ram;
    bool        hasGPU      = false;
    bool        hasBluetooth = false;
    size_t      diskId      = 0;
    std::string cpu;
    std::string os;

    // Частен конструктор — само Builder може да го извика:
    explicit Computer(const Builder& b)
        : ssd(b.ssd), ram(b.ram),
          hasGPU(b.hasGPU), hasBluetooth(b.hasBluetooth),
          diskId(b.diskId), cpu(b.cpu), os(b.os) {}

public:
    // Rule of Zero — компилаторът генерира всичко правилно

    struct Builder {
        // private по default — само Computer може да чете чрез friend:
        size_t      ssd, ram;
        bool        hasGPU       = false;
        bool        hasBluetooth = false;
        size_t      diskId       = 0;
        std::string cpu;
        std::string os;

        Builder(size_t ssd, size_t ram) : ssd(ssd), ram(ram) {}

        // Всеки метод връща Builder& за верижно извикване:
        Builder& setGPU(bool b)               { hasGPU = b;       return *this; }
        Builder& setBluetooth(bool b)         { hasBluetooth = b; return *this; }
        Builder& setCPU(const std::string& c) { cpu = c;          return *this; }
        Builder& setOS(const std::string& o)  { os = o;           return *this; }
        Builder& setDiskId(size_t id) {
            if (id > 100)
                throw std::invalid_argument("Невалиден disk ID: " + std::to_string(id));
            diskId = id;
            return *this;
        }

        Computer build() const { return Computer(*this); }
    };

    static Builder builder(size_t ssd, size_t ram) {
        return Builder(ssd, ram);
    }

    void print() const {
        std::cout << "SSD:       " << ssd                                << " GB\n"
                  << "RAM:       " << ram                                << " GB\n"
                  << "GPU:       " << (hasGPU       ? "да" : "не")      << "\n"
                  << "Bluetooth: " << (hasBluetooth ? "да" : "не")      << "\n"
                  << "Disk ID:   " << diskId                             << "\n"
                  << "CPU:       " << (cpu.empty() ? "не е зададен" : cpu) << "\n"
                  << "OS:        " << (os.empty()  ? "не е зададен" : os)  << "\n";
    }
};

int main() {
    // ✅ Ясно, четимо, верижно:
    Computer c = Computer::builder(512, 16)
        .setGPU(true)
        .setCPU("Intel i9")
        .setOS("Linux")
        .setDiskId(3)
        .build();

    c.print();
}
```

```
SSD:       512 GB
RAM:       16 GB
GPU:       да
Bluetooth: не
Disk ID:   3
CPU:       Intel i9
OS:        Linux
```

### Ключовите правила

```
✅ Конструкторът на Computer е ЧАСТЕН — само Builder може да го извика
✅ Builder е class — полетата са private, достъпни само чрез friend class Computer
✅ Задължителните параметри са в Builder(ssd, ram) — не могат да се пропуснат
✅ Незадължителните имат default стойности в Builder — ако не се set-нат, остават default
✅ Всеки set метод връща Builder& — позволява верижно извикване
✅ build() конструира финалния обект
✅ Rule of Zero — std::string управлява паметта, не трябва ръчен деструктор
```

---

## 12. Пример — URL Builder

```cpp
#include <string>
#include <iostream>

class URL {
    std::string protocol, subdomain, domain;
    std::string path;
    std::string query;

    explicit URL(const Builder& b)
        : protocol(b.protocol), subdomain(b.subdomain),
          domain(b.domain), path(b.path), query(b.query) {}

public:
    // Rule of Zero

    struct Builder {
        std::string protocol, subdomain, domain;   // задължителни
        std::string path;                          // незадължителни
        std::string query;

        Builder(const std::string& protocol,
                const std::string& subdomain,
                const std::string& domain)
            : protocol(protocol), subdomain(subdomain), domain(domain) {}

        Builder& setPath(const std::string& p)  { path  = p; return *this; }
        Builder& setQuery(const std::string& q) { query = q; return *this; }

        URL build() const { return URL(*this); }
    };

    static Builder builder(const std::string& protocol,
                           const std::string& subdomain,
                           const std::string& domain) {
        return Builder(protocol, subdomain, domain);
    }

    std::string toString() const {
        std::string url = protocol + "://" + subdomain + "." + domain;
        if (!path.empty())  url += path;
        if (!query.empty()) url += "?" + query;
        return url;
    }
};

int main() {
    URL url = URL::builder("https", "www", "example.com")
        .setPath("/products")
        .setQuery("category=books&sort=price")
        .build();

    std::cout << url.toString() << "\n";
    // https://www.example.com/products?category=books&sort=price
}
```

---

## 13. Пример — HTTP Request

```cpp
#include <string>
#include <unordered_map>
#include <iostream>

class HttpRequest {
    std::string                                method, url;
    std::unordered_map<std::string, std::string> headers;
    std::string                                body;

    HttpRequest(const std::string& method, const std::string& url)
        : method(method), url(url) {}

public:
    // Rule of Zero — string и unordered_map управляват паметта

    struct Builder {
        std::string                                method, url;
        std::unordered_map<std::string, std::string> headers;
        std::string                                body;

        Builder(const std::string& method, const std::string& url)
            : method(method), url(url) {}

        Builder& addHeader(const std::string& key, const std::string& val) {
            headers[key] = val;
            return *this;
        }

        Builder& setBody(const std::string& data) {
            body = data;
            return *this;
        }

        HttpRequest build() const {
            HttpRequest req(method, url);
            req.headers = headers;
            req.body    = body;
            return req;
        }
    };

    void send() const {
        std::cout << "=== HTTP " << method << " → " << url << " ===\n";
        for (const auto& [key, val] : headers)
            std::cout << "  " << key << ": " << val << "\n";
        if (!body.empty())
            std::cout << "Body: " << body << "\n";
    }
};

int main() {
    HttpRequest req = HttpRequest::Builder("POST", "https://api.example.com/users")
        .addHeader("Content-Type", "application/json")
        .addHeader("Authorization", "Bearer token123")
        .setBody(R"({"name": "Ivan", "email": "ivan@fmi.bg"})")
        .build();

    req.send();
}
```

```
=== HTTP POST → https://api.example.com/users ===
  Content-Type: application/json
  Authorization: Bearer token123
Body: {"name": "Ivan", "email": "ivan@fmi.bg"}
```

---

## 14. Какво е Strategy?

Представете си онлайн магазин с различни начини за изчисляване на отстъпки — или Vector, който трябва да поддържа различни алгоритми за търсене.

**Без Strategy:**
```cpp
// ❌ При нова отстъпка → редактираме Order:
double finalPrice(const std::string& type, double total) {
    if      (type == "new")       return total * 0.90;
    else if (type == "loyal")     return total * 0.80;
    else if (type == "corporate") return total - 50.0;
    // Нов тип → пипаме тук!
}
```

**С Strategy — нов алгоритъм = нов клас, Order не се пипа:**
```cpp
order.setDiscount(std::make_unique<LoyalCustomerDiscount>());
```

---

## 15. Проблемът без Strategy

```cpp
// ❌ Алгоритъмът е вграден в класа:
class Sorter {
public:
    void sort(std::vector<int>& data, const std::string& method) {
        if      (method == "bubble")    { /* bubble sort */ }
        else if (method == "quick")     { /* quick sort  */ }
        else if (method == "merge")     { /* merge sort  */ }
        // При нов алгоритъм → редактираме Sorter!
    }
};
```

```
❌ OCP нарушен — при нов алгоритъм редактираме класа
❌ SRP нарушен — Sorter знае за всички алгоритми
❌ Трудно се тества всеки алгоритъм поотделно
```

---

## 16. Решението — Strategy Pattern

### Структура

```
IStrategy (интерфейс)
    └── execute()
          ▲
    ┌─────┴──────────────────┐
Strategy1      Strategy2      Strategy3

Context
    └── unique_ptr<IStrategy> strategy
    └── setStrategy(unique_ptr<IStrategy>)
    └── doWork() → strategy->execute()
```

### Пример — Отстъпки

```cpp
#include <iostream>
#include <memory>
#include <string>

// Интерфейс:
class IDiscountStrategy {
public:
    virtual double calculate(double total) const = 0;
    virtual std::string name() const = 0;
    virtual ~IDiscountStrategy() = default;
};

// Конкретни стратегии — Rule of Zero (няма полета с ръчна памет):
class NewCustomerDiscount : public IDiscountStrategy {
public:
    double      calculate(double total) const override { return total * 0.10; }
    std::string name()                  const override { return "Нов клиент (10%)"; }
};

class LoyalCustomerDiscount : public IDiscountStrategy {
public:
    double      calculate(double total) const override { return total * 0.20; }
    std::string name()                  const override { return "Лоялен клиент (20%)"; }
};

class CorporateDiscount : public IDiscountStrategy {
    double fixedAmount;
public:
    explicit CorporateDiscount(double amount) : fixedAmount(amount) {}
    double      calculate(double total) const override { return fixedAmount; }
    std::string name()                  const override {
        return "Корпоративна (фиксирана " + std::to_string(fixedAmount) + " лв.)";
    }
};

// Context — пази стратегията чрез unique_ptr:
class Order {
    double                            total;
    std::unique_ptr<IDiscountStrategy> discount;

public:
    Order(double total, std::unique_ptr<IDiscountStrategy> discount)
        : total(total), discount(std::move(discount)) {}

    // Смяна на стратегия по runtime:
    void setDiscount(std::unique_ptr<IDiscountStrategy> d) {
        discount = std::move(d);
    }

    void printFinalPrice() const {
        double saved = discount->calculate(total);
        std::cout << discount->name() << "\n"
                  << "  Оригинална цена: " << total       << " лв.\n"
                  << "  Отстъпка:        " << saved       << " лв.\n"
                  << "  Крайна цена:     " << total-saved << " лв.\n\n";
    }

    // Rule of Zero — unique_ptr управлява паметта
};

int main() {
    Order order(300.0, std::make_unique<NewCustomerDiscount>());
    order.printFinalPrice();

    // Смяна на стратегия по runtime:
    order.setDiscount(std::make_unique<LoyalCustomerDiscount>());
    order.printFinalPrice();

    order.setDiscount(std::make_unique<CorporateDiscount>(50.0));
    order.printFinalPrice();
}
```

```
Нов клиент (10%)
  Оригинална цена: 300 лв.
  Отстъпка:        30 лв.
  Крайна цена:     270 лв.

Лоялен клиент (20%)
  Оригинална цена: 300 лв.
  Отстъпка:        60 лв.
  Крайна цена:     240 лв.

Корпоративна (фиксирана 50 лв.)
  Оригинална цена: 300 лв.
  Отстъпка:        50 лв.
  Крайна цена:     250 лв.
```

### Защо `unique_ptr` вместо суров указател

```cpp
// ❌ Суров указател — dangling pointer ако стратегията се изтрие отвън:
class Order {
    const IDiscountStrategy* discount;   // не притежаваме!
public:
    void setDiscount(const IDiscountStrategy* d) { discount = d; }
    // Ако d се изтрие → discount сочи към невалидна памет!
};

// ✅ unique_ptr — Order притежава стратегията:
class Order {
    std::unique_ptr<IDiscountStrategy> discount;
public:
    void setDiscount(std::unique_ptr<IDiscountStrategy> d) {
        discount = std::move(d);   // Order поема ownership
    }
    // При смяна → старата стратегия се изтрива автоматично
};
```

---

## 17. Пример — Search Strategy с шаблони

Когато стратегията работи с различни типове — използваме шаблони. Тъй като Context пази `unique_ptr`, стратегията трябва да има `clone()` за да може Context-ът да се копира:

```cpp
#include <vector>
#include <memory>
#include <iostream>

// Шаблонен интерфейс:
template<class T>
class ISearchStrategy {
public:
    virtual int search(const T* arr, int size, const T& target) const = 0;
    virtual std::unique_ptr<ISearchStrategy<T>> clone() const = 0;
    virtual ~ISearchStrategy() = default;
};

// Linear Search:
template<class T>
class LinearSearchStrategy : public ISearchStrategy<T> {
public:
    int search(const T* arr, int size, const T& target) const override {
        for (int i = 0; i < size; i++)
            if (arr[i] == target) return i;
        return -1;
    }

    std::unique_ptr<ISearchStrategy<T>> clone() const override {
        return std::make_unique<LinearSearchStrategy<T>>(*this);
    }
};

// Binary Search (масивът трябва да е сортиран):
template<class T>
class BinarySearchStrategy : public ISearchStrategy<T> {
public:
    int search(const T* arr, int size, const T& target) const override {
        int left = 0, right = size - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if      (arr[mid] == target) return mid;
            else if (arr[mid] >  target) right = mid - 1;
            else                         left  = mid + 1;
        }
        return -1;
    }

    std::unique_ptr<ISearchStrategy<T>> clone() const override {
        return std::make_unique<BinarySearchStrategy<T>>(*this);
    }
};

// Context — Vector с инжектирана стратегия:
template<class T>
class SearchableVector {
    std::vector<T>                         data;
    std::unique_ptr<ISearchStrategy<T>>    strategy;

public:
    explicit SearchableVector(std::unique_ptr<ISearchStrategy<T>> s)
        : strategy(std::move(s)) {}

    void add(const T& val) { data.push_back(val); }

    // Смяна на стратегия по runtime:
    void setStrategy(std::unique_ptr<ISearchStrategy<T>> s) {
        strategy = std::move(s);
    }

    int find(const T& target) const {
        return strategy->search(data.data(), data.size(), target);
    }

    // Rule of Zero — vector и unique_ptr управляват паметта
    // НО: copy constructor трябва ръчно заради unique_ptr!
    SearchableVector(const SearchableVector& other)
        : data(other.data)
        , strategy(other.strategy->clone()) {}

    SearchableVector& operator=(const SearchableVector& other) {
        if (this != &other) {
            data     = other.data;
            strategy = other.strategy->clone();
        }
        return *this;
    }

    SearchableVector(SearchableVector&&)            = default;
    SearchableVector& operator=(SearchableVector&&) = default;
    ~SearchableVector()                             = default;
};

int main() {
    SearchableVector<int> vec(std::make_unique<LinearSearchStrategy<int>>());

    for (int i = 0; i <= 10; i++) vec.add(i);   // [0, 1, 2, ..., 10]

    std::cout << "Linear: индекс на 7 = " << vec.find(7) << "\n";

    // Смяна на стратегия — масивът е сортиран, binary работи:
    vec.setStrategy(std::make_unique<BinarySearchStrategy<int>>());
    std::cout << "Binary: индекс на 7 = " << vec.find(7) << "\n";
}
```

```
Linear: индекс на 7 = 7
Binary: индекс на 7 = 7
```

---

## 18. Edge Cases и капани

### Builder — забравен `build()`

```cpp
// ❌ build() не е извикан — b е Builder, не Computer:
auto b = Computer::builder(512, 16).setGPU(true);
// b е Builder обект!

// ✅ Задължително извикай build():
Computer c = Computer::builder(512, 16).setGPU(true).build();
```

---

### Builder — забравен `return *this`

```cpp
// ❌ Верижното извикване се чупи:
Builder& setGPU(bool b) {
    hasGPU = b;
    // липсва return *this!
}

// ✅ Задължително:
Builder& setGPU(bool b) { hasGPU = b; return *this; }
```

---

### Strategy — нулева стратегия

```cpp
// ❌ Без инициализирана стратегия — crash:
SearchableVector<int> vec;   // strategy е nullptr!
vec.find(5);                  // → segfault

// ✅ Задължително подай стратегия в конструктора:
SearchableVector<int> vec(std::make_unique<LinearSearchStrategy<int>>());
```

---

### Strategy — копиране с `unique_ptr`

```cpp
// unique_ptr не се копира → трябва ръчен copy constructor с clone():
SearchableVector(const SearchableVector& other)
    : data(other.data)
    , strategy(other.strategy->clone())   // ← задължително clone()
{}
```

---

## 19. Обобщение

### Builder Pattern

```
Кога се ползва:
  ✅ Клас с много параметри (повече от 3-4)
  ✅ Много незадължителни параметри
  ✅ Валидация при конструиране
  ✅ Четимо верижно конструиране

Структура:
  Product::builder(задължителни)
      .setOptional1(...)    ← незадължителни
      .setOptional2(...)
      .build()              ← финален обект

Rule of Zero:
  std::string и std::vector в Builder → не е нужен деструктор
```

### Strategy Pattern

```
Кога се ползва:
  ✅ Група взаимозаменяеми алгоритми
  ✅ Алгоритъмът трябва да се сменя по runtime
  ✅ Разширяване без промяна на Context (OCP)
  ✅ if/else по тип алгоритъм → стратегии

Структура:
  IStrategy → конкретни стратегии
  Context::setStrategy(unique_ptr<IStrategy>)
  Context::doWork() → strategy->execute()

Rule of Zero:
  unique_ptr<IStrategy> → деструктор не е нужен в Context
  НО: ако Context се копира → нужен clone() в IStrategy
```

### Правила

```
BUILDER:
✅ Конструкторът на Product е частен
✅ Builder е class — полетата са private + friend class Product
✅ Задължителните параметри — в public Builder конструктора
✅ set методите са public и връщат Builder& за верижност
✅ build() е public и връща готовия обект по стойност
✅ std::string вместо char* — Rule of Zero

STRATEGY:
✅ IStrategy интерфейс с чисто виртуален метод
✅ unique_ptr<IStrategy> в Context — чист ownership
✅ setStrategy(unique_ptr<IStrategy>) за смяна по runtime
✅ clone() в IStrategy ако Context може да се копира
❌ Нулева стратегия → crash при извикване
❌ Суров указател → dangling pointer при изтриване
```

> **Builder:** четимо, стъпка по стъпка конструиране — задължителните са гарантирани, незадължителните са по избор.
>
> **Strategy:** капсулираме алгоритъм в обект — разширяемо, взаимозаменяемо, без if/else.
