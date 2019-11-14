//
// Created by boristab on 08.11.2019.
//

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include "fileRead.h"

enum errors {
    SUCCESS = 0,
    FILE_OPEN_FAILED = 1,
    NOT_FOUND_TREE_IN_FILE = 2,
};

const char dumpFilePath[FILENAME_MAX] = "../TreeDumpFile.txt";

int spaceN (const char *buffer);

template <class elemType>
class Node {
public:
    Node <elemType> *leftChild = nullptr;
    Node <elemType> *rightChild = nullptr;
    elemType value = {};

    explicit Node(elemType value) {
        this->value = value;
    }
};


template <class elemType>
class Tree {
private:
    Node <elemType> *root = nullptr;

    void printNodeInit(std::ofstream *dumpFile, Node <elemType> *node) {
        assert(node);
        assert(dumpFile);

        *dumpFile << "node_" << node << " [shape=record, label=\" { "<< node
            << " | Val: " << node->value
            << " | { left: " << node->leftChild
            << " | right: " << node->rightChild << " } } \"];\n";

        if (node->leftChild) printNodeInit(dumpFile, node->leftChild);
        if (node->rightChild) printNodeInit(dumpFile, node->rightChild);
    }

    void printNodeRel(std::ofstream *dumpFile, Node <elemType> *node) {
        if (node->leftChild) *dumpFile << "node_" << node << "-- node_" << node->leftChild << ";\n";
        if (node->rightChild) *dumpFile << "node_" << node << "-- node_" << node->rightChild << ";\n";

        if (node->leftChild) printNodeRel(dumpFile, node->leftChild);
        if (node->rightChild) printNodeRel(dumpFile, node->rightChild);
    }

    void saveNode(std::ofstream *outFile, Node <elemType> *node) {
        assert(outFile);
        assert(node);

        *outFile << "{ \"" << node->value << "\" ";

        if (!node->leftChild && !node->rightChild) {
            *outFile << "} ";
            return;
        }

        if (node->leftChild) saveNode(outFile, node->leftChild);
        else *outFile << "$ ";

        if (node->rightChild) saveNode(outFile, node->rightChild);
        else *outFile << "$ ";

        *outFile << "} ";
    }

    void writeNode(char **buffer, Node <elemType> *node) {
        if (**buffer == '$') (*buffer) += 2;
        else if (**buffer == '{'){
            (*buffer) += 2 + spaceN((*buffer) + 1);
            *(strchr(*buffer, '"')) = '\0';

            insertLeft(node, *buffer);
            (*buffer) += strlen(*buffer) + 2;
            writeNode(buffer, node->leftChild);
        }

        if (**buffer == '$') (*buffer) += 2;
        else if (**buffer == '{'){
            (*buffer) += 2 + spaceN((*buffer) + 1);
            *(strchr(*buffer, '"')) = '\0';

            insertRight(node, *buffer);
            (*buffer) += strlen(*buffer) + 2;
            writeNode(buffer, node->rightChild);
        }

        if (**buffer == '}') {
            (*buffer) += 2;
            return;
        }
    }

public:
    explicit Tree(elemType val) {
        auto *node = newNode(val);

        root = node;
    }

    Tree(char load, const char *inPath) {
        assert(load == 'L');


        int fileSize = getFileSize(inPath);

        char *buffer = new char[fileSize] ();
        char *bufferStart = buffer;
        readFile(inPath, buffer, fileSize);

        if (*buffer != '{') {
            printf("Error: No tree in file");
            exit(NOT_FOUND_TREE_IN_FILE);
        }

        if (*(buffer + spaceN(buffer + 1)) == '}') {
            printf("Error: tree is empty");
            exit(NOT_FOUND_TREE_IN_FILE);
        }

        buffer += 2 + spaceN(buffer + 1);
        *(strchr(buffer + 1, '"')) = '\0';
        auto *node = newNode(buffer);
        root = node;
        buffer += strlen(buffer);
        buffer += spaceN(buffer + 1) + 1;

        writeNode(&buffer, root);
    }

    Node <elemType> *newNode(elemType val) {
        return new Node <elemType> (val);
    }

    void insertNodeLeft(Node <elemType> *parent, Node <elemType> *node) {
        parent->leftChild = node;
    }

    void insertNodeRight(Node <elemType> *parent, Node <elemType> *node) {
        parent->rightChild = node;
    }

    void insertLeft(Node <elemType> *parentNode, elemType val) {
        auto *node = newNode(val);

        parentNode->leftChild = node;
    }

    void insertRight(Node <elemType> *parentNode, elemType val) {
        auto *node = newNode(val);

        parentNode->rightChild = node;
    }

    void deleteSubTree(Node <elemType> *node) {
        if (!node) return;

        if (node->leftChild) {
            deleteSubTree(node->leftChild);
        } else if (node->rightChild) {
            deleteSubTree(node->rightChild);
        } else {
            delete node;
        }
    }

    Node <elemType> *getRoot() {
        return root;
    }

    Node <elemType> *getLeftChild(Node <elemType> *node) {
        assert(node);

        return node->leftChild;
    }

    Node <elemType> *getRightChild(Node <elemType> *node) {
        assert(node);

        return node->rightChild;
    }

    elemType getVal(Node <elemType> *node) {
        return node->value;
    }

    Node <elemType> *findElem(Node <elemType> *subtree, elemType val) {
        assert(subtree);

        if(subtree->value == val) return subtree;
        if (subtree->leftChild) findElem(subtree->leftChild);
        if (subtree->rightChild) findElem(subtree->rightChild);
    }

    void changeVal(Node <elemType> *node, elemType val) {
        node->value = val;
    }

    void dump() {
        std::ofstream  dumpFile (dumpFilePath);
        if (!dumpFile) {
            printf("File isn't open\n");
            exit(FILE_OPEN_FAILED);
        }

        dumpFile << "graph G{\n";

        if (root) {
            Node <elemType> *currentNode = root;
            printNodeInit(&dumpFile, root);
            printNodeRel(&dumpFile, root);
        }

        dumpFile << "}\n";

        dumpFile.close();

        char dotCommand[FILENAME_MAX] = "";
        sprintf(dotCommand, "dot -Tpng -O %s", dumpFilePath);
        std::system(dotCommand);
    }

    void saveTo(const char *path) {
        std::ofstream outFile (path);

        if (!outFile) {
            printf("File isn't open\n");
            exit(FILE_OPEN_FAILED);
        }

        saveNode(&outFile, root);
        outFile.close();
    }
};

int spaceN (const char *buffer) {
    int count = 0;

    while (*buffer + count == ' ') {
        count++;
    }

    return count;
}
