#pragma once


#define LEFT_CHILD 1
#define RIGHT_CHILD 2

const int treeStrMaxSize = 101;

typedef char value_t[treeStrMaxSize];

struct node_t {
	value_t value = {};
	node_t* left = NULL;
	node_t* right = NULL;
	node_t* parent = NULL;
};

struct tree_t {

	node_t* root = {};                       ///<Корень
	int size = 0;                            ///<Настоящий размер дерева

#ifdef _DEBUG
	char name[30] = "";                      ///<Имя дерева
	int err = 0;                             ///<Код ошибки, содержащейся в дереве:\n
											 ///0 - нет ошибок\n
	                                         ///1 - размер отрицательный
	                                         ///2 - нет корня
	                                         ///3 - проблема с узлами
	                                         ///4 - количество узлов не соответствует размеру
#endif
};



/**
*	Проверяет дерево и записывает в него код ошибки
*
*	@param[in] tree Дерево
*
*	@return 0 - дерево некорректное; 1 - дерево корректное
*/

#ifdef _DEBUG
int TreeOk(tree_t* tree);
#endif


/**
*	Считает количество сыновей
*
*	@param[in] node Узел
*
*	@return Количество сыновей (0, 1, 2)
*/

int NodeChildsCount(node_t* node);


/**
*	Копирует элемент типа value_t
*
*	@param[out] dest Куда копируем
*	@param[in] source Откуда копируем
*
*	@return 0 - все прошло нормально
*/

int CopyValue_t(value_t dest, value_t source);


/**
*	Преобразует value_t в строку
*
*	@param[in] value Элемент типа value_t
*
*	@return Указатель на строку. Не забудьте освободить память по этому указателю!
*/

char* Value_tToStr(const value_t value);


/**
*	Сравнивает значения value_t
*
*	@param[in] value1 Первое значение
*	@param[in] value2 Второе значение
*
*	@return < 0 - первое значение меньше второго; 0 - значения равны; > 0 - первое значение больше второго
*/

int ValueCmp(value_t* value1, value_t* value2);

/**
*	Преобразует строку в value_t
*
*	@param[in] valueS Строка
*	@param[out] value Значение value_t
*
*	@return 1 - в строке неверное представление value_t; 0 - все прошло нормально
*/

int StrToValue_t(const char* valueS, value_t* value);



/**
*	Создает новое дерево. После вызова, измените значение корня\
 на нужное с помощью функции ChangeNodeValue()!
 *
 *	@param[in] name Имя дерева
 *
 *	@return Указатель на созданное дерево
*/

tree_t TreeConstructor(const char* name);


/**
*	Перезаписывает значение узла
*
*	@param node Узел
*	@param[in] value Значение
*
*	@return 0 - все прошло нормально
*/

int ChangeNodeValue(node_t* node, value_t value);


/**
*	Читает значение узла
*
*	@param[in] node Узел
*	@param[out] value Значение
*
*	@return 0 - все прошло нормально
*/

int GetNodeValue(node_t* node, value_t* value);


/**
*	Добавляет узел с указанной стороны по значению
*
*	@param tree Дерево, в котором находится узел
*	@param node Узел, к которому добавляем
*	@param[in] value Значение нового узла
*	@param[in] side Сторона, с которой добавляем (LEFT_CHILD, RIGHT_CHILD)
*	@param[out] newNode Адрес нового узла
*
*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
 2 - параметр side имел некорректное значние; 3 - на вход подалось дерево с\
  ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int AddChild(tree_t* tree, node_t* node, value_t elem, const int side, node_t** createdNode = NULL);


/**
*	Удаляет дочерний узел
*
*	@param tree Дерево, в котором находится узел
*	@param node Узел, в котором освобождаем удаляем узел
*	@param[in] side Сторона, с которой удаляем (LEFT_CHILD, RIGHT_CHILD)
*
*	@return 1 - не было дочернего узла; 2 - дочерний узел оказался поддеревом;\
 3 - параметр side имел некорректное значние; 4 - на вход подалось дерево с\
 ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int DeleteChild(tree_t* tree, node_t* node, const int side);


/**
*	Добавляет поддерево в дерево по указанному узлу.
*
*	@param tree Дерево, в которое добавляем
*	@param subtree Поддерево, которое добавляем. После присоединения деконструируется!
*	@param node Узел, к которому добавляем
*	@param[in] side Сторона узла, с которой добавляем
*
*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
 2 - параметр side имел некорректное значние; 3, 4 - на вход подалось\
 дерево или соответственно поддерево с ошибкой (только в режиме отладки);\
 0 - все прошло нормально
*/

int AddSubtree(tree_t* tree, tree_t* subtree, node_t* node, const int side);


/**
*	Удаляет поддерево
*
*	@param tree Дерево
*	@param node Узел, поддерево удаляется начиная с дочернего узла
*	@param side Сторона дочернего узла
*
*	@return 1 - не было дочернего узла; 2 - параметр side имел некорректное значние;\
 3 - на вход подалось дерево с ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int DeleteSubtree(tree_t* tree, node_t* node, const int side);


/**
*	Пролностью удаляет дерево, включая все его узлы.\
 Внимение, если данное дерево является поддеревом в\
 другом дереве, не пользуйтесь этой функцией.
*
*	@param[in] tree Дерево
*
*	@return 0 - Все прошло нормально
*/

int DeleteTree(tree_t* tree);


/**
*	Деконструктор дерева. Внимание, данная функция не удаляет узлы дерева!\
 Чтобы удалить узлы, воспользуйтесь функцией DeleteTree()!
*
*	@param tree Дерево
*
*	@return 0 - Все прошло нормально
*/

int TreeDestructor(tree_t* tree);


/**
*	Создает массив строк со значениями "последних" узлов
*
*	@param[in] tree Дерево
*	@param[out] words Массив строк (на входе должен быть NULL). Не забудьте\
 освободить память для всех элементов этого массива и для самого массива!
*	@param[out] NNodes Количество строк
*
*	@return 1 - на вход подалось некорректное дерево; 2 - words != NULL;\
 3 - проблема при реаллокации words; 4 - другая ошибка; 0 - все прошло нормально
*/

int LastNodesWords(tree_t* tree, char**& words, int* NNodes);


/**
*	Рисует дерево с помощью программы Graphviz.
*
*	@param[in] tree Дерево
*	@param[in] foutName Имя файла с картнкой (по умолчанию - "tree.png")
*	@param[in] gvFileName Имя файла с описанием узлов для Graphviz (по умолчанию - "tree.gv")
*
*	@return 1 - не удалось открыть файл для записи; 2 - на вход подалось дерево\
с ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int CreateTreeImage(tree_t* tree, const char foutName[] = "tree.png", const char gvFileName[] = "tree.gv");


/**
*	Создает изображение дерева и открывает его. Функция медленная, не используйте часто!
*
*	@param[in] tree Дерево
*
*	@return 1 - Проблема при создании изображения; 2 - проблема при открытии изображения;\
 3 - на вход подалось дерево с ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int ShowTree(tree_t* tree);



/**
*	Создает код по дереву
*
*	@param[in] tree Дерево
*	@param[out] size Длина полученного кода (без учета '\0').\
 Если значение отрицательное, то возникла следующая ошибка:\
 -1 - на вход подалось дерево с ошибкой (только в режиме отладки);\
 -2 -проблема при создании буфера; 0 - все прошло нормально
*
*	@return Указатель на буфер с кодом. В случае ошибки равен NULL.\
 Не забудьте освободить память по этому указателю!
*/

char* TreeToCode(tree_t* tree, int* size = NULL);


/**
*	Создает дерево по коду
*
*	@param[in] code Код
*	@param[in] treeName Имя дерева (по умолчанию "tree_from_code")
*	@param[out] err Код ошибки (по желанию): 1 - произвольная ошибка;\
 2 - ошибка в коде; 0 - все прошло нормально
*
*	@return Сгенерированное дерево
*/

tree_t CodeToTree(char* code, const char* treeName="tree_from_code", int* err = NULL);


/**
*	Ищет узел с указанным значением
*
*	@param[in] tree Дерево
*	@param[in] value Значение
*	@param[out] err Ошибка: 1 - произвольная ошибка; 2 - узел с таким значением не найден
*
*	@return Указатель на строку с путем до найденного узла (если нашелся) в виде '0' и '1',\
 где '0' означает левого сына, '1' - правого. В конце строки ставится '\0'.\
 Не забудьте освободить память по этому указателю! Если возникла ошибка, возвращает NULL.
*/

char* FindNodeByValue(tree_t* tree, value_t* value, node_t*& foundNode, int* err = NULL);