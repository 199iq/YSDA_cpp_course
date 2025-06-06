# scheme-parser

**NOTE:** Исходный код этой задачи находится в директории `scheme/`.
Для сдачи задания поменяйте содержимое файла stub.txt
и отправте эти изменения в свой удалённый репозиторий.

**Эту задачу нужно делать после [scheme-tokenizer](../scheme-tokenizer)**.

Вам нужно написать парсер для языка scheme. Парсер должен принимать
последовательность токенов и строить по ним синтаксическое дерево.

Синтаксическое дерево должно состоять из наследников класса `Object`.

Код токенизатора нужно переиспользовать из предыдущей задачи, используя `#include "tokenizer.h"`.

## Синтаксис scheme

В простом случае выражения языка состоят из одного числа или идентификатора.
В этом случае дерево разбора состоит из одной вершины - числа или идентификатора.
Для представления числа нужно сделать отдельный класс, наследник `Object`.
Для представления идентификатора тоже нужно сделать отдельный класс.

```
5
+
foo-bar
```

Выражение может быть парой. Пара обозначается круглыми скобками с точкой
между двумя элементами. Для пары тоже нужно завести отдельный класс.

Например, для выражения `(1 . 2)` должно строиться дерево из 3-х элементов.
Корнем дерева является пара с двумя потомками.

Выражение может быть списком.
Список это одна из двух сущностей:
1. пустой список. Обозначается пустыми скобочками - (). Можно думать, что это объект-константа со специальной семантикой.
2. пара, в которой второй элемент это список. В этом случае первый элемент пары называется головой (head) списка, а второй - хвостом (tail).

Из этого определения следует, что список из трех элементов можно записать так:
```
(1 . (2 . (3 . ())))

 pair => pair => pair => ()
  |       |       |
  1       2       3
```
Однако так записывать списки неудобно и разработчики языка добавили сокращенную запись:
```
(1 2 3)
```
Так записывается "правильный" список (proper list). 

В языке также есть поддержка "неправильных" списков (improper list).
В таком списке второй элемент самой вложенной пары не является пустым списком.
```
(1 . (2 . 3))
pair => pair => 3
  |       |
  1       2
```
Для таких списков тоже есть сокращенная запись:
```
(1  2 . 3)
```

В приведенных выше примерах элементы списков могут любыми корректными выражениями
```
((1 . 2) (3 4) 5)
(1 () (2 3 4) 5)
```

**От нашего задания требуется, чтобы пустой список был представлен с помощью nullptr.**


## Рекурсивный спуск

Грамматика языка `scheme` лежит в классе `LL(1)`. Это значит, что можно
написать рекурсивный алгоритм разбора, который заглядывает всего на один токен
вперёд.

В авторском решении удалось обойтись всего двумя взаимно рекурсивными функциями:
`Read` и `ReadList`.

 - `Read` читает произвольное значение.
 - `ReadList` читает список, пару или список с точкой в конце.

`Read` заглядывает на 1 токен вперёд.
  1. Если стоит число или имя, то выражение состоит из одного элемента.
  2. Если там стоит `(`, то нужно вызвать рекурсивно `ReadList`.

`ReadList` в цикле вызывает `Read`, пока не встретит `)`.

## Обработка ошибок

Ваш парсер должен проверять входной поток на корректность. В случае
если поток токенов не соответствует корректному выражению, нужно бросать
исключение `SyntaxError`.

Что такое исключения и как ими правильно пользоваться мы будем обсуждать
в будущих лекциях, пока воспринимайте это как магию. В каждом месте
в парсере, где вы понимаете, что поток токенов не соответствует синтаксису,
нужно написать `throw SyntaxError{"error message"}`. Когда код дойдёт до
`throw`, выполнение вашей функции прервётся.

```c++
if (/* случилась ошибка */) {
    throw SyntaxError{"Описание ошибки"};
}
```

## Pабота с shared_ptr

В задаче требуется, чтобы вы работали с объектами дерева через shared_ptr.

Чтобы создать новый объект и сразу обернуть его в shared_ptr в стандартной библиотеке
есть функция make_shared.

**Обратите внимание**, что make_shared принимает параметры конструктора, а не объект.

```c++
auto p = std::make_shared<Cell>(Cell{first, second}); // неправильно
auto p = std::make_shared<Cell>(first, second);       // правильно
```

## Quote

В этом задании пока что не нужно реализовывать элемент дерева, соответствующий
`QuoteToken`. При выполнении основного задания, вам скорее всего потребуется вернуться
к парсеру и добавить логику обработки токена данного типа. Рекомендуем ввести тип
узла дерева `Quote`, который имеет единственное поле типа `std::shared_ptr<Object>`, указывающее
на распаршенное выражение после символа `'`.
