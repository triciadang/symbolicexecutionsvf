#include <stdio.h>

int foo(int n) {
    if (n == 3) {
        return 0;
    } else {
        return - 1;
    }
}

int main() {
    int number;
    scanf("%d", &number);
    int output = foo(number);
    return output;
}
