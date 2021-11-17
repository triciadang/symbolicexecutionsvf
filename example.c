//void swap(char **p, char **q){
//  char* t = *p;
//       *p = *q;
//       *q = t;
//}
//int main(){
//      char a1, b1;
//      char *a = &a1;
//      char *b = &b1;
//      swap(&a,&b);
//}

//int foo(int n) {
//    if (n == 3) {
//        return n;
//    } else {
//        return n - 1;
//    }
//}
//
//int main() {
//    return foo(10);
//}

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
