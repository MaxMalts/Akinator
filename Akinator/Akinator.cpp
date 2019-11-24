#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <math.h>
#include "btree_string.h"
#include "buffer.h"

#define ANSWER_YES 1
#define ANSWER_NO 0

#define COMMAND_DATA 1
#define COMMAND_WORDS 2

#define COMMAND_DEFINITION 3
#define COMMAND_COMPARE 4

int IsUpperRus(char ch) {
	if (ch >= 'А' && ch <= 'Я') {
		return 1;
	}

	return 0;
}

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
*	@param[out] buf Буфер
*	@param[in] formSpec Спецификатор формата
*	@param[in] NChars Количество символов. Внимание, в конце может дописаться еще один '\0'!
*
*	@return 1 - ошибка; 0 - все прошло нормально
*/

int ScanNChars(char* buf, const char* formSpec, const int NChars) {
	assert(buf != NULL);
	assert(NChars >= 0);

	char format[100] = "";
	sprintf(format, "%%%s%ds", formSpec, NChars);
	int err = scanf(format, buf);
	if (err != 1 && err != 0) {
		return 1;
	}
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
*	@param[in] dataTree Дерево данных
*
*	@return 1 - не удалось показать дерево; 0 - все прошло нормально
*/

int ShowData(tree_t* dataTree) {
	assert(dataTree != NULL);

	if (ShowTree(dataTree) != 0) {
		return 1;
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


	printf("%s?\n", curNode->value);

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
*	Добивается от пользователя слова
*
*	@param[out] newWord Слово
*	@param[in] wordMaxSize Максимальная длина слова
*/

void InputWord(char* newWord, const int wordMaxSize) {
	assert(newWord != NULL);
	assert(wordMaxSize > 0);

	while (1) {
		memset(newWord, 0, wordMaxSize);

		ScanNChars(newWord, "", wordMaxSize - 1);

		if (newWord[wordMaxSize - 2] != '\0') {
			printf("Слово слишком длинное. Введи более короткое слово (макс. %d символов):\n", \
				wordMaxSize - 1);
		}
		else break;
	}
}


/**
*	Добивается от пользователя вопроса
*
*	@param[out] newQuest Вопрос
*	@param[in] questMaxSize Максимальная длина вопроса
*/

void InputQuestion(char* newQuest, const int questMaxSize) {
	assert(newQuest != NULL);
	assert(questMaxSize > 0);

	while (1) {
		memset(newQuest, 0, questMaxSize);

		ScanNChars(newQuest, "[^\n]", questMaxSize - 1);

		if (newQuest[questMaxSize - 2] != '\0') {
			printf("Вопрос слишком длинный. Введи более короткий вопрос (макс. %d символов):\n", \
				questMaxSize - 1);
			continue;
		}

		if (!IsUpperRus(newQuest[0])) {
			printf("Вопрос должен начинаться с большой буквы.\n");
			continue;
		}

		if (strstr(newQuest, " не ") || strstr(newQuest, "Не ")) {
			printf("Нельзя вводить вопрос со словом \"не\". Введи другой вопрос:\n");
			continue;
		}
		
		int questLen = strlen(newQuest);
		if (newQuest[questLen - 1] == '?') {
			newQuest[questLen - 1] = '\0';
		}

		break;
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

	const char strMaxSize = treeStrMaxSize;  //Максимальная длина вопроса.

	char newWord[strMaxSize] = "";
	printf("Хм... Возможно я не знаю это слово. Расскажи мне о нем, введи слово, которое ты загадал.\n");
	printf("Слово должно быть одно. Если введешь несколько, то я прочитаю только первое:\n");
	InputWord(newWord, strMaxSize);

	char newQuest[strMaxSize] = "";
	printf("Введи вопрос, который отличает слово \"%s\" "
	       "от твоего слова \"%s\":\n", oldAnsNode->value, newWord);
	InputQuestion(newQuest, strMaxSize);

	

	printf("\nВведи ответ на этот вопрос для твоего слова \"%s\" "
	       "(для слова \"%s\" ответ должен быть противоположный):\n", newWord, oldAnsNode->value);
	printf("%s?\n", newQuest);
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
*	Записывает массив строк в файл
*
*	@param[in] foutName Имя выходного файла
*	@param[in] words Массив строк
*	@param[in] NWords Количество строк
*	@param[in] separator Разделитель между словами
*
*	@return 1 - не удалось открыть файл; 0 - все прошло нормально
*/

int WordsToFile(const char* foutName, char** words, const int NWords, const char* separator) {
	assert(foutName != NULL);
	assert(words != NULL);
	assert(NWords >= 0);
	assert(separator != NULL);

	FILE* fout = fopen(foutName, "w");
	if (fout == NULL) {
		return 1;
	}
	for (int i = 0; i < NWords - 1; i++) {
		fprintf(fout, "%s", words[i]);
		fprintf(fout, "%s", separator);
	}
	fprintf(fout, "%s", words[NWords - 1]);
	fclose(fout);

	return 0;
}


/**
*	Записывает список всех доступных слов
*
*	@param[in] dataTree Дерево данных
*	@param[in] foutName Имя выходного файла
*
*	@return 1 - ошибка при создании массива слов; 2 - ошибка при записи слов в файл;\
 0 - все прошло нормально
*/

int GetWords(tree_t* dataTree, const char* foutName="words.txt") {
	assert(dataTree != NULL);
	assert(foutName != NULL);

	char** words = NULL;
	int NWords = 0;
	if (LastNodesWords(dataTree, words, &NWords) != 0) {
		return 1;
	}

	if (WordsToFile(foutName, words, NWords, "\n") != 0) {
		return 2;
	}

	for (int i = 0; i < NWords; i++) {
		free(words[i]);
	}
	free(words);

	return 0;
}


/**
*	Считывает ввод и определяет, какую секретную команды ввел пользователь
*
*	@return COMMAND_DATA - показ дерева данных;\
 COMMAND_WORDS - список доступных слов; 0 - секретная команда не введена
*/

int SecretCommandEntered() {
	const int commMaxLen = 50;  ///<Максимальная длина команды. Должна соответствовать самим командам
	const char dataCommand[] = "show_data";   ///<Показать дерево данных
	const char wordsCommand[] = "get_words";  ///<Получть список доступных слов


	char inp[commMaxLen + 1] = "";

	ScanNChars(inp, "[^\n]", commMaxLen);

	if (strcmp(inp, dataCommand) == 0) {
		return COMMAND_DATA;
	}
	if (strcmp(inp, wordsCommand) == 0) {
		return COMMAND_WORDS;
	}

	return 0;
}


/**
*	Обрабатывает секретные команды
*
*	@param[in] dataTree Дерево данных
*
*	@return 1 (true) - команда была введена и успешно выполнена;\
 0 (false) - команда не была введена; -1 - была введена команда COMMAND_DATA,\
 но возникла ошибка при выполнении; -2 - была введена команда COMMAND_WORDS,\
 но возникла ошибка при выполнении;
*/

int SecretCommand(tree_t* dataTree) {
	assert(dataTree != NULL);

	int secrComm = SecretCommandEntered();
	switch (secrComm) {
	case COMMAND_DATA:
		if (ShowData(dataTree) != 0) {
			printf("Ошибка при показе данных.\n");
			return -1;
		}
		return 1;
	case COMMAND_WORDS:
		char fName[201] = "";
		printf("Куда записать слова: ");
		ScanNChars(fName, "", 200);
		printf("\n");
		if (GetWords(dataTree, fName) != 0) {
			printf("\nОшибка при получении доступных слов.\n");
			return -2;
		}
		printf("\nЗаписано успешно в %s\n", fName);
		return 1;
	}

	return 0;
}


/**
*	Выводит одинаковые признаки
*
*	@param[in] dataTree Дерево данных
*	@prarm[in] way1 Путь до первого слова
*	@prarm[in] way2 Путь до второго слова
*	@prarm[out] firstDifferent Первый узел с необщим признаком
*
*	@return 1 - неверный формат пути; 0 - все прошло нормально
*/

int OutputSimilar(tree_t* dataTree, buf_t* way1, buf_t* way2, node_t*& firstDifferent) {
	assert(dataTree != NULL);
	assert(way1 != NULL);
	assert(way2 != NULL);
	assert(firstDifferent == NULL);

	firstDifferent = dataTree->root;

	char curCh1 = Bgetc(way1);
	char curCh2 = Bgetc(way2);
	char answer[50] = "";

	while (curCh1 == curCh2) {
		switch (curCh1) {
		case '0':
			strcpy(answer, "неверно");
			firstDifferent = firstDifferent->left;
			break;
		case '1':
			strcpy(answer, "верно");
			firstDifferent = firstDifferent->right;
			break;
		case '\0':
			Bseek(way1, -1, SEEK_CUR);
			Bseek(way2, -1, SEEK_CUR);
			return 0;
		default:
			return 1;
		}
		printf("%s. - %s\n", firstDifferent->parent->value, answer);

		curCh1 = Bgetc(way1);
		curCh2 = Bgetc(way2);
	}
	Bseek(way1, -1, SEEK_CUR);
	Bseek(way2, -1, SEEK_CUR);

	return 0;
}


/**
*	Сравнивает слова
*
*	@param[in] dataTree
*
*	@return 1 (3) - ошибка при построении пути до первого (второго) слова;\
 2 (4) - первое (второе) слово не найдено; 5 - ошибка при выводе одинаковых признаков;\
 0 - все прошло нормально
*/

int CompareWords(tree_t* dataTree) {
	assert(dataTree != NULL);

	char word1[treeStrMaxSize] = "";
	printf("Введи первое слово:\n");
	InputWord(word1, treeStrMaxSize);

	char word2[treeStrMaxSize] = "";
	printf("Введи второе слово:\n");
	InputWord(word2, treeStrMaxSize);

	node_t* tempNode = NULL;
	int err = 0;
	char* way1 = FindNodeByValue(dataTree, &word1, tempNode, &err);
	if (way1 == NULL) {
		if (err == 1) {
			return 1;
		}
		if (err == 2) {
			printf("\nПервое слово не найдено.\n");
			return 2;
		}
	}

	char* way2 = FindNodeByValue(dataTree, &word2, tempNode, &err);
	if (way2 == NULL) {
		if (err == 1) {
			return 3;
		}
		if (err == 2) {
			printf("\nВторое слово не найдено.\n");
			return 4;
		}
	}

	buf_t way1Buf = BufConstructor('r', way1, strlen(way1) + 1);
	buf_t way2Buf = BufConstructor('r', way2, strlen(way2) + 1);

	printf("\nОдинаковые признаки:\n");

	node_t* firstDifferent = NULL;
	if (OutputSimilar(dataTree, &way1Buf, &way2Buf, firstDifferent) != 0) {
		return 5;
	}
	
	printf("\nРазличие:\n");

	switch (Bgetc(&way1Buf)) {
	case '1':
		assert(Bgetc(&way2Buf) != '1');
		printf("%s: для слова \"%s\" - верно, для слова \"%s\" - неверно\n",
			   firstDifferent->value, word1, word2);
		break;
	case '0':
		assert(Bgetc(&way2Buf) != '0');
		printf("%s: для слова \"%s\" - неверно, для слова \"%s\" - верно\n",
			   firstDifferent->value, word1, word2);
		break;
	case '\0':
		printf("Различий нет, слова одинаковые.\n");
		break;
	default:
		assert(0);
	}

	return 0;
}


/**
*	Выводит признаки слова
*
*	@param[in] dataTree Дерево данных
*	@prarm[in] way Путь до слова
*
*	@return 1 - неверный формат пути; 0 - все прошло нормально
*/

int OutputDefinition(tree_t* dataTree, buf_t* way) {
	assert(dataTree != NULL);
	assert(way != NULL);

	node_t* curNode = dataTree->root;

	char curCh1 = Bgetc(way);
	char answer[50] = "";

	while (curCh1 != '\0') {
		switch (curCh1) {
		case '0':
			strcpy(answer, "неверно");
			curNode = curNode->left;
			break;
		case '1':
			strcpy(answer, "верно");
			curNode = curNode->right;
			break;
		default:
			return 1;
		}
		printf("%s. - %s\n", curNode->parent->value, answer);

		curCh1 = Bgetc(way);
	}

	return 0;
}


/**
*	Дает определение слову
*
*	@param[in] dataTree
*
*	@return 1 - ошибка при построении пути до слова; 2 - слово не найдено;\
 3 - ошибка при выводе определения; 0 - все прошло нормально
*/

int DetermineWord(tree_t* dataTree) {
	assert(dataTree != NULL);

	char word[treeStrMaxSize] = "";
	printf("Введи слово:\n");
	InputWord(word, treeStrMaxSize);

	node_t* tempNode = NULL;
	int err = 0;
	char* way = FindNodeByValue(dataTree, &word, tempNode, &err);
	if (way == NULL) {
		if (err == 1) {
			return 1;
		}
		if (err == 2) {
			printf("\nCлово не найдено.\n");
			return 2;
		}
	}

	buf_t wayBuf = BufConstructor('r', way, strlen(way) + 1);

	printf("\nОпределение:\n");

	if (OutputDefinition(dataTree, &wayBuf) != 0) {
		return 3;
	}

	return 0;
}


/**
*	Считывает ввод и определяет, какую дополнительную команду ввел пользователь
*
*	@return COMMAND_DEFINITION - определение слова;\
 COMMAND_COMPARE - сравнение слов; 0 - дополнительная команда не введена
*/

int AdvancedCommandEntered() {
	const int commMaxLen = 50;  ///<Максимальная длина команды. Должна соответствовать самим командам
	const char definitionCommand[] = "определение";   ///<Показать определение слова
	const char compareCommand[] = "сравнение";  ///<Сравнить два слова


	char inp[commMaxLen + 1] = "";

	ScanNChars(inp, "[^\n]", commMaxLen);

	if (strcmp(inp, definitionCommand) == 0) {
		return COMMAND_DEFINITION;
	}
	if (strcmp(inp, compareCommand) == 0) {
		return COMMAND_COMPARE;
	}

	return 0;
}


/**
*	Обрабатывает секретные команды
*
*	@param[in] dataTree Дерево данных
*
*	@return 1 (true) - команда была введена и успешно выполнена;\
 0 (false) - команда не была введена; -1 - была введена команда COMMAND_DEFINITION,\
 но возникла ошибка при выполнении; -2 - была введена команда COMMAND_COMPARE,\
 но возникла ошибка при выполнении;
*/

int AdvancedCommand(tree_t* dataTree) {
	assert(dataTree != NULL);

	int advComm = AdvancedCommandEntered();
	switch (advComm) {
	case COMMAND_DEFINITION:
		if (DetermineWord(dataTree) != 0) {
			printf("Ошибка при определении слова.\n");
			return -1;
		}
		return 1;
	case COMMAND_COMPARE:
		if (CompareWords(dataTree) == 1) {
			printf("\nОшибка при сравнении слов.\n");
			return -2;
		}
		return 1;
	}

	return 0;
}


/**
*	Главная функция игры
*
*	@return 1 - ошибка при открытии файла с данными; 2 - ошибка при чтении данных;\
 3 - ошибка при добавлении нового слова; 4 -ошибка при показе дерева данных;\
 5 - ошибка при записи доступных слов; 6 - ошибка при определении слова;\
 7 - ошибка при сравнении слов; 0 - все прошло нормально
*/

int StartAkinator(const char* dataFName = "data.bts") {

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
	printf("Я буду спрашивать тебя наводящие вопросы, а ты отвечай \"да\" или \"нет\".\n\n");

	int repeat = ANSWER_YES;
	while (repeat==ANSWER_YES) {
		printf("Можешь спросить меня, что означает какое - нибудь слово или сравнить два слова.\n"
			"Чтобы получить определение, введи \"определение\", чтобы сравнить - \"сравнение\", "
			"чтобы продолжить, нажми enter:\n");

		err = AdvancedCommand(&dataTree);
		if (err > 0) {
			printf("\n\n");
			continue;
		}
		if (err == -1) {
			return 6;
		}
		if (err == -2) {
			return 7;
		}
		if (err < -2) {
			assert(0);
		}

		printf("Давай играть! Если готов, нажми enter.\n");

		err = SecretCommand(&dataTree);
		if (err > 0) {
			printf("\n");
			continue;
		}
		if (err == -1) {
			return 4;
		}
		if (err == -2) {
			return 5;
		}
		if (err < -2) {
			assert(0);
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

	getchar();

	return err;
}