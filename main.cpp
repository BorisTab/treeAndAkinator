#include <iostream>
#include "tree.h"

const short MaxTextLen = 512;
const char *akinatorPath = "../akinator.txt";

void printAndSay(const char *text);

void tryAgain(char *answer);

void ask(Tree < const char *> *akinator, Node <const char *> *node);

void finalAnswer(Tree < const char *> *akinator, Node <const char *> *node);

void getAnswer(char *answer);

bool isYes(char *answer);

bool isNo(char *answer);

void newAnswer(Tree < const char *> *akinator, Node <const char *> *node);

void saveNewAnswer(Tree < const char *> *akinator, Node <const char *> *node, char *newQuestion, char *newAnswer);

bool checkNewAnswer(Tree < const char *> *akinator, char *newAnswer, char *newQuestion);

void showTree(Tree < const char *> *akinator, Node <const char *> *node, int level = 0);

int akinator3000(Tree <const char *> *akinator);

bool newAnswerDuplicateChecker(Tree < const char *> *akinator, Node <const char *> *node, char *newAnswer);

int printAnswers(Node <const char *> *node, int count = 1);

void compareTwoAnswers(Tree < const char *> *akinator);

int getNodeByAnswerN(Node <const char *> *node, Node <const char *> *outNode, int number, int count = 1);

int main() {
    printf("                                                                                                                            \n"
           "                          ,,                                                                                                \n"
           "      db      `7MM        db                         mm                                                                     \n"
           "     ;MM:       MM                                   MM                                                                     \n"
           "    ,V^MM.      MM  ,MP'`7MM  `7MMpMMMb.   ,6\"Yb.  mmMMmm   ,pW\"Wq.  `7Mb,od8      pd\"\"b.   ,pP\"\"Yq.   ,pP\"\"Yq.   ,pP\"\"Yq.  \n"
           "   ,M  `MM      MM ;Y     MM    MM    MM  8)   MM    MM    6W'   `Wb   MM' \"'     (O)  `8b 6W'    `Wb 6W'    `Wb 6W'    `Wb \n"
           "   AbmmmqMA     MM;Mm     MM    MM    MM   ,pm9MM    MM    8M     M8   MM              ,89 8M      M8 8M      M8 8M      M8 \n"
           "  A'     VML    MM `Mb.   MM    MM    MM  8M   MM    MM    YA.   ,A9   MM            \"\"Yb. YA.    ,A9 YA.    ,A9 YA.    ,A9 \n"
           ".AMA.   .AMMA..JMML. YA..JMML..JMML  JMML.`Moo9^Yo.  `Mbmo  `Ybmd9'  .JMML.             88  `Ybmmd9'   `Ybmmd9'   `Ybmmd9'  \n"
           "                                                                                  (O)  .M'                                  \n"
           "                                                                                   bmmmd'                                  \n\n\n");
    printAndSay("Давай поиграем?");
    printf("\"да\" или \"нет\"\n");

    Tree <const char *> akinator ('L', akinatorPath);

    char answer[10] = "";
    getAnswer(answer);
    while (isYes(answer)) {
        akinator3000(&akinator);

        printAndSay("Хочешь сыграть еще?");
        getAnswer(answer);
    }

    if (isNo(answer)) {
        printAndSay("Выбере, что ты хочешь");
        printf("1 Поиграть\n"
               "2 Посмотреть дерево\n"
               "3 Выйти\n");

        scanf("%s", answer);
        switch (*answer) {
            case '1': akinator3000(&akinator);
                break;
            case '2': showTree(&akinator, akinator.getRoot());
                break;
            case '3': printAndSay("Тогда я пошел");
                break;
            default: printAndSay("Не нажимаешь цифорки, тогда я пошел");
                break;
        }
    }

    printAndSay("Пока");
    akinator.~Tree();
    return 0;
}

int akinator3000(Tree <const char *> *akinator) {
    printAndSay("Загадывай вкусняшку");

    Node <const char *> *currentNode = akinator->getRoot();
    ask(akinator, currentNode);
}

void printAndSay(const char *text) {
    printf("%s\n", text);
    char command[MaxTextLen] = "";
    sprintf(command, "../audio.sh \"%s\"", text);
    system(command);
}

void ask(Tree < const char *> *akinator, Node <const char *> *node) {
    if (!node->leftChild) {
        finalAnswer(akinator, node);

        return;
    }
    printAndSay(akinator->getVal(node));

    char answer[10] = "";
    getAnswer(answer);

    if (isNo(answer)) {
        ask(akinator, node->leftChild);
    } else if (isYes(answer)) {
        ask(akinator, node->rightChild);
    }
}

void tryAgain(char *answer) {
    while (!isYes(answer) && !isNo(answer)) {
        printAndSay("Не понял. Попробуй еще раз");
        scanf("%s", answer);
    }
}

void finalAnswer(Tree < const char *> *akinator, Node <const char *> *node) {
    printAndSay("И так мой ответ:");
    printAndSay(akinator->getVal(node));

    printAndSay("Я оказался прав?");
    char answer[10] = "";
    getAnswer(answer);

    if (isYes(answer)) {
        printAndSay("Я всегда прав!!!");
    } else if(isNo(answer)) {
        newAnswer(akinator, node);
    }
}

void getAnswer(char *answer) {
    scanf("%s", answer);
    tryAgain(answer);
}

bool isYes(char *answer) {
    return !strcmp(answer, "да");
}

bool isNo(char *answer) {
    return !strcmp(answer, "нет");
}

void newAnswer(Tree < const char *> *akinator, Node <const char *> *node) {
    printAndSay("Хочешь добавить новый ответ?");
    char answer[10] = "";
    getAnswer(answer);
    if (isYes(answer)) {

        char newAnswer[MaxTextLen] = "";
        char newQuestion[MaxTextLen] = "";

        if (!checkNewAnswer(akinator, newAnswer, newQuestion)) {
            saveNewAnswer(akinator, node, newQuestion, newAnswer);
        }

        akinator->saveTo(akinatorPath);
    } else if(isNo(answer)) {
        printAndSay("Очень жаль");
    }
}

void saveNewAnswer(Tree < const char *> *akinator, Node <const char *> *node, char *newQuestion, char *newAnswer) {
    akinator->insertLeft(node, node->value);
    akinator->insertRight(node, newAnswer);
    akinator->changeVal(node, newQuestion);
}

bool checkNewAnswer(Tree < const char *> *akinator, char *newAnswer, char *newQuestion) {
    printAndSay("Что ты загадывал?");
    printAndSay("Поставь пожалуйста ТОЧКУ в конце");
    std::cin.getline(newAnswer, MaxTextLen, '.');
    scanf("%*c");

    printAndSay("Чем отличается от моего ответа?");
    printAndSay("Не пиши, пожалуйста, фразы включающие частицу НЕ и поставь пожалуйста ТОЧКУ в конце");

    std::cin.getline(newQuestion, MaxTextLen, '.');
    scanf("%*c");
    sprintf(newQuestion, "%s?", newQuestion);

    char answer[10] = "";
    while (!isYes(answer)) {
        printAndSay("Проверь все ли верно:");
        printf("Вопрос: %s\n", newQuestion);
        printf("Ответ: %s\n", newAnswer);

        getAnswer(answer);
        if (isYes(answer)) {
            if (newAnswerDuplicateChecker(akinator, akinator->getRoot(), newAnswer)) {
                printAndSay("Извини, но такой ответ уже есть");
                return true;
            }

            printAndSay("Спасибо за пополнение моих знаний");
            return false;
        }
    }
}

void showTree(Tree < const char *> *akinator, Node <const char *> *node, int level) {
    for (int i = 0; i < level; i++) {
        printf("\t");
    }
    printf("%s\n", akinator->getVal(node));
    level++;
    if(node->leftChild) {
        showTree(akinator, node->leftChild, level);
    }

    if (node->rightChild) {
        showTree(akinator, node->rightChild, level);
    }
}

bool newAnswerDuplicateChecker(Tree < const char *> *akinator, Node <const char *> *node, char *newAnswer) {
    bool checker = false;
    if (!strcmp(newAnswer, node->value)) {
        return true;
    }

    if(node->leftChild) {
        checker = newAnswerDuplicateChecker(akinator, node->leftChild, newAnswer);
    }

    if (node->rightChild) {
        checker = newAnswerDuplicateChecker(akinator, node->rightChild, newAnswer);
    }
    return checker;
}
