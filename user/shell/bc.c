// Shell Calculator
#include <shell.h>

typedef struct {
    const char *exp; // Pointer to the expression
    int err;         // Error flag
} expr_t;

enum Error {
    ERR_NONE, ERR_SYNTAX, ERR_DIVISION_BY_ZERO
};

int eval(expr_t *expr);

// Parse a number (decimal or hexadecimal)
int number(expr_t *expr) {
    int neg = 0;
    if (*(expr->exp) == '-') {
        neg = 1;
        expr->exp++;
    }

    int n = 0;
    if (*(expr->exp) == '0' && (expr->exp)[1] == 'x') {
        // Hexadecimal
        expr->exp += 2;
        while (isxdigit(*(expr->exp))) {
            n = n * 16 + (*(expr->exp) <= '9' ? *(expr->exp) - '0' : tolower(*(expr->exp)) - 'a' + 10);
            expr->exp++;
        }
    } else {
        // Decimal
        while (isdigit(*(expr->exp))) {
            n = n * 10 + (*(expr->exp) - '0');
            expr->exp++;
        }
    }

    return neg ? -n : n;
}

// Skip spaces
void skip(expr_t *expr) {
    while (*(expr->exp) == ' ') expr->exp++;
}

// Parse factors (numbers or parentheses)
int factor(expr_t *expr) {
    skip(expr);

    if (*(expr->exp) == '(') {
        expr->exp++; // Skip '('
        int result = eval(expr); // Recursive call for parentheses
        skip(expr);
        if (*(expr->exp) == ')') {
            expr->exp++; // Skip ')'
        } else {
            expr->err = 1; // Unmatched parentheses
        }
        return result;
    }

    return number(expr);
}

// Parse terms (factors with * and /)
int term(expr_t *expr) {
    int res = factor(expr);
    skip(expr);

    while (*(expr->exp) == '*' || *(expr->exp) == '/') {
        char op = *(expr->exp);
        expr->exp++; // Skip operator
        int num = factor(expr);

        if (op == '*') {
            res *= num;
        } else if (op == '/') {
            if (num == 0) {
                expr->err = 2; // Division by zero
                return 0;
            }
            res /= num;
        }
        skip(expr);
    }

    return res;
}

// Evaluate the expression (with + and -)
int eval(expr_t *expr) {
    int res = term(expr);
    skip(expr);

    while (*(expr->exp) == '+' || *(expr->exp) == '-') {
        char op = *(expr->exp);
        expr->exp++; // Skip operator
        int num = term(expr);

        if (op == '+') {
            res += num;
        } else if (op == '-') {
            res -= num;
        }
        skip(expr);
    }

    return res;
}

int bc_main() {
    char in[BUFFER_SIZE];

    while (1) {
        printf(PROMPT);
        if (!fgets(in, BUFFER_SIZE, stdin)) break;

        if (!strcmp(in, "exit") || !strcmp(in, "quit")) break;

        int err = ERR_NONE;
        expr_t expr = {in, err}; // Initialize expr_t with the expression and error state

        int res = eval(&expr);

        switch (expr.err)
        {
        case ERR_NONE:
            printf("%d\n", res);
            break;

        case ERR_SYNTAX:
            printf("Syntax error\n");
            break;

        case ERR_DIVISION_BY_ZERO:
            printf("Division by zero\n");
            break;
        
        default:
            break;
        }
    }

    return 0;
}

void shcmd_bc(int argc, char *argv[]) {
    if (!argc || !argv)
        return;
    
    bc_main();
}

