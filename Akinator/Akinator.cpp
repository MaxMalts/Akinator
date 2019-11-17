#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include "btree_string.h"

#define ANSWER_YES 1
#define ANSWER_NO 0


/**
*	Открывает файл с данными
*
*	@param[in] fName Имя файла
*
*	@return Указатель на файл. В случае ошибки равен NULL.
*/

FILE* GetDataFile(const char* fName) {
	assert(fName != NULL);

	return  fopen(fName, "r");
}


/**
*	Определяет размер файла с учетом символа '\r'
*
*	@param[in] f Файл
*
*	@return Размер файла. Если размер отрицательный, то произошла ошибка
*/

int GetFileSize(FILE* f) {
	assert(f != NULL);
	if (fseek(f, 0, SEEK_END) != 0) {
		return -1;
	}

	int fileSize = ftell(f);

	if (fseek(f, 0, SEEK_SET) != 0) {
		return -1;
	}

	return fileSize;
}


/**
*	Читает заданное количество символов из входного потока со спецификатором формата
*
*	@param[in] formSpec Спецификатор формата
*	@param[in] NChars Количество символов (с учетом последнего '\0')
*	@param[out] buf Буфер
*
*	@return 1 - ошибка; 0 - все прошло нормально
*/

int ScanNChars(const char* formSpec, const int NChars, char* buf) {
	assert(buf != NULL);
	assert(NChars >= 0);

	char format[100] = "";
	sprintf(format, "%%%s%ds", formSpec, NChars - 1);
	if (scanf(format, buf) != 1)
		return 1;
	fseek(stdin, 0, SEEK_END);

	return 0;
}

/**
*	Создает дерево с данными из файла
*
*	@param[in] dataFile Файл
*	@param[out] err Код ошибки: 1 - неверный формат данных; 0 - все прошло нормально
*
*	@return Дерево с данными. В случае ошибки возвращается пустое дерево.
*/

tree_t GetDataTree(FILE* dataFile, int* err = NULL) {
	assert(dataFile != NULL);

	tree_t errTree = TreeConstructor("errTree");

	int fileSize = GetFileSize(dataFile);
	assert(fileSize > 0);

	char* data = (char*)calloc(fileSize + 1, sizeof(char));
	fread(data, sizeof(char), fileSize, dataFile);
	if (strchr(data, '\n') != NULL) {
		if (err != NULL) {
			*err = 1;
		}
		free(data);
		return errTree;
	}

	int convErr = 0;
	tree_t dataTree = CodeToTree(data, "dataTree", &convErr);
	free(data);
	if (convErr != 0) {
		if (err != NULL) {
			*err = 1;
		}
		TreeDestructor(&dataTree);
		return errTree;
	}

	if (err != NULL) {
		*err = 0;
	}
	return dataTree;
}


/**
*	Показывает дерево данных
*
*	@param[in] dataFName
*
*	@return 1 - ошибка при открытии файла; 2 - ошибка при чтении данных;\
 3 - ошибка припоказе дерева; 0 - все прошло нормально
*/

int ShowData(const char* dataFName = "data.bts") {
	assert(dataFName != NULL);

	FILE* dataFile = GetDataFile(dataFName);
	if (dataFile == NULL) {
		return 1;
	}

	int err = 0;
	tree_t dataTree = GetDataTree(dataFile, &err);
	fclose(dataFile);
	if (err != 0) {
		return 2;
	}

	if (ShowTree(&dataTree) != 0) {
		return 3;
	}

	return 0;
}

/**
*	Добивается, чтобы пользователь ввел "да" или "нет"
*
*	@return ANSWER_YES - "да"; ANSWER_NO - "нет"
*/

int GetYesOrNo() {

	char ans[4] = "";

	while (1) {
		scanf("%3s", ans);
		fseek(stdin, 0, SEEK_END);

		if (strcmp(ans, "да") == 0) {
			return ANSWER_YES;
		}
		else if (strcmp(ans, "нет") == 0) {
			return ANSWER_NO;
		}
		else {
			printf("Ответьте \"да\" или \"нет\".\n");
		}
	}

}


/**
*	Отгадывает слово (рекурсивная)
*
*	@param[in] curNode Текущий узел (при первичном вызове - корень дерева данных)
*	@param[out] ansNode Узел, являющийся ответом
*
*	@return 0 (false) - словов не отгадано; 1 (true) - слово отгадано
*/

int AkinatorCycle(node_t* curNode, node_t*& ansNode) {
	assert(curNode!= NULL);

	if (NodeChildsCount(curNode) == 0) {
		printf("Это - %s!\n", curNode->value);
		ansNode = curNode;

		printf("Я угадал?\n");

		int ans = GetYesOrNo();

		switch (ans) {
		case ANSWER_YES:
			return 1;
		case ANSWER_NO:
			return 0;
		default:
			assert(0);
		}
	}


	printf("%s\n", curNode->value);

	int ans = GetYesOrNo();

	switch (ans) {
	case ANSWER_YES:
		return AkinatorCycle(curNode->right, ansNode);
	case ANSWER_NO:
		return AkinatorCycle(curNode->left, ansNode);
	default:
		assert(0);
	}

	return 0;
}


/**
*	Добивается от пользователя нового слова
*
*	@param[out] newWord Новое слово
*	@param[in] wordMaxSize Максимальная длина слова
*/

void GetNewWord(char* newWord, const int wordMaxSize) {
	assert(newWord != NULL);
	assert(wordMaxSize > 0);

	while (1) {
		memset(newWord, 0, wordMaxSize);

		ScanNChars("", wordMaxSize, newWord);

		if (newWord[wordMaxSize - 2] != '\0') {
			printf("Слово слишком длинное. Введи более короткое слово (макс. %d символов):\n", \
				wordMaxSize - 1);
		}
		else break;
	}
}


/**
*	Добивается от пользователя нового вопроса
*
*	@param[out] newQuest Новый вопрос
*	@param[in] questMaxSize Максимальная длина вопроса
*/

void GetNewQuestion(char* newQuest, const int questMaxSize) {
	assert(newQuest != NULL);
	assert(questMaxSize > 0);

	while (1) {
		memset(newQuest, 0, questMaxSize);

		ScanNChars("[^\n]", questMaxSize, newQuest);

		if (newQuest[questMaxSize - 2] != '\0') {
			printf("Вопрос слишком длинный. Введи более короткий вопрос (макс. %d символов):\n", \
				questMaxSize - 1);
		}
		else break;
	}
}


/**
*	Добавляет в дерево данных новое слово
*
*	@param dataTree Дерево данных
*	@param oldAnsNode Узел со старым словом
*	@param newWord Новое слово
*	@param newQuest Новый вопрос
*	@param ansForNew Ответ на новый вопрос для нового слова (ANSWER_YES или ANSWER_NO)
*
*	@return 1 - не удалось добавить новое слово; 2 - не удалось добавить старое слово;\
 3 - не удалось изменить значение старого узла на вопрос; 4 - некорректное значение ansForNew;\
 0 - все прошло нормально
*/

int AddWords(tree_t* dataTree, node_t* oldAnsNode, char* newWord, char* newQuest, const int ansForNew) {
	assert(dataTree != NULL);
	assert(oldAnsNode != NULL);
	assert(newWord != NULL);
	assert(newQuest != NULL);

	switch (ansForNew) {
	case ANSWER_YES:
		if (AddChild(dataTree, oldAnsNode, newWord, RIGHT_CHILD) != 0) {
			return 1;
		}
		if (AddChild(dataTree, oldAnsNode, oldAnsNode->value, LEFT_CHILD) != 0) {
			return 2;
		}
		if (ChangeNodeValue(oldAnsNode, newQuest) != 0) {
			return 3;
		}
		break;
	case ANSWER_NO:
		if (AddChild(dataTree, oldAnsNode, newWord, LEFT_CHILD) != 0) {
			return 1;
		}
		if (AddChild(dataTree, oldAnsNode, oldAnsNode->value, RIGHT_CHILD) != 0) {
			return 2;
		}
		if (ChangeNodeValue(oldAnsNode, newQuest) != 0) {
			return 3;
		}
		break;
	default:
		return 4;
	}
	return 0;
}


/**
*	Записывает данные игры в файл
*
*	@param[in] dataTree Дерево с данными
*	@param[in] dataFName Имя файла для записи
*
*	@return 1 - проблема при генерации кода по дереву; 2 - проблема при открытии файла;\
 3 - проблема при записи в файл; 0 - все прошло нормально
*/

int DataToFile(tree_t* dataTree, const char* dataFName) {
	assert(dataTree != NULL);
	assert(dataFName != NULL);

	int dataSize = 0;
	char* newData = TreeToCode(dataTree, &dataSize);
	if (newData == NULL) {
		return 1;
	}

	FILE* dataFile = fopen(dataFName, "w");
	if (dataFile == NULL) {
		return 2;
	}
	if (fwrite(newData, sizeof(char), dataSize, dataFile) != dataSize) {
		return 3;
	}

	fclose(dataFile);
	free(newData);
	return 0;
}


/**
*	Добавляет новый вопрос и сохраняет (записывает) новое дерево в файл
*
*	@param dataTree Дерево данных
*	@param oldAnsNode Узел со старым словом
*	@param[in] dataFName Имя файла с данными
*
*	@return Возвращает ошибку. Если в разряде десятков 0, то произошла ошибка в функции AddWords();\
 если в разряде десятков 1, то в функции DataToFile. Соответствующие коды ошибок стоят в разряде единиц,\
 их смотри в соответствующих функциях.
*/

int AddQuestion(tree_t* dataTree, node_t* oldAnsNode, const char* dataFName) {
	assert(oldAnsNode != NULL);

	const char strMaxSize = treeStrMaxSize;  //Максимальная длина вопроса. При изменении\
											   измените соответствующие значения в строках,\
											   помеченных (*)!

	char newWord[strMaxSize] = "";
	printf("Введи слово, которое ты загадал. Слово должно быть одно. \
Если введешь несколько, то я прочитаю только первое:\n");
	GetNewWord(newWord, strMaxSize);

	char newQuest[strMaxSize] = "";
	printf("Введи вопрос, который отличает слово \"%s\" \
от твоего слова \"%s\":\n", oldAnsNode->value, newWord);
	GetNewQuestion(newQuest, strMaxSize);

	

	printf("\nВведи ответ на этот вопрос для твоего слова \"%s\" \
(для слова \"%s\" ответ должен быть противоположный):\n", newWord, oldAnsNode->value);
	printf("%s\n", newQuest);
	int ansForNew = GetYesOrNo();

	int err = AddWords(dataTree, oldAnsNode, newWord, newQuest, ansForNew);
	if (err != 0) {
		return err;
	}

	err = DataToFile(dataTree, dataFName);
	if (err != 0) {
		return 10 + err;
	}

	return 0;
}


/**
*	Считывает ввод и определяет, хочет ли пользователь увидеть дерево данных
*
*	@return 1 (true) - хочет; 0 (false) - не хочет
*/

int WantsData() {
	const char dataCommand[] = "show_data";   ///<Кодовое слово, которое нужно 
	                                          ///ввести, чтобы увидеть дерево данных

	char inp[sizeof(dataCommand) + 1] = "";

	ScanNChars("[^\n]", sizeof(dataCommand) + 1, inp);

	if (strcmp(inp, dataCommand) == 0) {
		return 1;
	}

	return 0;
}

/**
*	Главная функция игры
*
*	@return 1 - ошибка при открытии файла с данными; 2 - ошибка при чтении данных;\
 3 - ошибка при добавлении нового слова; 4 -ошибка при показе дерева данных;\
 0 - все прошло нормально
*/

int StartAkinator(const char* dataFName = "data.bts") {
	//setlocale(LC_ALL, "Russian");

	printf("Загрузка... ");

	FILE* dataFile = GetDataFile(dataFName);
	if (dataFile == NULL) {
		printf("\n\nОшибка при загрузке данных: невозможно открыть файл с данными\n");
		return 1;
	}

	int err = 0;
	tree_t dataTree = GetDataTree(dataFile, &err);
	fclose(dataFile);
	if (err != 0) {
		printf("\n\nОшибка при загрузке данных: невозможно прочитать данные\n");
		return 2;
	}
	

	printf("\n\n");
	printf("Добро пожаловать в Акинатор! Загадай слово, а я попробую его отгадать.\n");
	printf("Я буду спрашивать тебя наводящие вопросы, а ты отвечай \"да\" или \"нет\".\n");

	int repeat = ANSWER_YES;
	while (repeat==ANSWER_YES) {
		printf("Если готов, нажми enter.\n");
		if (WantsData()) {
			if (ShowData(dataFName) != 0) {
				printf("Ошибка при показе данных.\n");
				return 4;
			}
			continue;
		}

		node_t* ansNode = NULL;
		int guessed = AkinatorCycle(dataTree.root, ansNode);
		printf("\n");

		if (!guessed) {
			int TErr = AddQuestion(&dataTree, ansNode, dataFName);
			if (TErr != 0) {
				printf("Ошибка при добавлении нового слова. %d\n", TErr);
				return 3;
			}
			else {
				printf("Слово добавлено.\n\n");
			}
		}

		printf("Сыграем еще?\n");
		repeat = GetYesOrNo();
		printf("\n");
	}

	printf("Пока!");

	return 0;
}

int main() {

	int err = StartAkinator();

	//int err = ShowData();

	fseek(stdin, 0, SEEK_END);
	getchar();

	return err;
}