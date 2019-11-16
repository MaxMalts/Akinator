#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include "btree_string.h"


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
*	Добивается, чтобы пользователь ввел "да" или "нет"
*
*	@return 1 - "да"; 0 - "нет"
*/

int GetYesOrNo() {

	char ans[4] = "";

	while (true) {
		scanf("%3s", ans);

		if (strcmp(ans, "да") == 0) {
			return 1;
		}
		else if (strcmp(ans, "нет") == 0) {
			return 0;
		}
		else {
			printf("Ответьте \"да\" или \"нет\".\n");
		}
	}

}

int AkinatorCycle(node_t* curNode, node_t* ansNode) {
	assert(curNode!= NULL);

	if (NodeChildsCount(curNode) == 0) {
		printf("Это - %s\n", curNode->value);
		ansNode = curNode;

		printf("Я угадал?\n");

		int ans = GetYesOrNo();

		assert(ans == 0 || ans == 1);
		return ans;
	}


	printf("%s\n", curNode->value);

	int ans = GetYesOrNo();

	switch (ans) {
	case 1:
		return AkinatorCycle(curNode->right, ansNode);
	case 0:
		return AkinatorCycle(curNode->left, ansNode);
	default:
		assert(0);
	}
}

int StartAkinator() {
	setlocale(LC_CTYPE, "Russian");

	printf("Загрузка... ");

	FILE* dataFile = GetDataFile("data.bts");
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
	printf("Добро пожаловать в Акинатор! Загадай предмет, а я попробую его отгадать.\n");
	printf("Я буду спрашивать тебя наводящие вопросы, а ты отвечай \"да\" или \"нет\".\n");
	printf("Если готов, нажми enter.\n");
	getchar();
	
	node_t* ansNode = NULL;
	int guessed = AkinatorCycle(dataTree.root, ansNode);

	if (!guessed) {
		
	}

	return 0;
}

int main() {

	StartAkinator();

}