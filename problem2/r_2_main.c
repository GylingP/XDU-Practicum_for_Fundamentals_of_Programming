#include <stdio.h>

#define MAX_SIZE 100

int stack[MAX_SIZE];
int top = -1;

void push(int element) {
    if (top == MAX_SIZE - 1) {
        printf("��ջ�������޷���ջ��\n");
        return;
    }
    stack[++top] = element;
}

int pop() {
    if (top == -1) {
        printf("��ջΪ�գ��޷���ջ��\n");
        return -1;
    }
    return stack[top--];
}

int peek() {
    if (top == -1) {
        printf("��ջΪ�ա�\n");
        return -1;
    }
    return stack[top];
}

void displayStack() {
    if (top == -1) {
        printf("��ջΪ�ա�\n");
        return;
    }
    printf("��ջ�е�Ԫ��Ϊ��");
    for (int i = 0; i <= top; i++) {
        printf("%d ", stack[i]);
    }
    printf("\n");
}

int main() {
    int option, element;

    do {
        printf("\nѡ�������\n");
        printf("1. ��ջ\n");
        printf("2. ��ջ\n");
        printf("3. ����ջ��Ԫ��\n");
        printf("4. չʾջ��Ԫ��\n");
        printf("0. �˳�����\n");
        printf("������ѡ�");
        scanf("%d", &option);

        switch (option) {
            case 1:
                printf("������Ҫ��ջ��Ԫ�أ�");
                scanf("%d", &element);
                push(element);
                displayStack();
                break;
            case 2:
                element = pop();
                if (element != -1) {
                    printf("��ջԪ�أ�%d\n", element);
                }
                displayStack();
                break;
            case 3:
                element = peek();
                if (element != -1) {
                    printf("ջ��Ԫ�أ�%d\n", element);
                }
                break;
            case 4:
                displayStack();
                break;
            case 0:
                printf("�������˳���\n");
                break;
            default:
                printf("��Ч��ѡ����������롣\n");
        }
    } while (option != 0);

    return 0;
}