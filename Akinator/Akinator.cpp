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

enum extra_commands {
	no_command,
	command_data,
	command_words,
	command_definition,
	command_compare
};


enum common_errors {
	no_err,
	is_err
};


/**
*	���������, �������� �� ������ ������� ��������� ������
*
*	@praram[in] ch ������
*
*	@return 1 (true) - ��������; 0 (false) - �� ��������
*/

int IsUpperRus(char ch) {
	if (ch >= '�' && ch <= '�') {
		return 1;
	}

	return 0;
}

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
	int err = scanf(format, buf);
	if (err != 1 && err != 0) {
		return is_err;
	}
	fseek(stdin, 0, SEEK_END);

	return no_err;
}


/**
*	������� ������ � ������� �� �����
*
*	@param[in] dataFile ����
*	@param[out] err ��� ������: 1 - �������� ������ ������; 0 - ��� ������ ���������
*
*	@return ������ � �������. � ������ ������ ������������ ������ ������.
*/

enum GetDataTree_errs {
	invalid_data_format
};

tree_t GetDataTree(FILE* dataFile, int* err = NULL) {
	assert(dataFile != NULL);

	tree_t errTree = TreeConstructor("errTree");

	int fileSize = GetFileSize(dataFile);
	assert(fileSize > 0);

	char* data = (char*)calloc(fileSize + 1, sizeof(char));
	fread(data, sizeof(char), fileSize, dataFile);
	if (strchr(data, '\n') != NULL) {
		if (err != NULL) {
			*err = invalid_data_format;
		}
		free(data);
		return errTree;
	}

	int convErr = 0;
	tree_t dataTree = CodeToTree(data, "dataTree", &convErr);
	free(data);
	if (convErr != 0) {
		if (err != NULL) {
			*err = invalid_data_format;
		}
		TreeDestructor(&dataTree);
		return errTree;
	}

	if (err != NULL) {
		*err = no_err;
	}
	return dataTree;
}


/**
*	���������� ������ ������
*
*	@param[in] dataTree ������ ������
*
*	@return 1 - �� ������� �������� ������; 0 - ��� ������ ���������
*/

int ShowData(tree_t* dataTree) {
	assert(dataTree != NULL);

	if (ShowTree(dataTree) != 0) {
		return is_err;
	}

	return no_err;
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
*	���������� �� ������������ �����
*
*	@param[out] newWord �����
*	@param[in] wordMaxSize ������������ ����� �����
*/

void InputWord(char* newWord, const int wordMaxSize) {
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
*	���������� �� ������������ �������
*
*	@param[out] newQuest ������
*	@param[in] questMaxSize ������������ ����� �������
*/

void InputQuestion(char* newQuest, const int questMaxSize) {
	assert(newQuest != NULL);
	assert(questMaxSize > 0);

	while (1) {
		memset(newQuest, 0, questMaxSize);

		ScanNChars(newQuest, "[^\n]", questMaxSize - 1);

		if (newQuest[questMaxSize - 2] != '\0') {
			printf("������ ������� �������. ����� ����� �������� ������ (����. %d ��������):\n", \
				questMaxSize - 1);
			continue;
		}

		if (!IsUpperRus(newQuest[0])) {
			printf("������ ������ ���������� � ������� �����.\n");
			continue;
		}

		if (strstr(newQuest, " �� ") || strstr(newQuest, "�� ")) {
			printf("������ ������� ������ �� ������ \"��\". ����� ������ ������:\n");
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

enum AddWords_errs {
	add_new_word_err,
	add_old_word_err,
	edit_old_node_err,
	ansForNew_invalid,
};

int AddWords(tree_t* dataTree, node_t* oldAnsNode, char* newWord, char* newQuest, const int ansForNew) {
	assert(dataTree != NULL);
	assert(oldAnsNode != NULL);
	assert(newWord != NULL);
	assert(newQuest != NULL);

	switch (ansForNew) {
	case ANSWER_YES:
		if (AddChild(dataTree, oldAnsNode, newWord, RIGHT_CHILD) != 0) {
			return add_new_word_err;
		}
		if (AddChild(dataTree, oldAnsNode, oldAnsNode->value, LEFT_CHILD) != 0) {
			return add_old_word_err;
		}
		if (ChangeNodeValue(oldAnsNode, newQuest) != 0) {
			return edit_old_node_err;
		}
		break;
	case ANSWER_NO:
		if (AddChild(dataTree, oldAnsNode, newWord, LEFT_CHILD) != 0) {
			return add_new_word_err;
		}
		if (AddChild(dataTree, oldAnsNode, oldAnsNode->value, RIGHT_CHILD) != 0) {
			return add_old_word_err;
		}
		if (ChangeNodeValue(oldAnsNode, newQuest) != 0) {
			return edit_old_node_err;
		}
		break;
	default:
		return ansForNew_invalid;
	}
	return no_err;
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

enum DataToFile_errs {
	code_from_tree_generation_err,
	file_open_err,
	file_write_err,
};

int DataToFile(tree_t* dataTree, const char* dataFName) {
	assert(dataTree != NULL);
	assert(dataFName != NULL);

	int dataSize = 0;
	char* newData = TreeToCode(dataTree, &dataSize);
	if (newData == NULL) {
		return code_from_tree_generation_err;
	}

	FILE* dataFile = fopen(dataFName, "w");
	if (dataFile == NULL) {
		return file_open_err;
	}
	if (fwrite(newData, sizeof(char), dataSize, dataFile) != dataSize) {
		return file_write_err;
	}

	fclose(dataFile);
	free(newData);
	return no_err;
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

	const char strMaxSize = treeStrMaxSize;  //������������ ����� �������.

	char newWord[strMaxSize] = "";
	printf("��... �������� � �� ���� ��� �����. �������� ��� � ���, ����� �����, ������� �� �������.\n");
	printf("����� ������ ���� ����. ���� ������� ���������, �� � �������� ������ ������:\n");
	InputWord(newWord, strMaxSize);

	char newQuest[strMaxSize] = "";
	printf("����� ������, ������� �������� ����� \"%s\" "
	       "�� ������ ����� \"%s\":\n", oldAnsNode->value, newWord);
	InputQuestion(newQuest, strMaxSize);

	

	printf("\n����� ����� �� ���� ������ ��� ������ ����� \"%s\" "
	       "(��� ����� \"%s\" ����� ������ ���� ���������������):\n", newWord, oldAnsNode->value);
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
*	���������� ������ ����� � ����
*
*	@param[in] foutName ��� ��������� �����
*	@param[in] words ������ �����
*	@param[in] NWords ���������� �����
*	@param[in] separator ����������� ����� �������
*
*	@return 1 - �� ������� ������� ����; 0 - ��� ������ ���������
*/

enum WordsToFile_errs {
	cannot_open_file,
};

int WordsToFile(const char* foutName, char** words, const int NWords, const char* separator) {
	assert(foutName != NULL);
	assert(words != NULL);
	assert(NWords >= 0);
	assert(separator != NULL);

	FILE* fout = fopen(foutName, "w");
	if (fout == NULL) {
		return file_open_err;
	}
	for (int i = 0; i < NWords - 1; i++) {
		fprintf(fout, "%s", words[i]);
		fprintf(fout, "%s", separator);
	}
	fprintf(fout, "%s", words[NWords - 1]);
	fclose(fout);

	return no_err;
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

enum GetWords_errs {
	create_words_array_err,
	write_words_to_file_err
};

int GetWords(tree_t* dataTree, const char* foutName="words.txt") {
	assert(dataTree != NULL);
	assert(foutName != NULL);

	char** words = NULL;
	int NWords = 0;
	if (LastNodesWords(dataTree, words, &NWords) != 0) {
		return create_words_array_err;
	}

	if (WordsToFile(foutName, words, NWords, "\n") != 0) {
		return write_words_to_file_err;
	}

	for (int i = 0; i < NWords; i++) {
		free(words[i]);
	}
	free(words);

	return no_err;
}


/**
*	��������� ���� � ����������, ����� ��������� ������� ���� ������������
*
*	@return command_data - ����� ������ ������;\
 command_words - ������ ��������� ����; no_command - ��������� ������� �� �������
*/

int SecretCommandEntered() {
	const int commMaxLen = 50;  ///<������������ ����� �������. ������ ��������������� ����� ��������
	const char dataCommand[] = "show_data";   ///<�������� ������ ������
	const char wordsCommand[] = "get_words";  ///<������� ������ ��������� ����


	char inp[commMaxLen + 1] = "";

	ScanNChars(inp, "[^\n]", commMaxLen);

	if (strcmp(inp, dataCommand) == 0) {
		return command_data;
	}
	if (strcmp(inp, wordsCommand) == 0) {
		return command_words;
	}

	return no_command;
}


/**
*	������������ ��������� �������
*
*	@param[in] dataTree ������ ������
*
*	@return 1 (true) - ������� ���� ������� � ������� ���������;\
 0 (false) - ������� �� ���� �������; -1 - ���� ������� ������� command_data,\
 �� �������� ������ ��� ����������; -2 - ���� ������� ������� command_words,\
 �� �������� ������ ��� ����������;
*/

enum SecretCommand_errs {
	command_data_err = -1,
	command_words_err = -2,
};

int SecretCommand(tree_t* dataTree) {
	assert(dataTree != NULL);

	int secrComm = SecretCommandEntered();
	switch (secrComm) {
	case command_data:
		if (ShowData(dataTree) != 0) {
			printf("������ ��� ������ ������.\n");
			return -1;
		}
		return 1;
	case command_words:
		char fName[201] = "";
		printf("���� �������� �����: ");
		ScanNChars(fName, "", 200);
		printf("\n");
		if (GetWords(dataTree, fName) != 0) {
			printf("\n������ ��� ��������� ��������� ����.\n");
			return -2;
		}
		printf("\n�������� ������� � %s\n", fName);
		return 1;
	}

	return 0;
}


/**
*	������� ���������� ��������
*
*	@param[in] dataTree ������ ������
*	@prarm[in] way1 ���� �� ������� �����
*	@prarm[in] way2 ���� �� ������� �����
*	@prarm[out] firstDifferent ������ ���� � ������� ���������
*
*	@return 1 - �������� ������ ����; 0 - ��� ������ ���������
*/

enum OutputSimilar_errs {
	invalid_way_format
};

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
			strcpy(answer, "�������");
			firstDifferent = firstDifferent->left;
			break;
		case '1':
			strcpy(answer, "�����");
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
*	���������� �����
*
*	@param[in] dataTree
*
*	@return 1 (3) - ������ ��� ���������� ���� �� ������� (�������) �����;\
 2 (4) - ������ (������) ����� �� �������; 5 - ������ ��� ������ ���������� ���������;\
 0 - ��� ������ ���������
*/

enum CompareWords_errs {
	first_way_construct_err,
	second_way_construct_err,
	first_word_not_found,
	second_word_not_found,
	similar_output_err
};

int CompareWords(tree_t* dataTree) {
	assert(dataTree != NULL);

	char word1[treeStrMaxSize] = "";
	printf("����� ������ �����:\n");
	InputWord(word1, treeStrMaxSize);

	char word2[treeStrMaxSize] = "";
	printf("����� ������ �����:\n");
	InputWord(word2, treeStrMaxSize);

	node_t* tempNode = NULL;
	int err = 0;
	char* way1 = FindNodeByValue(dataTree, &word1, tempNode, &err);
	if (way1 == NULL) {
		if (err == 1) {
			return first_way_construct_err;
		}
		if (err == 2) {
			printf("\n������ ����� �� �������.\n");
			return first_word_not_found;
		}
	}

	char* way2 = FindNodeByValue(dataTree, &word2, tempNode, &err);
	if (way2 == NULL) {
		if (err == 1) {
			return second_way_construct_err;
		}
		if (err == 2) {
			printf("\n������ ����� �� �������.\n");
			return second_word_not_found;
		}
	}

	buf_t way1Buf = BufConstructor('r', way1, strlen(way1) + 1);
	buf_t way2Buf = BufConstructor('r', way2, strlen(way2) + 1);

	printf("\n���������� ��������:\n");

	node_t* firstDifferent = NULL;
	if (OutputSimilar(dataTree, &way1Buf, &way2Buf, firstDifferent) != 0) {
		return similar_output_err;
	}
	
	printf("\n��������:\n");

	switch (Bgetc(&way1Buf)) {
	case '1':
		assert(Bgetc(&way2Buf) != '1');
		printf("%s: ��� ����� \"%s\" - �����, ��� ����� \"%s\" - �������\n",
			   firstDifferent->value, word1, word2);
		break;
	case '0':
		assert(Bgetc(&way2Buf) != '0');
		printf("%s: ��� ����� \"%s\" - �������, ��� ����� \"%s\" - �����\n",
			   firstDifferent->value, word1, word2);
		break;
	case '\0':
		printf("�������� ���, ����� ����������.\n");
		break;
	default:
		assert(0);
	}

	free(way1);
	free(way2);
	BufDestructor(&way1Buf);
	BufDestructor(&way2Buf);
	return no_err;
}


/**
*	������� �������� �����
*
*	@param[in] dataTree ������ ������
*	@prarm[in] way ���� �� �����
*
*	@return 1 - �������� ������ ����; 0 - ��� ������ ���������
*/

enum OutputDefinition_errs {
	way_format_invalid
};

int OutputDefinition(tree_t* dataTree, buf_t* way) {
	assert(dataTree != NULL);
	assert(way != NULL);

	node_t* curNode = dataTree->root;

	char curCh1 = Bgetc(way);
	char answer[50] = "";

	while (curCh1 != '\0') {
		switch (curCh1) {
		case '0':
			strcpy(answer, "�������");
			curNode = curNode->left;
			break;
		case '1':
			strcpy(answer, "�����");
			curNode = curNode->right;
			break;
		default:
			return invalid_way_format;
		}
		printf("%s. - %s\n", curNode->parent->value, answer);

		curCh1 = Bgetc(way);
	}

	return no_err;
}


/**
*	���� ����������� �����
*
*	@param[in] dataTree
*
*	@return 1 - ������ ��� ���������� ���� �� �����; 2 - ����� �� �������;\
 3 - ������ ��� ������ �����������; 0 - ��� ������ ���������
*/

enum DetermineWord_errs {
	create_way_to_word_err,
	word_not_found,
	output_definition_err
};

int DetermineWord(tree_t* dataTree) {
	assert(dataTree != NULL);

	char word[treeStrMaxSize] = "";
	printf("����� �����:\n");
	InputWord(word, treeStrMaxSize);

	node_t* tempNode = NULL;
	int err = 0;
	char* way = FindNodeByValue(dataTree, &word, tempNode, &err);
	if (way == NULL) {
		if (err == 1) {
			return create_way_to_word_err;
		}
		if (err == 2) {
			printf("\nC���� �� �������.\n");
			return word_not_found;
		}
	}

	buf_t wayBuf = BufConstructor('r', way, strlen(way) + 1);

	printf("\n�����������:\n");

	if (OutputDefinition(dataTree, &wayBuf) != 0) {
		return output_definition_err;
	}

	free(way);
	BufDestructor(&wayBuf);
	return no_err;
}


/**
*	��������� ���� � ����������, ����� �������������� ������� ���� ������������
*
*	@return command_definition - ����������� �����;\
 command_compare - ��������� ����; 0 - �������������� ������� �� �������
*/

int AdvancedCommandEntered() {
	const int commMaxLen = 50;  ///<������������ ����� �������. ������ ��������������� ����� ��������
	const char definitionCommand[] = "�����������";   ///<�������� ����������� �����
	const char compareCommand[] = "���������";  ///<�������� ��� �����


	char inp[commMaxLen + 1] = "";

	ScanNChars(inp, "[^\n]", commMaxLen);

	if (strcmp(inp, definitionCommand) == 0) {
		return command_definition;
	}
	if (strcmp(inp, compareCommand) == 0) {
		return command_compare;
	}

	return 0;
}


/**
*	������������ ��������� �������
*
*	@param[in] dataTree ������ ������
*
*	@return 1 (true) - ������� ���� ������� � ������� ���������;\
 0 (false) - ������� �� ���� �������; -1 - ���� ������� ������� command_definition,\
 �� �������� ������ ��� ����������; -2 - ���� ������� ������� command_compare,\
 �� �������� ������ ��� ����������;
*/

enum AdvancedCommand_errs {
	command_definition_err = -1,
	command_compare_err = -2
};

int AdvancedCommand(tree_t* dataTree) {
	assert(dataTree != NULL);

	int advComm = AdvancedCommandEntered();
	switch (advComm) {
	case command_definition:
		if (DetermineWord(dataTree) != 0) {
			printf("������ ��� ����������� �����.\n");
			return command_definition_err;
		}
		return 1;
	case command_compare:
		if (CompareWords(dataTree) == 1) {
			printf("\n������ ��� ��������� ����.\n");
			return command_compare_err;
		}
		return 1;
	}

	return 0;
}


/**
*	������� ������� ����
*
*	@return 1 - ������ ��� �������� ����� � �������; 2 - ������ ��� ������ ������;\
 3 - ������ ��� ���������� ������ �����; 4 - ������ ��� ������ ������ ������;\
 5 - ������ ��� ������ ��������� ����; 6 - ������ ��� ����������� �����;\
 7 - ������ ��� ��������� ����; 0 - ��� ������ ���������
*/

enum StartAkinator_errs {
	open_data_file_err,
	read_data_err,
	new_word_add_err,
	show_data_err,
	write_available_words_err,
	word_definition_err,
	words_compare_err,
};

int StartAkinator(const char* dataFName = "data.bts") {

	printf("��������... ");

	FILE* dataFile = GetDataFile(dataFName);
	if (dataFile == NULL) {
		printf("\n\n������ ��� �������� ������: ���������� ������� ���� � �������\n");
		return open_data_file_err;
	}

	int err = 0;
	tree_t dataTree = GetDataTree(dataFile, &err);
	fclose(dataFile);
	if (err != 0) {
		printf("\n\n������ ��� �������� ������: ���������� ��������� ������\n");
		return read_data_err;
	}
	

	printf("\n\n");
	printf("����� ���������� � ��������! ������� �����, � � �������� ��� ��������.\n");
	printf("� ���� ���������� ���� ��������� �������, � �� ������� \"��\" ��� \"���\".\n\n");

	int repeat = ANSWER_YES;
	while (repeat==ANSWER_YES) {
		printf("������ �������� ����, ��� �������� ����� - ������ ����� ��� �������� ��� �����.\n"
			"����� �������� �����������, ����� \"�����������\", ����� �������� - \"���������\", "
			"����� ����������, ����� enter:\n");

		err = AdvancedCommand(&dataTree);
		if (err > 0) {
			printf("\n\n");
			continue;
		}
		if (err == command_definition_err) {
			return word_definition_err;
		}
		if (err == command_compare_err) {
			return words_compare_err;
		}
		if (err < -2) {
			assert(0);
		}

		printf("����� ������! ���� �����, ����� enter.\n");

		err = SecretCommand(&dataTree);
		if (err > 0) {
			printf("\n");
			continue;
		}
		if (err == command_data_err) {
			return show_data_err;
		}
		if (err == command_words_err) {
			return write_available_words_err;
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
				printf("������ ��� ���������� ������ �����. %d\n", TErr);
				return add_new_word_err;
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

	return no_err;
}

int main() {

	int err = StartAkinator();

	getchar();

	return err;
}