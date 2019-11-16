#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include "btree_string.h"

#define ANSWER_YES 1
#define ANSWER_NO 2


/**
*	���뢠�� 䠩� � ����묨
*
*	@param[in] fName ��� 䠩��
*
*	@return �����⥫� �� 䠩�. � ��砥 �訡�� ࠢ�� NULL.
*/

FILE* GetDataFile(const char* fName) {
	assert(fName != NULL);

	return  fopen(fName, "r");
}


/**
*	��।���� ࠧ��� 䠩�� � ��⮬ ᨬ���� '\r'
*
*	@param[in] f ����
*
*	@return ������ 䠩��. �᫨ ࠧ��� ����⥫��, � �ந��諠 �訡��
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
*	������� ��ॢ� � ����묨 �� 䠩��
*
*	@param[in] dataFile ����
*	@param[out] err ��� �訡��: 1 - ������ �ଠ� ������; 0 - �� ��諮 ��ଠ�쭮
*
*	@return ��ॢ� � ����묨. � ��砥 �訡�� �����頥��� ���⮥ ��ॢ�.
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
*	����������, �⮡� ���짮��⥫� ���� "��" ��� "���"
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
			printf("�⢥��� \"��\" ��� \"���\".\n");
		}
	}

}


/**
*	�⣠�뢠�� ᫮�� (४��ᨢ���)
*
*	@param[in] curNode ����騩 㧥� (�� ��ࢨ筮� �맮�� - ��७� ��ॢ� ������)
*	@param[out] ansNode ����, ��騩�� �⢥⮬
*
*	@return 0 (false) - ᫮��� �� �⣠����; 1 (true) - ᫮�� �⣠����
*/

int AkinatorCycle(node_t* curNode, node_t*& ansNode) {
	assert(curNode!= NULL);

	if (NodeChildsCount(curNode) == 0) {
		printf("�� - %s!\n", curNode->value);
		ansNode = curNode;

		printf("� 㣠���?\n");

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
*	���������� �� ���짮��⥫� ������ ᫮��
*
*	@param[out] newWord ����� ᫮��
*	@param[in] wordMaxSize ���ᨬ��쭠� ����� ᫮��
*/

void GetNewWord(char* newWord, const int wordMaxSize) {
	assert(newWord != NULL);
	assert(wordMaxSize > 0);

	while (1) {
		memset(newWord, 0, wordMaxSize);
		scanf("%100s", newWord);   //(*)
		fseek(stdin, 0, SEEK_END);

		if (newWord[wordMaxSize - 2] != '\0') {
			printf("����� ᫨誮� �������. ����� ����� ���⪮� ᫮�� (����. %d ᨬ�����):\n", \
				wordMaxSize - 1);
		}
		else break;
	}
}


/**
*	���������� �� ���짮��⥫� ������ �����
*
*	@param[out] newQuest ���� �����
*	@param[in] questMaxSize ���ᨬ��쭠� ����� �����
*/

void GetNewQuestion(char* newQuest, const int questMaxSize) {
	assert(newQuest != NULL);
	assert(questMaxSize > 0);

	while (1) {
		memset(newQuest, 0, questMaxSize);
		scanf("%100[^\n]s", newQuest);   //(*)
		fseek(stdin, 0, SEEK_END);

		if (newQuest[questMaxSize - 2] != '\0') {
			printf("����� ᫨誮� ������. ����� ����� ���⪨� ����� (����. %d ᨬ�����):\n", \
				questMaxSize - 1);
		}
		else break;
	}
}


/**
*	�������� � ��ॢ� ������ ����� ᫮��
*
*	@param dataTree ��ॢ� ������
*	@param oldAnsNode ���� � ���� ᫮���
*	@param newWord ����� ᫮��
*	@param newQuest ���� �����
*	@param ansForNew �⢥� �� ���� ����� ��� ������ ᫮�� (ANSWER_YES ��� ANSWER_NO)
*
*	@return 1 - �� 㤠���� �������� ����� ᫮��; 2 - �� 㤠���� �������� ��஥ ᫮��;\
 3 - �� 㤠���� �������� ���祭�� ��ண� 㧫� �� �����; 4 - �����४⭮� ���祭�� ansForNew;\
 0 - �� ��諮 ��ଠ�쭮
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
}


/**
*	�����뢠�� ����� ���� � 䠩�
*
*	@param[in] dataTree ��ॢ� � ����묨
*	@param[in] dataFName ��� 䠩�� ��� �����
*
*	@return 1 - �஡���� �� �����樨 ���� �� ��ॢ�; 2 - �஡���� �� ����⨨ 䠩��;\
 3 - �஡���� �� ����� � 䠩�; 0 - �� ��諮 ��ଠ�쭮
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
*	�������� ���� ����� � ��࠭�� (�����뢠��) ����� ��ॢ� � 䠩�
*
*	@param dataTree ��ॢ� ������
*	@param oldAnsNode ���� � ���� ᫮���
*	@param[in] dataFName ��� 䠩�� � ����묨
*
*	@return �����頥� �訡��. �᫨ � ࠧ�拉 ����⪮� 0, � �ந��諠 �訡�� � �㭪樨 AddWords();\
 �᫨ � ࠧ�拉 ����⪮� 1, � � �㭪樨 DataToFile. ���⢥�����騥 ���� �訡�� ���� � ࠧ�拉 ������,\
 �� ᬮ�� � ᮮ⢥������� �㭪���.
*/

int AddQuestion(tree_t* dataTree, node_t* oldAnsNode, const char* dataFName) {
	assert(oldAnsNode != NULL);

	const char strMaxSize = treeStrMaxSize;  //���ᨬ��쭠� ����� �����. �� ���������\
											   ������� ᮮ⢥�����騥 ���祭�� � ��ப��,\
											   ����祭��� (*)!

	char newWord[strMaxSize] = "";
	printf("����� ᫮��, ���஥ �� �������. ����� ������ ���� ����. \
�᫨ ������� ��᪮�쪮, � � ����� ⮫쪮 ��ࢮ�:\n");
	GetNewWord(newWord, strMaxSize);   //(*)

	char newQuest[strMaxSize] = "";
	printf("����� �����, ����� �⫨砥� ᫮�� \"%s\" \
�� ⢮��� ᫮�� \"%s\":\n", oldAnsNode->value, newWord);
	GetNewQuestion(newQuest, strMaxSize);   //(*)

	

	printf("\n����� �⢥� �� ��� ����� ��� ⢮��� ᫮�� \"%s\" \
(��� ᫮�� \"%s\" �⢥� ������ ���� ��⨢��������):\n", newWord, oldAnsNode->value);
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
*	������� �㭪�� ����
*
*	@return 1 - �訡�� �� ����⨨ 䠩�� � ����묨; 2 - �訡�� �� �⥭�� ������;\
 3 - �訡�� �� ���������� ������ ᫮��; 0 - �� ��諮 ��ଠ�쭮
*/

int StartAkinator(const char* dataFName = "data.bts") {
	//setlocale(LC_ALL, "Russian");

	printf("����㧪�... ");

	FILE* dataFile = GetDataFile(dataFName);
	if (dataFile == NULL) {
		printf("\n\n�訡�� �� ����㧪� ������: ���������� ������ 䠩� � ����묨\n");
		return 1;
	}

	int err = 0;
	tree_t dataTree = GetDataTree(dataFile, &err);
	fclose(dataFile);
	if (err != 0) {
		printf("\n\n�訡�� �� ����㧪� ������: ���������� ������ �����\n");
		return 2;
	}
	

	printf("\n\n");
	printf("���� ���������� � �������! ������� �।���, � � ���஡�� ��� �⣠����.\n");
	printf("� ��� ��訢��� ⥡� ������騥 ������, � �� �⢥砩 \"��\" ��� \"���\".\n");

	int repeat = ANSWER_YES;
	while (repeat==ANSWER_YES) {
		printf("�᫨ ��⮢, ����� enter.\n");
		getchar();

		node_t* ansNode = NULL;
		int guessed = AkinatorCycle(dataTree.root, ansNode);
		printf("\n");

		if (!guessed) {
			if (AddQuestion(&dataTree, ansNode, dataFName) != 0) {
				printf("�訡�� �� ���������� ������ ᫮��.\n");
				return 3;
			}
			else {
				printf("����� ���������.\n");
			}
		}

		printf("��ࠥ� ��?\n");
		repeat = GetYesOrNo();
		printf("\n\n");
	}

	printf("����!");

	return 0;
}

int main() {

	int err = StartAkinator();

	fseek(stdin, 0, SEEK_END);
	getchar();

	return err;
}