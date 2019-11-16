#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
#include "btree_string.h"


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
*	����������, ����� ������������ ���� "��" ��� "���"
*
*	@return 1 - "��"; 0 - "���"
*/

int GetYesOrNo() {

	char ans[4] = "";

	while (true) {
		scanf("%3s", ans);

		if (strcmp(ans, "��") == 0) {
			return 1;
		}
		else if (strcmp(ans, "���") == 0) {
			return 0;
		}
		else {
			printf("�������� \"��\" ��� \"���\".\n");
		}
	}

}

int AkinatorCycle(node_t* curNode, node_t* ansNode) {
	assert(curNode!= NULL);

	if (NodeChildsCount(curNode) == 0) {
		printf("��� - %s\n", curNode->value);
		ansNode = curNode;

		printf("� ������?\n");

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

	printf("��������... ");

	FILE* dataFile = GetDataFile("data.bts");
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
	printf("����� ���������� � ��������! ������� �������, � � �������� ��� ��������.\n");
	printf("� ���� ���������� ���� ��������� �������, � �� ������� \"��\" ��� \"���\".\n");
	printf("���� �����, ����� enter.\n");
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