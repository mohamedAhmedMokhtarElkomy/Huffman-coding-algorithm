//Name: Mohamed Ahmed Mokhtar
//ID: 18105910
//name of input file for compress is: input.txt
//name of output file of compress is: output.dat
//name of input file for decompress is: output.dat
//name of output file for decompress is: output.txt
//codes.dat conatain number of extra bits used in compression and variable length table

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string.h>
#include <math.h>

//used for creating Huffman tree
struct HuffmanTreeNode {

    int frequency;
    unsigned char character;
    HuffmanTreeNode *right;
    HuffmanTreeNode *left;

};

//used for creating sorted list for HuffmanTreeNode
struct HuffmanLinkedListNode {

    HuffmanTreeNode *huffmanNode;
    HuffmanLinkedListNode *next;
};

struct IntLinkedListNode{

    int data;
    IntLinkedListNode *next;
};

//used for storing variable length (Binary)
struct Stack {

    IntLinkedListNode *top;
    int size;

    Stack() {
        top = NULL;
        size = 0;
    }

};

//store variable length code for each char
struct VariableLengthNode {

    int *variableLength;
    int size;
    char character;
};

//list contain variable length of each char
//sorted from least size
struct VariableLengthLinkedList {

    VariableLengthNode *variableLengthNode;
    VariableLengthLinkedList *next;
};

HuffmanTreeNode *CreateHuffmanNode(int frequency, char character, HuffmanTreeNode *left, HuffmanTreeNode *right) {

    HuffmanTreeNode *newNode = (HuffmanTreeNode *) malloc(sizeof(HuffmanTreeNode));

    newNode->frequency = frequency;
    newNode->character = character;
    newNode->right = right;
    newNode->left = left;

    return newNode;
}

//create Huffman node for the lowest frequency in the ascii array
HuffmanLinkedListNode *CreateMinFrequencyNode(int ascii[]) {

    int minIndex = 0;

    for (int i = 0; i < 256; i++) {

        if (ascii[i] > 0) {

            if (minIndex == 0)
                minIndex = i;
            else if (ascii[i] < ascii[minIndex])
                minIndex = i;
        }
    }

    //all nodes frequency are stored
    if (minIndex == 0)
        return NULL;
    else {
        HuffmanLinkedListNode *newNode = (HuffmanLinkedListNode *) malloc(sizeof(HuffmanLinkedListNode));
        newNode->huffmanNode = CreateHuffmanNode(ascii[minIndex], minIndex, NULL, NULL);
        newNode->next = NULL;

        ascii[minIndex] = 0; //to avoid using it again
        return newNode;
    }
}

//create sorted linked list from lowest frequency to the highest one
HuffmanLinkedListNode *CreateSortedLinkedList(int ascii[]) {

    HuffmanLinkedListNode *headNode;
    headNode = CreateMinFrequencyNode(ascii);

    HuffmanLinkedListNode *currentListNode = headNode;

    //if all frequency are stored CreateMinFrequency returns NULL
    while (currentListNode != NULL) {

        currentListNode->next = CreateMinFrequencyNode(ascii);
        currentListNode = currentListNode->next;
    }

    return headNode;
}

//this function use sorted linked list to make huffman tree
//return head of huffman tree
HuffmanTreeNode *BuildHuffmanTree(HuffmanLinkedListNode **parentNode) {

    //currentNode used for iteration
    HuffmanLinkedListNode *currentNode = *parentNode;

    //if the list has only one node end loop
    while (currentNode->next != NULL) {

        //newNode has the sum of the lowest 2 frequency in the sorted linked list
        //and point to both of them
        //and has NULL character
        HuffmanLinkedListNode *newNode = (HuffmanLinkedListNode *) malloc(sizeof(HuffmanLinkedListNode));
        newNode->next = NULL;
        newNode->huffmanNode = CreateHuffmanNode(
                currentNode->huffmanNode->frequency + currentNode->next->huffmanNode->frequency, NULL,
                currentNode->huffmanNode, currentNode->next->huffmanNode);

        //to make the current node the second node
        currentNode = currentNode->next;
        //loop to sort the new node in the sorted linked list
        while (currentNode->next != NULL &&
               currentNode->next->huffmanNode->frequency <= newNode->huffmanNode->frequency)
            currentNode = currentNode->next;

        //if it reached the end of the list then the frequency of the new node is bigger than the frequency of all nodes in the list
        if (currentNode->next == NULL)
            currentNode->next = newNode;
        else {
            newNode->next = currentNode->next;
            currentNode->next = newNode;
        }

        //remove the the 2 nodes used (first two nodes)
        for (int i = 1; i <= 2; i++) {
            currentNode = *parentNode;
            HuffmanLinkedListNode *tempNode = currentNode;
            currentNode = currentNode->next;
            *parentNode = currentNode;
            free(tempNode);
        }
    }

    return currentNode->huffmanNode;

}

HuffmanTreeNode *CreateHuffmanTree(char inputFileName[]) {

    FILE *inputFile = fopen(inputFileName, "r");

    if (inputFile == NULL) {
        printf("file not found");
        exit(0);
    }

    unsigned char character = fgetc(inputFile);
    int ascii[256];

    //initialize all frequencys in array by 0
    for (int i = 0; i < 256; i++)
        ascii[i] = 0;

    HuffmanLinkedListNode *linkedListParentNode = NULL;

    while (character != 255) {

        ascii[character]++;
        character = fgetc(inputFile);
    }

    linkedListParentNode = CreateSortedLinkedList(ascii);
    HuffmanTreeNode *huffmanHeadNode = BuildHuffmanTree(&linkedListParentNode);

    free(linkedListParentNode);
    fclose(inputFile);

    return huffmanHeadNode;
}

IntLinkedListNode *CreateIntLinkedListNode(int data, IntLinkedListNode *next) {

    IntLinkedListNode *intLinkedListNode = (IntLinkedListNode *) malloc(sizeof(IntLinkedListNode));

    intLinkedListNode->data = data;
    intLinkedListNode->next = next;

    return intLinkedListNode;
}

void Push(Stack *stack, int data) {

    if (stack->top == NULL)
        stack->top = CreateIntLinkedListNode(data, NULL);
    else {

        IntLinkedListNode *newNode = CreateIntLinkedListNode(data, stack->top);
        stack->top = newNode;
    }

    stack->size++;
}

void Pop(Stack *stack) {

    if (stack->top == NULL)
        return;
    else {
        IntLinkedListNode *tempNode = stack->top;
        stack->top = stack->top->next;
        free(tempNode);
        stack->size--;
    }
}

//used recursion to print from the end of stack
//at the beginning, node is the top of the stack
void CreateVariableLengthArray(IntLinkedListNode *node, VariableLengthNode *variableLengthNode) {

    int static index = 0;

    if (node->next == NULL) {

        variableLengthNode->size = index + 1;
        variableLengthNode->variableLength = (int *) malloc(sizeof(int) * index + 1);
        variableLengthNode->variableLength[0] = node->data;
        index--;

    } else {
        index++;
        CreateVariableLengthArray(node->next, variableLengthNode);
        variableLengthNode->variableLength[variableLengthNode->size - index - 1] = node->data;
        index--;
    }
    //reset index to 0 again for the next use
    if (index == -1)
        index = 0;
}

void AddVariableLengthNodeToList(VariableLengthLinkedList **headnode, VariableLengthNode *variableLengthNode) {

    VariableLengthLinkedList *newNode;
    newNode = (VariableLengthLinkedList *) malloc(sizeof(VariableLengthLinkedList));
    newNode->variableLengthNode = variableLengthNode;
    newNode->next = NULL;

    VariableLengthLinkedList *currentNode = *headnode;

    if (*headnode == NULL)
        *headnode = newNode;

    else if (currentNode->variableLengthNode->size > variableLengthNode->size) {

        newNode->next = *headnode;
        *headnode = newNode;
    }
        //sort the new node
    else {

        while (currentNode->next != NULL && currentNode->next->variableLengthNode->size <= variableLengthNode->size)
            currentNode = currentNode->next;

        //if it reached the end of the list then the frequency of the new node is bigger than the frequency of all nodes in the list
        if (currentNode->next == NULL)
            currentNode->next = newNode;
        else {
            newNode->next = currentNode->next;
            currentNode->next = newNode;
        }
    }
}

//create list of nodes where each node has an array of variable length
void CreateVariableLengthList(HuffmanTreeNode *node, Stack *stack, VariableLengthLinkedList **headnode) {

    VariableLengthNode *variableLengthNode;
    variableLengthNode = (VariableLengthNode *) malloc(sizeof(variableLengthNode));

    if (node->right == NULL && node->left == NULL) {

        CreateVariableLengthArray(stack->top, variableLengthNode);
        variableLengthNode->character = node->character;
        AddVariableLengthNodeToList(&*headnode, variableLengthNode);
    } else {
        if (node->right != NULL) {
            Push(stack, 1);
            CreateVariableLengthList(node->right, stack, &*headnode);
        }
        if (node->left != NULL) {
            Push(stack, 0);
            CreateVariableLengthList(node->left, stack, &*headnode);
        }
    }

    Pop(stack);
}

//%d this number to know how many extra bits added
//"%c %d array", character, size of Variable length code, variable length code
void PrintVariableLengthTable(VariableLengthLinkedList *node, int extraBits) {

    FILE *codes = fopen("codes.dat", "w");
    fprintf(codes, "%d\n", extraBits);

    while (node != NULL) {

        fprintf(codes, "%c\t%d\t", node->variableLengthNode->character, node->variableLengthNode->size);
        for (int i = 0; i < node->variableLengthNode->size; i++) {
            fprintf(codes, "%d ", node->variableLengthNode->variableLength[i]);
        }
        fprintf(codes, "\n");
        node = node->next;
    }
    fclose(codes);

}

void ReadVariableLength(VariableLengthLinkedList **node, int *extraBits) {

    FILE *codes = fopen("codes.dat", "r");
    if (codes == NULL) {
        printf("codes.dat file not found");
        return;
    }
    VariableLengthLinkedList *currentNode = *node;
    currentNode->variableLengthNode = (VariableLengthNode *) malloc(sizeof(VariableLengthNode));

    int flag = 1;

    fscanf(codes, "%d\n", &*extraBits);

    char character = getc(codes);
    currentNode->variableLengthNode->character = character;
    getc(codes);
    currentNode->variableLengthNode->size = getc(codes) - 48;
    getc(codes);
    currentNode->variableLengthNode->variableLength = (int *) malloc(
            sizeof(int) * currentNode->variableLengthNode->size);
    int *array = (int *) malloc(sizeof(int) * currentNode->variableLengthNode->size);
    for (int i = 0; i < currentNode->variableLengthNode->size; i++) {
        array[i] = getc(codes) - 48;
        getc(codes);
    }
    currentNode->variableLengthNode->variableLength = array;
    getc(codes);
    *node = currentNode;
    character = getc(codes);

    while (character != EOF) {

        VariableLengthLinkedList *newNode = (VariableLengthLinkedList *) malloc(sizeof(VariableLengthLinkedList));
        newNode->variableLengthNode = (VariableLengthNode *) malloc(sizeof(VariableLengthNode));
        newNode->variableLengthNode->character = character;
        getc(codes);
        newNode->variableLengthNode->size = getc(codes) - 48;
        getc(codes);
        newNode->variableLengthNode->variableLength = (int *) malloc(sizeof(int) * newNode->variableLengthNode->size);

        for (int i = 0; i < newNode->variableLengthNode->size; i++) {
            newNode->variableLengthNode->variableLength[i] = getc(codes) - 48;
            getc(codes);
        }
        getc(codes);
        currentNode->next = newNode;
        newNode->next = NULL;
        currentNode = currentNode->next;
        character = getc(codes);

    }
}

//return node of the biggest variable length code
int SizeOfBiggestVL(VariableLengthLinkedList *node) {

    while (node->next != NULL)
        node = node->next;
    return node->variableLengthNode->size;
}

//convert 8 bits to char
unsigned char Convert8BitToChar(int binary8Bit[]) {

    unsigned char charAsciiCode = 0;

    for (int i = 7; i >= 0; i--)
        if (binary8Bit[i] == 1)
            charAsciiCode += pow(2, 7 - i);

    return charAsciiCode;

}

//array carry binary numbers
//index show the number of indices reserved in the array

void ConvertCharTo8Bit(unsigned char character, int array[], int *index) {

    for (int i = (*index + 8); i > *index; i--) {

        if (character % 2 == 1)
            array[i] = 1;
        else
            array[i] = 0;

        character = character / 2;
    }

    *index += 8;
}

int Compress(char inputFileName[], VariableLengthLinkedList *headNode) {


    FILE *inputFile = fopen(inputFileName, "r");

    if (inputFile == NULL) {
        printf("file not found");
        exit(0);
    }

    FILE *outputFile = fopen("output.dat", "w");

    VariableLengthLinkedList *currentNode;

    //bit8 is an array has 8 indices for 8 bits
    //bit8Index is the last index in the bit8 array
    int bit8[8];
    int bit8Index = -1;

    int signedChar = fgetc(inputFile);
    unsigned char character = signedChar;

    while (signedChar != EOF) {

        currentNode = headNode;

        while (currentNode->variableLengthNode->character != character)
            currentNode = currentNode->next;

        for (int j = 0; j < currentNode->variableLengthNode->size; j++) {

            bit8Index++;
            bit8[bit8Index] = currentNode->variableLengthNode->variableLength[j];

            if (bit8Index == 7) {

                fprintf(outputFile, "%c", Convert8BitToChar(bit8));
                bit8Index = -1;
            }
        }

        signedChar = fgetc(inputFile);
        character = signedChar;
    }

    //if EOF reached and still there bits in the bit8 array
    //then add zero until it become 8 bits
    if (bit8Index > -1) {

        for (int i = bit8Index + 1; i <= 7; i++)
            bit8[i] = 0;

        fprintf(outputFile, "%c", Convert8BitToChar(bit8));
    }
    fclose(outputFile);
    fclose(inputFile);

    //return number of bits added
    return (7 - bit8Index);
}

//use for deletion of bits
void ShiftLeft(int array[], int numberOfTimes, int size) {

    for (int j = 1; j <= numberOfTimes; j++)
        for (int i = 0; i < ((size - 1) - j); i++)
            array[i] = array[i + 1];
}

void Decompress(char inputFileName[], VariableLengthLinkedList *variableLengthListHeadNode, int extraBits) {

    FILE *inputFile = fopen(inputFileName, "r");

    if (inputFile == NULL) {
        printf("file not found");
        exit(0);
    }

    FILE *outputFile = fopen("output.txt", "w");

    unsigned char character;
    VariableLengthLinkedList *currentNode;

    int size =
            SizeOfBiggestVL(variableLengthListHeadNode) + (8 - (SizeOfBiggestVL(variableLengthListHeadNode) % 8)) + 8;
    int *binaryArray = (int *) malloc(sizeof(int) * size);
    int indexOfBinaryArray = -1;

    int nextChar = fgetc(inputFile);

    for (int k = 0; k < size; ++k) {
        binaryArray[k] = 0;
    }

    do {
        character = nextChar;
        nextChar = fgetc(inputFile);

        currentNode = variableLengthListHeadNode;
        int flag = 1;
        ConvertCharTo8Bit(character, binaryArray, &indexOfBinaryArray);

        while (currentNode != NULL && indexOfBinaryArray != -1) {
            if ((indexOfBinaryArray + 1) >= currentNode->variableLengthNode->size) {
                for (int i = 0; i < currentNode->variableLengthNode->size; i++)
                    if (currentNode->variableLengthNode->variableLength[i] != binaryArray[i])
                        flag = 0;
                if (currentNode->next != NULL && flag == 0) {
                    currentNode = currentNode->next;
                    flag = 1;
                } else if (flag == 1) {

                    if (nextChar == EOF)
                        indexOfBinaryArray -= extraBits;
                    ShiftLeft(binaryArray, currentNode->variableLengthNode->size, size);
                    indexOfBinaryArray -= currentNode->variableLengthNode->size;
                    char x = currentNode->variableLengthNode->character;
                    fprintf(outputFile, "%c", currentNode->variableLengthNode->character);
                    currentNode = variableLengthListHeadNode;
                }
            } else
                currentNode = NULL;
        }
    } while (nextChar != EOF);

    fclose(outputFile);
}

int LeafSum(HuffmanTreeNode *node) {

    int static leafSum = 0;

    if (!node)
        return 0;

    if (!node->left && !node->right)
        return (leafSum += node->frequency);

    leafSum = LeafSum(node->left);
    leafSum = LeafSum(node->right);
}

int searchForNode(HuffmanTreeNode *node, unsigned char character) {

    int static sum = 0;

    if (!node)
        return sum;

    if (!node->left && !node->right)
        if (node->character == character)
            return node->frequency;

    sum = searchForNode(node->left, character);
    sum = searchForNode(node->right, character);
    return sum;
}

void PrintRatioBetweenCompressedOriginal(VariableLengthLinkedList *node, HuffmanTreeNode *huffmanTreeNode) {

    int sumBeforCompression;
    sumBeforCompression = LeafSum(huffmanTreeNode);

    int sumAfterCompression = 0;

    while (node != NULL) {
        sumAfterCompression += (node->variableLengthNode->size *
                                searchForNode(huffmanTreeNode, node->variableLengthNode->character));
        node = node->next;
    }

    sumBeforCompression *= 8;


    printf("original size = %d bits\n", sumBeforCompression);
    printf("compressed size = %d bits\n", sumAfterCompression);
    printf("ration between original and compressed: original = %f of the compressed file\n",
           ((float) sumBeforCompression / sumAfterCompression));
}

int main() {

    char inputFileName[] = "input.txt";
    char compressedInputFileName[] = "output.dat";

    printf("Enter number of your choice\n"
           "1-compress\n"
           "2-decompress(must be compressed first by this program)\n"
           "3-both\n");
    int choice;
    scanf("%d", &choice);

    printf("This may take few seconds...\n");

    int extraBits;

    //parent node for list contains all characters with its variable length
    VariableLengthLinkedList *variableLengthLinkedListNode;
    variableLengthLinkedListNode = (VariableLengthLinkedList *) malloc(sizeof(VariableLengthLinkedList));

    if (choice == 1) {

        variableLengthLinkedListNode = NULL;

        HuffmanTreeNode *huffmanHeadNode = CreateHuffmanTree(inputFileName);

        //this stack store the steps moved inside huffman tree
        Stack *variableLengthSteps = (Stack *) malloc(sizeof(Stack));
        variableLengthSteps->top = NULL;

        CreateVariableLengthList(huffmanHeadNode, variableLengthSteps, &variableLengthLinkedListNode);

        free(variableLengthSteps);

        extraBits = Compress(inputFileName, variableLengthLinkedListNode);
        PrintVariableLengthTable(variableLengthLinkedListNode, extraBits);
        PrintRatioBetweenCompressedOriginal(variableLengthLinkedListNode, huffmanHeadNode);

    } else if (choice == 2) {

        ReadVariableLength(&variableLengthLinkedListNode, &extraBits);
        Decompress(compressedInputFileName, variableLengthLinkedListNode, extraBits);
    } else if (choice == 3) {

        HuffmanTreeNode *huffmanHeadNode = CreateHuffmanTree(inputFileName);

        //this stack store the steps moved inside huffman tree
        Stack *variableLengthSteps = (Stack *) malloc(sizeof(Stack));
        variableLengthSteps->top = NULL;

        //parent node for list contains all characters with its variable length
        VariableLengthLinkedList *variableLengthLinkedListNode;
        variableLengthLinkedListNode = (VariableLengthLinkedList *) malloc(sizeof(VariableLengthLinkedList));
        variableLengthLinkedListNode = NULL;

        CreateVariableLengthList(huffmanHeadNode, variableLengthSteps, &variableLengthLinkedListNode);

        free(variableLengthSteps);

        int extraBits = Compress(inputFileName, variableLengthLinkedListNode);

        PrintVariableLengthTable(variableLengthLinkedListNode, extraBits);

        Decompress(compressedInputFileName, variableLengthLinkedListNode, extraBits);

    } else {
        printf("Wrong choice");
        return 0;
    }
    return 1;
}