#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS 100

void reverse(char str[]) {
    int i = 0, j = strlen(str) - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

void strToBigInt(char str[], int num[]) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        num[i] = str[len - 1 - i] - '0';
    }
}

void bigIntToStr(int num[], char str[]) {
    int i = MAX_DIGITS - 1;
    while (i > 0 && num[i] == 0) {
        i--;
    }
    int j = 0;
    while (i >= 0) {
        str[j] = num[i] + '0';
        i--;
        j++;
    }
    str[j] = '\0';
    reverse(str);
}

void addBigIntegers(int a[], int b[], int result[]) {
    int carry = 0;
    for (int i = 0; i < MAX_DIGITS; i++) {
        int sum = a[i] + b[i] + carry;
        result[i] = sum % 10;
        carry = sum / 10;
    }
}

void subtractBigIntegers(int a[], int b[], int result[]) {
    int borrow = 0;
    for (int i = 0; i < MAX_DIGITS; i++) {
        int diff = a[i] - b[i] - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[i] = diff;
    }
}

void printBigInteger(int num[], int isNegative) {
    if (isNegative) {
        printf("-");
    }
    int leadingZeros = 1;
    for (int i = MAX_DIGITS - 1; i >= 0; i--) {
        if (num[i] != 0) {
            leadingZeros = 0;
        }
        if (!leadingZeros) {
            printf("%d", num[i]);
        }
    }
    if (leadingZeros) {
        printf("0");
    }
    printf("\n");
}

int compareBigIntegers(int a[], int b[]) {
    for (int i = MAX_DIGITS - 1; i >= 0; i--) {
        if (a[i] > b[i]) {
            return 1;
        } else if (a[i] < b[i]) {
            return -1;
        }
    }
    return 0;
}

int main() {
    char num1Str[MAX_DIGITS + 1];
    char num2Str[MAX_DIGITS + 1];
    char operator;
    int num1[MAX_DIGITS] = {0};
    int num2[MAX_DIGITS] = {0};
    int result[MAX_DIGITS] = {0};

    printf("请输入第一个大整数: ");
    scanf("%s", num1Str);
    printf("请输入 + 或 - 号: ");
    scanf(" %c", &operator);
    printf("请输入第二个大整数: ");
    scanf("%s", num2Str);

    strToBigInt(num1Str, num1);
    strToBigInt(num2Str, num2);

    int isResultNegative = 0;
    if (operator == '+') {
        addBigIntegers(num1, num2, result);
        printf("加法结果: ");
    } else if (operator == '-') {
        int compareResult = compareBigIntegers(num1, num2);
        if (compareResult == 0) {
            printf("减法结果: 0\n");
            return 0;
        } else if (compareResult < 0) {
            subtractBigIntegers(num2, num1, result);
            isResultNegative = 1;
        } else {
            subtractBigIntegers(num1, num2, result);
        }
        printf("减法结果: ");
    } else {
        printf("无效的操作符\n");
        return 1;
    }

    printBigInteger(result, isResultNegative);

    return 0;
}
