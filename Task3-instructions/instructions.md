# Инструкции для Coding Agent: разработка пользовательских функций NX на базе UGOPEN и Block UI Styler

## Цель

Создавать DLL-модули для Siemens NX с использованием UGOPEN (Open C API) и Block UI Styler. Код должен быть совместим с Visual Studio 2022 и NX Open API.

---

# 1. Общие требования

## Язык и окружение

- Язык разработки: C++.
- IDE: Visual Studio 2022.
- Тип проекта: Console Application.
- Тип сборки: Dynamic Link Library (\*.dll).
- Целевая платформа: Siemens NX.

## Обязательные библиотеки

Добавлять include и linker зависимости для:

- UGOPEN
- UGOPENPP

### Дополнительные зависимости линковщика

```txt
libufun.lib
libnxopencpp.lib
libugopenint.lib
libnxopenuicpp.lib
libopenpp.lib
libopenintpp.lib
libvmathpp.lib
```

---

# 2. Базовая структура NX DLL

## Точка входа

Каждый модуль должен содержать:

- `ufusr()` — основная точка входа.
- `ufusr_ask_unload()` — управление выгрузкой DLL.

## Обязательная инициализация

Перед вызовом любых UF\_\* API необходимо вызывать:

```cpp
UF_initialize();
```

После завершения:

```cpp
UF_terminate();
```

## Базовый шаблон

```cpp
#include <uf.h>

void ufusr(char* param, int* retcode, int paramLen)
{
    UF_initialize();

    // Основной код

    UF_terminate();
}

int ufusr_ask_unload(void)
{
    return (UF_UNLOAD_IMMEDIATELY);
}
```

---

# 3. Правила проектирования кода

## Общие правила

- Использовать API Siemens NX Open C.
- Не использовать сторонние CAD-библиотеки.
- Все NX-объекты хранить через `tag_t`.
- Все геометрические сущности создавать через UF\_\* API.
- После создания временных списков обязательно освобождать память.
- Избегать глобального состояния.
- Код должен быть модульным.
- Каждая операция должна быть инкапсулирована в отдельную функцию.

## Именование

### Переменные

```txt
camelCase
```

Примеры:

- `lineTag`
- `sketchTag`
- `featureList`

### Функции

```txt
PascalCase
```

Примеры:

- `CreateCylinder`
- `BuildSketch`
- `CreateExtrude`

### Константы

```txt
UPPER_CASE
```

---

# 4. Работа с геометрией

## Линии

Использовать:

```cpp
UF_CURVE_create_line
```

### Структура

```cpp
UF_CURVE_line_t
```

### Пример

```cpp
UF_CURVE_line_t lineData;

lineData.start_point[0] = 0.0;
lineData.start_point[1] = 0.0;
lineData.start_point[2] = 0.0;

lineData.end_point[0] = 10.0;
lineData.end_point[1] = 10.0;
lineData.end_point[2] = 10.0;

UF_CURVE_create_line(&lineData, &lineTag);
```

---

## Окружности и дуги

Использовать:

```cpp
UF_CURVE_create_arc_thru_3pts
```

### Правила

- `1` — создать дугу.
- `2` — создать окружность.

### Пример

```cpp
UF_CURVE_create_arc_thru_3pts(
    2,
    point1,
    point2,
    point3,
    &circleTag
);
```

---

# 5. Работа со скетчами

## Заголовочный файл

```cpp
#include <uf_sket.h>
```

## Инициализация скетча

```cpp
UF_SKET_initialize_sketch
```

## Создание скетча

```cpp
UF_SKET_create_sketch
```

### Режимы

#### Option = 1

Создание на плоскости или поверхности.

#### Option = 2

Создание через CSYS.

### Стандартная матрица

```cpp
double matrix[9] =
{
    1,0,0,
    0,1,0,
    0,0,0
};
```

## Добавление объектов в скетч

```cpp
UF_SKET_add_objects
```

### Пример

```cpp
tag_t sketchObjects[4] =
{
    line1,
    line2,
    line3,
    line4
};

UF_SKET_add_objects(sketchTag, 4, sketchObjects);
```

---

# 6. Булевы операции

## Используемый enum

```cpp
UF_FEATURE_SIGN
```

## Значения

### Создание нового тела

```cpp
UF_NULLSIGN
```

### Объединение

```cpp
UF_POSITIVE
```

### Вычитание

```cpp
UF_NEGATIVE
```

### Пересечение

```cpp
UF_INTERSECT
```

---

# 7. Создание твердых тел

## Цилиндр

Использовать:

```cpp
UF_MODL_create_cyl1
```

### Пример

```cpp
double origin[3] = {0.0, 0.0, 0.0};
double direction[3] = {0.0, 0.0, 1.0};

char height[] = "100";
char diameter[] = "50";

UF_MODL_create_cyl1(
    UF_NULLSIGN,
    origin,
    height,
    diameter,
    direction,
    &cylinderTag
);
```

---

## Блок

Использовать:

```cpp
UF_MODL_create_block1
```

### Пример

```cpp
double corner[3] = {0.0, 0.0, 0.0};

char x[] = "10";
char y[] = "20";
char z[] = "30";

char* sizes[3] = {x, y, z};

UF_MODL_create_block1(
    UF_NULLSIGN,
    corner,
    sizes,
    &blockTag
);
```

---

# 8. Вытягивание

## Создание списка объектов

```cpp
UF_MODL_create_list
```

## Добавление объектов

```cpp
UF_MODL_put_list_item
```

## Создание вытягивания

```cpp
UF_MODL_create_extruded
```

## Удаление списка

```cpp
UF_MODL_delete_list
```

## Важные требования

- После завершения работы обязательно удалять список.
- Не допускать утечек памяти.

### Шаблон

```cpp
uf_list_p_t objectList;
UF_MODL_create_list(&objectList);

UF_MODL_put_list_item(objectList, sketchTag);

UF_MODL_create_extruded(
    objectList,
    taperAngle,
    limits,
    point,
    direction,
    sign,
    &featureList
);

UF_MODL_delete_list(&featureList);
UF_MODL_delete_list(&objectList);
```

---

# 9. Вращение

Использовать:

```cpp
UF_MODL_create_revolved
```

## Параметры

- список объектов;
- углы вращения;
- точка оси;
- направление оси;
- булева операция.

### Шаблон

```cpp
UF_MODL_create_revolved(
    revolveList,
    limits,
    axisPoint,
    axisDirection,
    sign,
    &featureList
);
```

---

# 10. Требования к памяти и ресурсам

## Обязательно

Всегда освобождать:

- `uf_list_p_t`
- временные структуры
- выделенные ресурсы

## Запрещено

- Повторный вызов `UF_initialize()` после `UF_terminate()`.
- Использование неинициализированных `tag_t`.
- Хранение dangling pointers.

---

# 11. Требования к архитектуре

## Рекомендуемая структура проекта

```txt
/src
    main.cpp
    geometry.cpp
    geometry.h
    sketch.cpp
    sketch.h
    features.cpp
    features.h
    ui.cpp
    ui.h
```

---

# 12. Требования к функциям

Каждая функция должна:

- выполнять одну задачу;
- возвращать статус выполнения;
- проверять ошибки NX API;
- содержать комментарии.

## Пример

```cpp
bool CreateLine(
    const double start[3],
    const double end[3],
    tag_t& lineTag
);
```

---

# 13. Обработка ошибок

## Обязательно

Проверять коды возврата всех UF\_\* функций.

## Рекомендуемый шаблон

```cpp
int errorCode = UF_initialize();

if (errorCode != 0)
{
    // обработка ошибки
}
```

---

# 14. Block UI Styler

При создании UI:

- использовать нативный стиль NX;
- разделять UI и бизнес-логику;
- не смешивать геометрию и UI-код;
- использовать callback-функции.

## Рекомендуется

- все элементы UI выносить в отдельный модуль;
- хранить состояния UI в отдельной структуре.

---

# 15. Что должен делать Coding Agent

## Agent обязан

- генерировать полностью компилируемый код;
- использовать только NX Open C API;
- автоматически добавлять нужные include;
- создавать корректные `tag_t`;
- освобождать списки через `UF_MODL_delete_list`;
- использовать правильные булевы операции;
- генерировать понятную архитектуру проекта;
- документировать сложные места комментариями.

## Agent не должен

- использовать псевдокод;
- пропускать инициализацию NX API;
- создавать утечки памяти;
- смешивать UI и моделирование;
- использовать STL там, где ожидаются структуры NX API.

---

# 16. Предпочтительный стиль генерации

## При генерации новых функций

Coding Agent должен:

1. Сначала создать базовые сущности.
2. Затем собрать скетч.
3. После этого создавать feature.
4. В конце освобождать ресурсы.

## Последовательность операций

```txt
UF_initialize
↓
Создание геометрии
↓
Создание sketch
↓
Добавление объектов
↓
Создание feature
↓
Удаление временных списков
↓
UF_terminate
```

---

# 17. Минимальный стандарт качества

Любой генерируемый код должен:

- компилироваться без ошибок;
- не содержать memory leaks;
- использовать корректные include;
- быть пригодным для интеграции в NX;
- иметь читаемую структуру;
- быть разделён на логические модули.
