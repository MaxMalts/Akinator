#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include "btree_string.h"

#define ANSWER_YES 1
#define ANSWER_NO 0

#define COMMAND_DATA 1
#define COMMAND_WORDS 2

/**
*	��������� ���� � �������
*
*	@param[in] fName ��� �����
*
*	@return ��������� �� ����. � ������ ������ ����� NULL.
*/

FILE* GetDataFile(const char* fName) {
	assert(fName != NULL);

	return  fopen(fName, "r");
}


/**
*	���������� ������ ����� � ������ ������� '\r'
*
*	@param[in] f ����
*
*	@return ������ �����. ���� ������ �������������, �� ��������� ������
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
*	������ �������� ���������� �������� �� �������� ������ �� �������������� �������
*
*	@param[out] buf �����
*	@param[in] formSpec ������������ �������
*	@param[in] NChars ���������� ��������. ��������, � ����� ����� ���������� ��� ���� '\0'!
*
*	@return 1 - ������; 0 - ��� ������ ���������
*/

int ScanNChars(char* buf, const char* formSpec, const int NChars) {
	assert(buf != NULL);
	assert(NChars >= 0);

	char format[100] = "";
	sprintf(format, "%%%s%ds", formSpec, NChars);
	if (scanf(format, buf) != 1)
		return 1;
	fseek(stdin, 0, SEEK_END);

	return 0;
}

/**
*	������� ������ � ������� �� �����
*
*	@param[in] dataFile ����
*	@param[out] err ��� ������: 1 - �������� ������ ������; 0 - ��� ������ ���������
*
*	@return ������ � �������. � ������ ������ ������������ ������ ������.
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
*	���������� ������ ������
*
*	@param[in] dataFName
*
*	@return 1 - ������ ��� �������� �����; 2 - ������ ��� ������ ������;\
 3 - ������ ��������� ������; 0 - ��� ������ ���������
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
*	����������, ����� ������������ ���� "��" ��� "���"
*
*	@return ANSWER_YES - "��"; ANSWER_NO - "���"
*/

int GetYesOrNo() {

	char ans[4] = "";

	while (1) {
		scanf("%3s", ans);
		fseek(stdin, 0, SEEK_END);

		if (strcmp(ans, "��") == 0) {
			return ANSWER_YES;
		}
		else if (strcmp(ans, "���") == 0) {
			return ANSWER_NO;
		}
		else {
			printf("�������� \"��\" ��� \"���\".\n");
		}
	}

}


/**
*	���������� ����� (�����������)
*
*	@param[in] curNode ������� ���� (��� ��������� ������ - ������ ������ ������)
*	@param[out] ansNode ����, ���������� �������
*
*	@return 0 (false) - ������ �� ��������; 1 (true) - ����� ��������
*/

int AkinatorCycle(node_t* curNode, node_t*& ansNode) {
	assert(curNode!= NULL);

	if (NodeChildsCount(curNode) == 0) {
		printf("��� - %s!\n", curNode->value);
		ansNode = curNode;

		printf("� ������?\n");

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
*	���������� �� ������������ ������ �����
*
*	@param[out] newWord ����� �����
*	@param[in] wordMaxSize ������������ ����� �����
*/

void GetNewWord(char* newWord, const int wordMaxSize) {
	assert(newWord != NULL);
	assert(wordMaxSize > 0);

	while (1) {
		memset(newWord, 0, wordMaxSize);

		ScanNChars(newWord, "", wordMaxSize - 1);

		if (newWord[wordMaxSize - 2] != '\0') {
			printf("����� ������� �������. ����� ����� �������� ����� (����. %d ��������):\n", \
				wordMaxSize - 1);
		}
		else break;
	}
}


/**
*	���������� �� ������������ ������ �������
*
*	@param[out] newQuest ����� ������
*	@param[in] questMaxSize ������������ ����� �������
*/

void GetNewQuestion(char* newQuest, const int questMaxSize) {
	assert(newQuest != NULL);
	assert(questMaxSize > 0);

	while (1) {
		memset(newQuest, 0, questMaxSize);

		ScanNChars(newQuest, "[^\n]", questMaxSize - 1);

		if (newQuest[questMaxSize - 2] != '\0') {
			printf("������ ������� �������. ����� ����� �������� ������ (����. %d ��������):\n", \
				questMaxSize - 1);
		}
		else break;
	}
}


/**
*	��������� � ������ ������ ����� �����
*
*	@param dataTree ������ ������
*	@param oldAnsNode ���� �� ������ ������
*	@param newWord ����� �����
*	@param newQuest ����� ������
*	@param ansForNew ����� �� ����� ������ ��� ������ ����� (ANSWER_YES ��� ANSWER_NO)
*
*	@return 1 - �� ������� �������� ����� �����; 2 - �� ������� �������� ������ �����;\
 3 - �� ������� �������� �������� ������� ���� �� ������; 4 - ������������ �������� ansForNew;\
 0 - ��� ������ ���������
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
*	���������� ������ ���� � ����
*
*	@param[in] dataTree ������ � �������
*	@param[in] dataFName ��� ����� ��� ������
*
*	@return 1 - �������� ��� ��������� ���� �� ������; 2 - �������� ��� �������� �����;\
 3 - �������� ��� ������ � ����; 0 - ��� ������ ���������
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
*	��������� ����� ������ � ��������� (����������) ����� ������ � ����
*
*	@param dataTree ������ ������
*	@param oldAnsNode ���� �� ������ ������
*	@param[in] dataFName ��� ����� � �������
*
*	@return ���������� ������. ���� � ������� �������� 0, �� ��������� ������ � ������� AddWords();\
 ���� � ������� �������� 1, �� � ������� DataToFile. ��������������� ���� ������ ����� � ������� ������,\
 �� ������ � ��������������� ��������.
*/

int AddQuestion(tree_t* dataTree, node_t* oldAnsNode, const char* dataFName) {
	assert(oldAnsNode != NULL);

	const char strMaxSize = treeStrMaxSize;  //������������ ����� �������. ��� ���������\
											   �������� ��������������� �������� � �������,\
											   ���������� (*)!

	char newWord[strMaxSize] = "";
	printf("����� �����, ������� �� �������. ����� ������ ���� ����. \
���� ������� ���������, �� � �������� ������ ������:\n");
	GetNewWord(newWord, strMaxSize);

	char newQuest[strMaxSize] = "";
	printf("����� ������, ������� �������� ����� \"%s\" \
�� ������ ����� \"%s\":\n", oldAnsNode->value, newWord);
	GetNewQuestion(newQuest, strMaxSize);

	

	printf("\n����� ����� �� ���� ������ ��� ������ ����� \"%s\" \
(��� ����� \"%s\" ����� ������ ���� ���������������):\n", newWord, oldAnsNode->value);
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
*	���������� ������ ����� � ����
*
*	@param[in] foutName ��� ��������� �����
*	@param[in] words ������ �����
*	@param[in] NWords ���������� �����
*	@param[in] separator ����������� ����� �������
*
*	@return 1 - �� ������� ������� ����; 0 - ��� ������ ���������
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
*	���������� ������ ���� ��������� ����
*
*	@param[in] dataTree ������ ������
*	@param[in] foutName ��� ��������� �����
*
*	@return 1 - ������ ��� �������� ������� ����; 2 - ������ ��� ������ ���� � ����;\
 0 - ��� ������ ���������
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
*	��������� ���� � ����������, ����� ��������� ������� ���� ������������
*
*	@return COMMAND_DATA - ����� ������ ������;\
 COMMAND_WORDS - ������ ��������� ����; 0 - ��������� �������� �� �������
*/

int SecretCommand() {
	const int commMaxLen = 50;  ///<������������ ����� �������. ������ ��������������� ����� ��������
	const char dataCommand[] = "show_data";   ///<�������� ������ ������
	const char wordsCommand[] = "get_words";  ///<������� ������ ��������� ����


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
*	������� ������� ����
*
*	@return 1 - ������ ��� �������� ����� � �������; 2 - ������ ��� ������ ������;\
 3 - ������ ��� ���������� ������ �����; 4 -������ ��� ������ ������ ������;\
 5 - ������ ��� ������ ��������� ����; 0 - ��� ������ ���������
*/

int StartAkinator(const char* dataFName = "data.bts") {
	//setlocale(LC_ALL, "Russian");

	printf("��������... ");

	FILE* dataFile = GetDataFile(dataFName);
	if (dataFile == NULL) {
		printf("\n\n������ ��� �������� ������: ���������� ������� ���� � �������\n");
		return 1;
	}

	int err = 0;
	tree_t dataTree = GetDataTree(dataFile, &err);
	fclose(dataFile);
	if (err != 0) {
		printf("\n\n������ ��� �������� ������: ���������� ��������� ������\n");
		return 2;
	}
	

	printf("\n\n");
	printf("����� ���������� � ��������! ������� �����, � � �������� ��� ��������.\n");
	printf("� ���� ���������� ���� ��������� �������, � �� ������� \"��\" ��� \"���\".\n");

	int repeat = ANSWER_YES;
	while (repeat==ANSWER_YES) {
		printf("���� �����, ����� enter.\n");
		int secrComm = SecretCommand();
		switch (secrComm) {
		case COMMAND_DATA:
			if (ShowData(dataFName) != 0) {
				printf("������ ��� ������ ������.\n");
				return 4;
			}
			continue;
		case COMMAND_WORDS:
			char fName[201] = "";
			ScanNChars(fName, "", 200);
			printf("\n");
			if (GetWords(&dataTree, fName) != 0) {
				printf("\n������ ��� ��������� ��������� ����.\n");
				return 5;
			}
			else {
				printf("\n�������� ������� � %s\n\n", fName);
			}
			continue;
		}

		node_t* ansNode = NULL;
		int guessed = AkinatorCycle(dataTree.root, ansNode);
		printf("\n");

		if (!guessed) {
			int TErr = AddQuestion(&dataTree, ansNode, dataFName);
			if (TErr != 0) {
				printf("������ ��� ���������� ������ �����. %d\n", TErr);
				return 3;
			}
			else {
				printf("����� ���������.\n\n");
			}
		}

		printf("������� ���?\n");
		repeat = GetYesOrNo();
		printf("\n");
	}

	printf("����!");

	return 0;
}

int main() {

	int err = StartAkinator();

	//int err = ShowData();

	fseek(stdin, 0, SEEK_END);
	getchar();

	return err;
}