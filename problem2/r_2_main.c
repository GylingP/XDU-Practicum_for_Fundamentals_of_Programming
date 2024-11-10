#include <stdio.h>

#define MAX_SIZE 100

int stack[MAX_SIZE];
int top = -1;

void push(int element) {
    if (top == MAX_SIZE - 1) {
        printf("堆栈已满，无法入栈。\n");
        return;
    }
    stack[++top] = element;
}

int pop() {
    if (top == -1) {
        printf("堆栈为空，无法出栈。\n");
        return -1;
    }
    return stack[top--];
}

int peek() {
    if (top == -1) {
        printf("堆栈为空。\n");
        return -1;
    }
    return stack[top];
}

void displayStack() {
    if (top == -1) {
        printf("堆栈为空。\n");
        return;
    }
    printf("堆栈中的元素为：");
    for (int i = 0; i <= top; i++) {
        printf("%d ", stack[i]);
    }
    printf("\n");
}

int main() {
    int option, element;

    do {
        printf("\n选择操作：\n");
        printf("1. 入栈\n");
        printf("2. 出栈\n");
        printf("3. 返回栈顶元素\n");
        printf("4. 展示栈中元素\n");
        printf("0. 退出程序\n");
        printf("请输入选项：");
        scanf("%d", &option);

        switch (option) {
            case 1:
                printf("请输入要入栈的元素：");
                scanf("%d", &element);
                push(element);
                displayStack();
                break;
            case 2:
                element = pop();
                if (element != -1) {
                    printf("出栈元素：%d\n", element);
                }
                displayStack();
                break;
            case 3:
                element = peek();
                if (element != -1) {
                    printf("栈顶元素：%d\n", element);
                }
                break;
            case 4:
                displayStack();
                break;
            case 0:
                printf("程序已退出。\n");
                break;
            default:
                printf("无效的选项，请重新输入。\n");
        }
    } while (option != 0);

    return 0;
}