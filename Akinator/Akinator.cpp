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

int AkinatorCycle(tree_t* dataTree) {
	assert(dataTree != NULL);


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
	
	AkinatorCycle(&dataTree);

	return 0;
}

int main() {

	StartAkinator();

}