#include <stdio.h>

int foo(int a, int b) {
    if (a < b) {
        return -1;
    } else {
        return 1;
    }
}

int main() {
    int number1, number2;
    scanf("%d", &number1);
    scanf("%d", &number2);
    int output = foo(number1, number2);
    return output;
}
