#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "btree_string.h"
#include "buffer.h"


#ifdef _DEBUG
#define PrintTree_OK(tree) TreeDump(&tree, __FILE__, __LINE__, __FUNCTION__, "just looking");
#define PrintTree_NOK(tree) TreeDump(&tree, __FILE__, __LINE__, __FUNCTION__, "tree has an error");
#else
#define PrintTree_OK(tree) ;
#define PrintTree_NOK(tree) ;
#endif


/*  Не для пользователя
*	Проверяет, соответствует ли аргумент одной из сторон узла
*
*	@param[in] side Сторона
*
*	@return 1 (true) - соответствует; 0 (false) - не соответствует
*/

int SideIsCorrect(const int side) {
	if (side == LEFT_CHILD || side == RIGHT_CHILD) {
		return 1;
	}
	return 0;
}


/**
*	Считает количество сыновей
*
*	@param[in] node Узел
*
*	@return Количество сыновей (0, 1, 2)
*/

int NodeChildsCount(node_t* node) {
	assert(node != NULL);

	if (node->left != NULL && node->right != NULL) {
		return 2;
	}
	else if (node->left != NULL || node->right != NULL) {
		return 1;
	}
	return 0;
}


/**
*	Копирует элемент типа value_t
*
*	@param[out] dest Куда копируем
*	@param[in] source Откуда копируем
*
*	@return 0 - все прошло нормально
*/

int CopyValue_t(value_t* dest, value_t* source) {
	assert(dest != NULL);
	assert(source != NULL);

	strcpy(*dest, *source);

	return 0;
}


/**
*	Преобразует value_t в строку
*
*	@param[in] value Элемент типа value_t
*
*	@return Указатель на строку. Не забудьте освободить память по этому указателю!
*/

char* Value_tToStr(const value_t value) {

	const int value_tMaxStrSize = strMaxSize;

	char* str = (char*)calloc(value_tMaxStrSize, sizeof(char));
	strcpy(str, value);

	return str;
}


/**
*	Преобразует строку в value_t
*
*	@param[in] valueS Строка
*	@param[out] value Значение value_t
*
*	@return 1 - в строке неверное представление value_t; 0 - все прошло нормально
*/

int StrToValue_t(const char* valueS, value_t* value) {
	assert(valueS != NULL);
	assert(value != NULL);
	
	strcpy(*value, valueS);

	return 0;
}


/*  Не для пользователя
*	Проверяет узлы ниже заданного и считает их количество
*
*	@param[in] node Узел
*	@param[out] NNodes Количество узлов. На входе значение должно быть 0!
*
*	@return 1 - ошибка есть; 0 - ошибок нет
*/

#ifdef _DEBUG
int NodesOk(node_t* node, int* NNodes) {
	assert(node != NULL);
	assert(NNodes != NULL);
	
	(*NNodes)++;
	int err = 0;
	if (node->left != NULL) {
		if (node->left->parent != node) {
			return 1;
		}
		err = NodesOk(node->left, NNodes);
		if (err != 0) {
			return err;
		}
	}
	if (node->right != NULL) {
		if (node->right->parent != node) {
			return 1;
		}
		err = NodesOk(node->right, NNodes);
		if (err != 0) {
			return err;
		}
	}
	
	return 0;
}
#endif


/**
*	Проверяет дерево и записывает в него код ошибки
*
*	@param[in] tree Дерево
*
*	@return 0 - дерево некорректное; 1 - дерево корректное
*/

#ifdef _DEBUG
int TreeOk(tree_t* tree) {
	if (tree == NULL) {
		return 0;
	}
	if (tree->size < 0) {
		tree->err = 1;
		return 0;
	}
	if (tree->root == NULL) {
		tree->err = 2;
		return 0;
	}
	int actSize = 0;
	if (NodesOk(tree->root, &actSize) != 0) {
		tree->err = 3;
	}
	if (actSize != tree->size) {
		tree->err = 4;
		return 0;
	}

	return 1;
}
#endif


/*  Не для пользователя
*	Выводит информацию о дереве. Для визуализации дерева, используйте ShowTree().
*
*	@param[in] tree Дерево
*	@param[in] file Название файла, окуда вызвали функцию
*	@param[in] line Номер строки, из которой вызвали функцию
*	@param[in] function Имя функции, из которой вызвали функцию
*	@param[in] reason Причина, по которой вызвали функцию
*/

#ifdef _DEBUG
void TreeDump(tree_t* tree, const char* file, const int line, const char* function, const char* reason) {
	assert(tree != NULL);
	assert(file != NULL);
	assert(line > 0);
	assert(function != NULL);
	assert(reason != NULL);

	char status[10] = "ok";
	if (tree->err != 0) {
		strcpy(status, "ERR");
	}

	printf("\nInfo about a tree from file: %s, function: %s, line: %d, reason: %s:\n", file, function, line, reason);
	printf("tree_t \"%s\" (%p):    (%s)\n", tree->name, tree, status);

	printf("\tsize = %d\n", tree->size);

	char* valueS = Value_tToStr(tree->root->value);
	printf("\troot (%p):\n", tree->root);
	printf("\t\tvalue: %s;\n", valueS);
	printf("\t\tleft: %p;\n", tree->root->left);
	printf("\t\tright: %p;\n", tree->root->right);
	printf("\t\tparent: %p;\n", tree->root->parent);
	free(valueS);

	printf("err = %d\n\n\n", tree->err);
}
#endif


/*  Не для пользователя
*	Рекурсивно записывает информацию обо всех узлах для программы Graphviz.\
 Информация, написанная этой функцией не является полноценным оформлением дерева.
*
*	@param[in] gvFile Файл для записи
*	@param[in] node Корневой узел
*
*	@return 0 - все прошло нормально
*/

int NodesOutput(FILE* gvFile, node_t* node) {
	assert(gvFile != NULL);
	assert(node != NULL);

	char* valueS = Value_tToStr(node->value);

#ifdef _DEBUG
	fprintf(gvFile, "\t%d [label=\"{%p|%p|%s|{%p|%p}}\"]\n", (int)node, node, node->parent, valueS, node->left, node->right);
#else
	fprintf(gvFile, "\t%d [label=\"%s\"]", (int)node, valueS);
#endif
	free(valueS);

	if (node->left != NULL) {
		NodesOutput(gvFile, node->left);
		fprintf(gvFile, "\t%d -> %d\n", (int)node, (int)node->left);
	}
	if (node->right != NULL) {
		NodesOutput(gvFile, node->right);
		fprintf(gvFile, "\t%d -> %d\n", (int)node, (int)node->right);
	}

	return 0;
}


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

int CreateTreeImage(tree_t* tree, const char foutName[], const char gvFileName[]) {
	assert(tree != NULL);
	assert(gvFileName != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 2;
	}
#endif

	FILE* gvFile = fopen(gvFileName, "w");
	if (NULL == gvFile) {
		return 1;
	}
	
#ifdef _DEBUG
	fprintf(gvFile, "digraph %s {\n", tree->name);
	fprintf(gvFile, "\tnode [shape=record]\n\n");
	fprintf(gvFile, "\tformat_node [label=\"{adress|parent|value|{left|right}}\"]\n\n");
#else
	fprintf(gvFile, "digraph {\n");
#endif

	NodesOutput(gvFile, tree->root);

	fprintf(gvFile, "}");
	fclose(gvFile);

	char sysCommand[1000] = "";
	sprintf(sysCommand, "dot -Tpng %s -o %s", gvFileName, foutName);
	system(sysCommand);

	return 0;
}


/**
*	Создает изображение дерева и открывает его. Функция медленная, не используйте часто!
*
*	@param[in] tree Дерево
*
*	@return 1 - Проблема при создании изображения; 2 - проблема при открытии изображения;\
 3 - на вход подалось дерево с ошибкой (только в режиме отладки); 0 - все прошло нормально
*/

int ShowTree(tree_t* tree) {
	assert(tree != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

	//Чтобы не использовать Sleep() из windows.h.
	//Если этого не сделать, то предыдущее изображение открывается как новое.
	int a = 0;
	for (int i = 0; i < 1000000; i++) {
		a++;
		if (a % 100 == 0) {
			i -= 99;
		}
	}

	if (CreateTreeImage(tree) == 1) {
		return 1;
	}
	if (system("tree.png") != 0) {
		return 2;
	}
	return 0;
}


/**
*	Создает новое дерево. После вызова, измените значение корня\
 на нужное с помощью функции ChangeNodeValue()!
 *
 *	@param[in] name Имя дерева
 *
 *	@return Указатель на созданное дерево
*/

tree_t TreeConstructor(const char* name) {
	assert(name != NULL);

	tree_t tree = {};
	
#ifdef _DEBUG
	strcpy(tree.name, name);
	tree.err = 0;
#endif
	
	tree.root = (node_t*)calloc(1, sizeof(node_t));
	tree.size = 1;

#ifdef _DEBUG
	if (TreeOk(&tree)) {
		PrintTree_OK(tree);
	}
	else {
		PrintTree_NOK(tree);
	}
#endif

	return tree;
}


/**
*	Перезаписывает значение узла
*
*	@param node Узел
*	@param[in] value Значение
*
*	@return 0 - все прошло нормально
*/

int ChangeNodeValue(node_t* node, value_t* value) {
	assert(node != NULL);

	CopyValue_t(&node->value, value);

	return 0;
}


/**
*	Читает значение узла
*
*	@param[in] node Узел
*	@param[out] value Значение
*
*	@return 0 - все прошло нормально
*/

int GetNodeValue(node_t* node, value_t* value) {
	assert(node != NULL);
	assert(value != NULL);

	strcpy(*value, node->value);

	return 0;
}

/*  Не для пользователя
*	Создает новый узел
*
*	@return Указатель на новый узел. Не забудьте освободить память по этому указателю!
*/

node_t* CreateNode() {

	node_t* node = (node_t*)calloc(1, sizeof(node_t));
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	return node;
}

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

int AddChild(tree_t* tree, node_t* node, value_t value, const int side, node_t** createdNode) {
	assert(tree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

	if (!SideIsCorrect(side)) {
		return 2;
	}

	node_t* newNode = CreateNode();
	strcpy(newNode->value, value);
	newNode->parent = node;

	switch (side) {
	case LEFT_CHILD:
		if (node->left != NULL) {
			return 1;
		}
		node->left = newNode;
		break;
	case RIGHT_CHILD:
		if (node->right != NULL) {
			return 1;
		}
		node->right = newNode;
		break;
	}

	tree->size++;

	*createdNode = newNode;

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	return 0;
}


///**
//*	Добавляет дочерний узел с указанной стороны по адресу
//*
//*	@param tree Дерево, в котором находится узел
//*	@param parentNode Узел, к которому добавляем
//*	@param[in] childNode Дочерний узел
//*	@param[in] side Сторона, с которой добавляем (LEFT_CHILD, RIGHT_CHILD)
//*
//*	@return 1 - у узла уже был дочерний узел с соответствующей стороны;\
// 2 - параметр side имел некорректное значние; 0 - все прошло нормально
//*/
//
//int AddNode(tree_t* tree, node_t* parentNode, node_t* childNode, const int side) {
//	assert(tree != NULL);
//	assert(parentNode != NULL);
//	assert(childNode != NULL);
//
//	if (!SideIsCorrect(side)) {
//		return 2;
//	}
//
//	switch (side) {
//	case LEFT_CHILD:
//		if (parentNode->left != NULL) {
//			return 1;
//		}
//		parentNode->left = childNode;
//		break;
//	case RIGHT_CHILD:
//		if (parentNode->right != NULL) {
//			return 1;
//		}
//		parentNode->right = childNode;
//		break;
//	}
//
//	return 0;
//}

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

int DeleteChild(tree_t* tree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 4;
	}
#endif

	if (!SideIsCorrect(side)) {
		return 3;
	}

	switch (side) {
	case LEFT_CHILD:
		if (node->left == NULL) {
			return 1;
		}
		else if (NodeChildsCount(node->left) > 0) {
			return 2;
		}

		free(node->left);
		node->left = NULL;
		break;

	case RIGHT_CHILD:
		if (node->right == NULL) {
			return 1;
		}
		else if (NodeChildsCount(node->right) > 0) {
			return 2;
		}

		free(node->right);
		node->right = NULL;
		break;
	}

	tree->size--;

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	return 0;
}


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

int AddSubtree(tree_t* tree, tree_t* subtree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(subtree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif
#ifdef _DEBUG
	if (!TreeOk(subtree)) {
		PrintTree_NOK(*subtree);
		return 4;
	}
#endif

	if (!SideIsCorrect(side)) {
		return 2;
	}

	switch (side) {
	case LEFT_CHILD:
		if (node->left != NULL) {
			return 1;
		}
		node->left = subtree->root;
		break;

	case RIGHT_CHILD:
		if (node->right != NULL) {
			return 1;
		}
		node->right = subtree->root;
		break;
	}
	subtree->root->parent = node;
	tree->size += subtree->size;

	TreeDestructor(subtree);

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	return 0;
}


/*  Не для пользователя
*	Удаляет все дочерние узлы и сам узел
*
*	@param Узел
*	@param[in] count Количество уже удаленных узлов\
 (при первичном вызове опустить, переменная только для рекурсивного вызова)
*
*	@return Количество удаленных узлов
*/

int DeleteNodes(node_t* node, int count = 0) {
	assert(node != NULL);
	assert(count >= 0);

	if (node->left != NULL) {
		count = DeleteNodes(node->left, count);
	}
	if (node->right != NULL) {
		count = DeleteNodes(node->right, count);
	}
	free(node);

	return count + 1;
}


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

int DeleteSubtree(tree_t* tree, node_t* node, const int side) {
	assert(tree != NULL);
	assert(node != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 3;
	}
#endif

	if (!SideIsCorrect(side)) {
		return 2;
	}

	int deletedCount = 0;
	switch (side) {
	case LEFT_CHILD:
		if (node->left == NULL) {
			return 1;
		}

		deletedCount = DeleteNodes(node->left);
		node->left = NULL;
		break;

	case RIGHT_CHILD:
		if (node->right == NULL) {
			return 1;
		}

		deletedCount = DeleteNodes(node->right);
		node->right = NULL;
		break;
	}

	tree->size -= deletedCount;
	assert(tree->size >= 0);

#ifdef _DEBUG
	if (TreeOk(tree)) {
		PrintTree_OK(*tree);
	}
	else {
		PrintTree_NOK(*tree);
	}
#endif

	return 0;
}


/**
*	Пролностью удаляет дерево, включая все его узлы.\
 Внимение, если данное дерево является поддеревом в\
 другом дереве, не пользуйтесь этой функцией.
*
*	@param[in] tree Дерево
*
*	@return 0 - Все прошло нормально
*/

int DeleteTree(tree_t* tree) {
	assert(tree != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
	}
#endif

	DeleteNodes(tree->root);
	TreeDestructor(tree);

	return 0;
}


/**
*	Деконструктор дерева. Внимание, данная функция не удаляет узлы дерева!\
 Чтобы удалить узлы, воспользуйтесь функцией DeleteTree()!
*
*	@param tree Дерево
*
*	@return 0 - Все прошло нормально
*/

int TreeDestructor(tree_t* tree) {
	assert(tree != NULL);

	tree->root = NULL;
	tree->size = 0;

	return 0;
}


/*  Не для пользователя
*	Создает код по узлу
*
*	@param[in] node Узел
*	@param[out] codeBuf Буфер с кодом
*
*	@return 0 - все прошло нормально
*/

int NodesToCode(node_t* node, buf_t* codeBuf) {
	assert(node != NULL);
	assert(codeBuf != NULL);

	char* valueS = Value_tToStr(node->value);
	
	if (NodeChildsCount(node) > 0) {
		Bufcat(codeBuf, valueS);
		Bufcat(codeBuf, "{");
		if (node->left != NULL) {
			NodesToCode(node->left, codeBuf);
		}
		else {
			Bufcat(codeBuf, "@");
		}
		Bufcat(codeBuf, ",");
		if (node->right != NULL) {
			NodesToCode(node->right, codeBuf);
		}
		else {
			Bufcat(codeBuf, "@");
		}
		Bufcat(codeBuf, "}");
	}
	else {
		Bufcat(codeBuf, "{");
		Bufcat(codeBuf, valueS);
		Bufcat(codeBuf, "}");
	}

	return 0;
}


/**
*	Создает код по дереву
*
*	@param[in] tree Дерево
*	@param[out] str Буфер
*
*	@return 1 - на вход подалось дерево с ошибкой (только в режиме отладки);\
 0 - все прошло нормально
*/

int TreeToCode(tree_t* tree, char* code) {
	assert(tree != NULL);
	assert(code != NULL);

#ifdef _DEBUG
	if (!TreeOk(tree)) {
		PrintTree_NOK(*tree);
		return 1;
	}
#endif

	buf_t codeBuf = {};
	codeBuf.str = code;
	
	Bufcat(&codeBuf, "{");
	NodesToCode(tree->root, &codeBuf);
	Bufcat(&codeBuf, "}");

	return 0;
}


/*  Не для пользователя
*	Рекурсивно создает узлы по коду
*
*	@param[in] buf Буфер с кодом
*	@param[out] node Текущий узел. Внимание, при первичном вызове\
 должен быть NULL, сюда запишется корневой узел!
*	@param[out] size
*
*	@return 1 - ошибка в коде; 0 - все прошло нормально
*/

int CodeToNodes(buf_t* buf, node_t*& node, int* size) {
	assert(buf != NULL);
	assert(size != NULL);

	char curCh = Bgetc(buf);
	if (curCh == '{') {
		if (Bgetc(buf) != '{') {
			Bseek(buf, -1, BSEEK_CUR);
		}

		if (Bgetc(buf) == '@') {
			node->left = NULL;

			int err = 0;
			err = CodeToNodes(buf, node, size);
			if (err != 0) {
				return err;
			}
		}
		else {
			Bseek(buf, -1, BSEEK_CUR);

			char valueS[100] = "";
			ReadToChar(valueS, buf, "{,}");

			node_t* newNode = CreateNode();

			value_t value = {};
			StrToValue_t(valueS, &value);
			ChangeNodeValue(newNode, &value);
			if (node != NULL) {
				node->left = newNode;
			}
			newNode->parent = node;
			if (node == NULL) {
				node = newNode;
			}
			(*size)++;

			int err = 0;
			err = CodeToNodes(buf, newNode, size);
			if (err != 0) {
				return err;
			}
		}
	}
	else if (curCh == ',') {
		if (Bgetc(buf) != '{') {
			Bseek(buf, -1, BSEEK_CUR);
		}

		if (Bgetc(buf) == '@') {
			node->right = NULL;

			int err = 0;
			err = CodeToNodes(buf, node, size);
			if (err != 0) {
				return err;
			}
		}
		else {
			Bseek(buf, -1, BSEEK_CUR);

			char valueS[100] = "";
			ReadToChar(valueS, buf, "{,}");

			node_t* newNode = CreateNode();

			value_t value = {};
			StrToValue_t(valueS, &value);
			ChangeNodeValue(newNode, &value);
			node->right = newNode;
			newNode->parent = node;
			(*size)++;

			int err = 0;
			err = CodeToNodes(buf, newNode, size);
			if (err != 0) {
				return err;
			}
		}
	}
	else if (curCh == '}') {

		if (node->parent != NULL) {
			int err = 0;
			err = CodeToNodes(buf, node->parent, size);
			if (err != 0) {
				return err;
			}
		}
	}
	else {
		return 1;
	}

	return 0;
}


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

tree_t CodeToTree(char* code, const char* treeName, int* err) {
	assert(code != NULL);

	tree_t tree = TreeConstructor(treeName);

#ifdef _DEBUG
	if (TreeOk(&tree)) {
		PrintTree_OK(tree);
	}
	else {
		PrintTree_NOK(tree);
	}
#endif

	int constructErr = 0;
	buf_t codeBuf = BufConstructor('r', code, strlen(code), &constructErr);
	if (constructErr != 0) {
		*err = 1;
		return tree;
	}

	free(tree.root);
	tree.root = NULL;
	tree.size = 0;
	int retErr = CodeToNodes(&codeBuf, tree.root, &tree.size);
	
	if (err != NULL) {
		*err = 2;
	}

#ifdef _DEBUG
	if (!TreeOk(&tree)) {
		*err = 2;
	}
#endif

	return tree;
}