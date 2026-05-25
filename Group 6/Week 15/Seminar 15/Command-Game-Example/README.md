# Command Pattern

## Какво е Command Pattern?

Command е **behavioral** design pattern, чиято идея е проста: **обвиване на заявката за действие в обект**.

Вместо извикващият код да вика директно `game.moveCharNorth()`, той създава обект `NorthCommand` и извиква `cmd->exec()`. Действието е капсулирано - извикващият не знае как точно се изпълнява, а самата команда носи в себе си всичко необходимо.

### Основни участници

Роля - Отговорност:
- **Command** (интерфейс) - Декларира `exec()` и `undo()`
- **Specific Command** - Реализира конкретно действие; пази данни за отмяна
- **Receiver** - Обектът, върху когото действието реално се изпълнява
- **Invoker** - Задейства командата, без да знае детайлите
- **Client** - Създава командата и я свързва с Receiver-а

### Защо е полезен?

- **Undo/Redo** - командата пази старото състояние и може да го върне.
- **Опашки и история** - командите са обекти и могат да се съхраняват.
- **Разделяне на отговорностите** - кодът, който решава *какво* да се направи, е отделен от кода, който го *прави*.
- **Лесно разширяване** - нова команда = нов клас.

---

## Структура

```
CommandPatternShowcase/
├── Commands/
│   ├── Command.h / .cpp              ← Абстрактен базов клас (интерфейс)
│   ├── MoveCommands/
│   │   ├── MoveCommand.h / .cpp      ← Междинна абстракция за движение
│   │   ├── NorthCommand.h / .cpp
│   │   ├── SouthCommand.h / .cpp
│   │   ├── EastCommand.h / .cpp
│   │   ├── WestCommand.h / .cpp
│   │   ├── NorthEastCommand.h / .cpp
│   │   ├── NorthWestCommand.h / .cpp
│   │   ├── SouthEastCommand.h / .cpp
│   │   └── SouthWestCommand.h / .cpp
│   ├── MarkCommand/
│   │   └── MarkCommand.h / .cpp      ← Маркира текущата клетка
│   ├── Factory/
│   │   └── CommandFactory.h / .cpp   ← Превръща текстов вход в команда
│   └── CommandHistory/
│       └── CommandHistory.h / .cpp   ← Съхранява командите за undo
├── Game/
│   └── Game.h / .cpp                 ← Receiver: координира Grid и Character
├── Grid/
│   └── Grid.h / .cpp                 ← NxN мрежа с маркирани клетки
├── Character/
│   └── Character.h / .cpp            ← Геройче с позиция и символ, с който да се отбелязва на Grid-а
└── main.cpp                          ← Invoker: основен игрови цикъл
```

---

## Walkthrough - стъпка по стъпка

### 1. Абстрактната команда - `Command`

```cpp
class Command {
protected:
    Game& game;
public:
    Command(Game& game);
    virtual void exec() = 0;
    virtual void undo() = 0;
    virtual ~Command() = default;
};
```

Всяка команда получава референция към `Game` - това е нейният **Receiver**. Двата чисто виртуални метода `exec()` и `undo()` са договорът, който всяка конкретна команда трябва да изпълни.

---

### 2. Конкретни команди - движение

Движението е разделено на два слоя.

**`MoveCommand`** е обща абстракция за всички посоки. При конструиране записва текущата позиция на персонажа и изчислява новата:

```cpp
MoveCommand::MoveCommand(Game& game, int dx, int dy) : Command(game) {
    oldX = game.getCharacter().getX();
    oldY = game.getCharacter().getY();
    newX = oldX + dx;
    newY = oldY + dy;
}

void MoveCommand::exec()  { game.moveChar(newX, newY); }
void MoveCommand::undo()  { game.returnChar(oldX, oldY); }
```

Ключовото е, че `oldX` и `oldY` се записват **в момента на създаване на командата**, не при изпълнение. Именно това прави `undo()` тривиален - командата знае откъде е дошла.

**Конкретните посоки** са тривиални - само подават подходящите делти:

```cpp
// NorthCommand.cpp
NorthCommand::NorthCommand(Game& game) : MoveCommand(game, -1, 0) { }
```

Осемте посоки (`north`, `south`, `east`, `west`, `northeast`, `northwest`, `southeast`, `southwest`) са реализирани по абсолютно същия начин с различни `dx`/`dy` стойности. Новата посока изисква само нов клас от три реда.

---

### 3. Конкретна команда - маркиране

`MarkCommand` маркира клетката, на която стои персонажът, като непроходима:

```cpp
void MarkCommand::exec() {
    game.markGrid(game.getCharacter().getX(), game.getCharacter().getY());
}

void MarkCommand::undo() {
    game.unmarkGrid(game.getCharacter().getX(), game.getCharacter().getY());
}
```

Забележете разликата спрямо `MoveCommand`: тук позицията **не се записва при конструиране**.

---

### 4. CommandFactory

`CommandFactory` е **Client-ът** в терминологията на pattern-а. Тя превръща низ от потребителя в конкретен команден обект:

```cpp
std::optional<std::unique_ptr<Command>> CommandFactory::create(
    const std::string& input, Game& game)
{
    if (input == NorthCommand::CMD)
        return std::make_unique<NorthCommand>(game);
    // ... останалите посоки ...
    if (input == MarkCommand::CMD)
        return std::make_unique<MarkCommand>(game);
    if (input == UNDO_CMD) {
        game.undo();
        return std::nullopt;  // undo не се записва в историята
    }
    throw std::invalid_argument("Invalid command.");
}
```

Връщането на `std::optional` позволява `undo` да бъде обработен вътрешно, без да се създава команден обект - нямаме нужда от Undo на Undo в тази реализация.

Всяка команда носи своя стринг в `static constexpr char CMD[]`, което елиминира разпръснати "магически низове" из кода.

---

### 5. CommandHistory

```cpp
class CommandHistory {
    std::vector<std::unique_ptr<Command>> commands;
public:
    void add(std::unique_ptr<Command> cmd);
    void undoLast();
};
```

`CommandHistory` притежава командите чрез `unique_ptr` - ownership-ът е еднозначен. `undoLast()` извиква `undo()` на последната команда и я изтрива от историята:

```cpp
void CommandHistory::undoLast() {
    if (!commands.empty()) {
        commands.back()->undo();
        commands.pop_back();
    } else {
        throw std::logic_error("No commands to undo.");
    }
}
```

---

### 6. Receiver-ът - `Game`

`Game` е обектът, чрез когото командите реално променят състоянието. Той координира `Grid` и `Character` и пази историята:

```cpp
void Game::moveChar(int x, int y) {
    if (!grid.isInside(x, y)) {
        Grid::gridThrowOutside();
    }
    if (grid.isMarked(x, y)) {
        throw std::logic_error("Cannot step on marked area.");
    }

    character.move(x, y);
}
```

Бизнес правилата (проверка за граници, маркирани клетки) живеят в `Game`, не в командите. Командите само инструктират `Game` какво да направи.

---

### 7. Invoker-ът - `main`

Основният цикъл е **Invoker-ът**:

```cpp
while (true) {
    std::cin >> input;
    auto cmd = CommandFactory::create(input, game);

    if (cmd.has_value()) {
        (*cmd)->exec();
        game.addCommand(std::move(*cmd));
    }
    game.print();
}
```

`main` не знае нищо за посоки, координати или маркиране. Той само:
1. Получава команда от фабриката.
2. Изпълнява я.
3. Предава ownership на историята чрез `std::move`.

---

## Поток на едно действие

```
Потребител въвежда "north"
        │
        ▼
CommandFactory::create("north", game)
        │  създава NorthCommand(game)
        │  записва oldX, oldY, newX, newY
        ▼
(*cmd)->exec()
        │  вика game.moveChar(newX, newY)
        │  game проверява граници и маркиране
        │  character.move(newX, newY)
        ▼
game.addCommand(std::move(*cmd))
        │  CommandHistory::add(...)
        │  commands.push_back(cmd)
        ▼
game.print()  →  обновена мрежа на екрана
```

При `undo`:

```
Потребител въвежда "undo"
        │
        ▼
CommandFactory → game.undo()
        │
        ▼
CommandHistory::undoLast()
        │  commands.back()->undo()
        │  game.returnChar(oldX, oldY)
        │  commands.pop_back()  →  командата се унищожава
        ▼
game.print()  →  персонажът е на предишната позиция
```

---

## Ownership и паметта

Проектът използва `std::unique_ptr<Command>` навсякъде - командите имат точно един собственик в даден момент:

```
CommandFactory::create  →  връща unique_ptr (owns)
        │
        │  std::move(*cmd)
        ▼
CommandHistory::commands  →  vector<unique_ptr> (owns)
        │
        │  commands.pop_back()
        ▼
        ~Command()  →  автоматично унищожаване
```

Няма ръчно `delete`, няма memory leak.

---

## Разширяване

Добавянето на нова команда следва един и същ шаблон:

1. Създай клас, наследяващ `Command` (или `MoveCommand`).
2. Реализирай `exec()` и `undo()`.
3. Добави `static constexpr char CMD[]` с текстовия низ.
4. Регистрирай командата в `CommandFactory::create`.

Никой съществуващ клас не се променя - само фабриката получава нов `if` клон.
