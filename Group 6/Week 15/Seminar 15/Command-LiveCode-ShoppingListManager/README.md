# Задача: Shopping List Manager


---

## Клас `ShoppingList` — Receiver

Списък за пазаруване с продукти:
- `addItem(name)` — добавя продукт. При вече съществуващ — игнорира се.
- `removeItem(name)` — премахва продукт. При несъществуващ — игнорира се.
- `clear()` — изчиства всички продукти.
- `print() const` — извежда списъка.
- `clone()` — връща **независимо копие** на целия списък.

---

## Интерфейс `ICommand`

```cpp
class ICommand {
public:
    virtual void execute()            = 0;
    virtual bool isModifiable() const = 0;
    virtual ~ICommand() = default;
};
```

Реализирайте **шест** конкретни команди:

| Команда | Действие | `isModifiable()` |
|---|---|---|
| `AddItemCommand` | Добавя продукт | `true` |
| `RemoveItemCommand` | Премахва продукт | `true` |
| `ClearCommand` | Изчиства списъка | `true` |
| `PrintCommand` | Извежда списъка | `false` |
| `UndoCommand` | Отменя последното действие | `false` |
| `RedoCommand` | Повтаря отмененото действие | `false` |

`UndoCommand` и `RedoCommand` приемат `CommandHistory&` и `unique_ptr<ShoppingList>&` — при `execute()` директно заменят списъка с предишния/следващия snapshot.

---

## Клас `CommandHistory`

Отговаря **само** за пазенето на state историята:

- `saveState(list)` — запазва snapshot преди модифицираща операция.
- `undo(list)` — връща предишния snapshot, запазва текущия за redo.
- `redo(list)` — връща следващия snapshot, запазва текущия за undo.
- `canUndo() const`, `canRedo() const`.

---

## Клас `CommandFactory`

Приема **целия ред** като един string — парси го вътрешно и връща конкретна команда. По този начин Manager и Main не знаят нищо за парсинг.

- `create(line, list, history)` → `unique_ptr<ICommand>` или `nullptr` при непозната команда.

```cpp
// Вътрешно в Factory:
// "add Мляко"  → name="add",    arg="Мляко"
// "remove Хляб"→ name="remove", arg="Хляб"
// "print"      → name="print",  arg=""
// "undo"       → name="undo",   arg=""
```

---

## Клас `ShoppingManager` — Invoker

Обвива `ShoppingList`, `CommandHistory` и `CommandFactory`. **Не знае** за конкретни команди, **не знае** за undo/redo — делегира всичко на Factory.

```cpp
void ShoppingManager::run(const std::string& line) {
    auto cmd = factory.create(line, list, history);
    if (!cmd) return;

    if (cmd->isModifiable())
        history.saveState(*list);   // snapshot ПРЕДИ

    cmd->execute();
}
```

Main loop извиква само `manager.run(line)` — uniform за всичко.

---

## Пример за използване

```cpp
int main() {
    ShoppingManager manager("Седмични покупки");

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty())
            manager.run(line);
    }
}
```

```
Вход:           Изход:
add Мляко
add Хляб
add Яйца
print           [ Мляко Хляб Яйца ]
undo
print           [ Мляко Хляб ]
redo
print           [ Мляко Хляб Яйца ]
remove Хляб
clear
print           [ ]
undo
print           [ Мляко Яйца ]
undo
print           [ Мляко Хляб Яйца ]
unknown         Непозната команда: unknown
```

---

## Структурата накратко

```
main()
  └── manager.run(line)        ← само това

ShoppingManager (Invoker)
  ├── CommandFactory           ← парси line, създава команда
  ├── CommandHistory           ← пази state стековете
  └── unique_ptr<ShoppingList> ← текущото състояние

ICommand
  ├── AddItemCommand    (isModifiable = true)
  ├── RemoveItemCommand (isModifiable = true)
  ├── ClearCommand      (isModifiable = true)
  ├── PrintCommand      (isModifiable = false)
  ├── UndoCommand       (isModifiable = false) ← знае за History и List
  └── RedoCommand       (isModifiable = false) ← знае за History и List
```

---
