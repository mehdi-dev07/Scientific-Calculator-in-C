#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

//==================================variable=======================================//
typedef struct variable
{
    double number;
    char operation;
    bool is_num;
    int priority;
} variable;

//==========================main list after convertion=============================//
typedef struct Node
{
    struct Node *next;
    variable data;
} Node;

typedef struct List
{
    Node *head;
} List;

List *create_list()
{
    List *list = malloc(sizeof(List));
    if (!list)
        return NULL;
    list->head = NULL;
    return list;
}

int insert_end(List *list, variable value)
{
    if (!list)
        return 0;

    Node *node = malloc(sizeof(Node));
    if (!node)
        return 0;

    node->data = value;
    node->next = NULL;

    if (!list->head)
    {
        list->head = node;
        return 1;
    }

    Node *curr = list->head;
    while (curr->next)
        curr = curr->next;

    curr->next = node;
    return 1;
}

void free_list(List *list)
{
    if (!list)
        return;

    Node *curr = list->head;
    while (curr)
    {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }
    free(list);
}

int list_size(List *l)
{
    int count = 0;
    Node *curr = l->head;
    while (curr != NULL)
    {
        count++;
        curr = curr->next;
    }
    return count;
}

//=============================the input list======================================//
typedef struct inputNode
{
    struct inputNode *next;
    char data;
} inputNode;

typedef struct inputList
{
    inputNode *head;
} inputList;

inputList *create_inputlist(void)
{
    inputList *list = malloc(sizeof(inputList));
    if (!list)
        return NULL;
    list->head = NULL;
    return list;
}

int insert_input_end(inputList *list, char c)
{
    inputNode *node = malloc(sizeof(inputNode));
    if (!node)
        return 0;

    node->data = c;
    node->next = NULL;

    if (!list->head)
    {
        list->head = node;
        return 1;
    }

    inputNode *curr = list->head;
    while (curr->next)
        curr = curr->next;

    curr->next = node;
    return 1;
}

void free_input_list(inputList *list)
{
    if (!list)
        return;

    inputNode *curr = list->head;
    while (curr)
    {
        inputNode *next = curr->next;
        free(curr);
        curr = next;
    }
    free(list);
}

//====================stack for postfix=========================================//
typedef struct Stack
{
    int size;
    int capacity;
    variable *collection;
} Stack;

Stack *create_stack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->size = 0;
    stack->collection = (variable *)malloc(stack->capacity * sizeof(variable));
    return stack;
}

void push(Stack *stack, variable item)
{
    if (stack->size < stack->capacity)
    {
        stack->collection[stack->size++] = item;
    }
}

variable pop(Stack *stack)
{
    if (stack->size == 0)
    {
        variable empty = {0};
        return empty;
    }
    return stack->collection[--stack->size];
}

variable peek(Stack *stack)
{
    if (stack->size == 0)
    {
        variable empty = {0};
        return empty;
    }
    return stack->collection[stack->size - 1];
}

bool if_empty_stack(Stack *stack)
{
    return stack->size == 0;
}

void free_stack(Stack *stack)
{
    if (stack)
    {
        free(stack->collection);
        free(stack);
    }
}

//=======================useful functions=======================================//
bool check_if_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

int get_priority(char op)
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 3;
    return 0;
}

bool is_integer(char c)
{
    return c >= '0' && c <= '9';
}

//====================evaluate expression===========================//
double evaluate_expression(inputList *expr_list);

List *input_convertion(inputList *l);
List *postfix_conversion(List *input);
double evaluate_equation(List *list);

//====================extract content between parentheses=======================//
inputList *extract_parentheses(inputNode **curr_ptr)
{
    inputList *sub_expr = create_inputlist();
    inputNode *curr = *curr_ptr;

    if (curr && curr->data == '(')
    {
        curr = curr->next; // Skip opening parenthesis
    }

    int depth = 1;
    while (curr != NULL && depth > 0)
    {
        if (curr->data == '(')
            depth++;
        if (curr->data == ')')
            depth--;

        if (depth > 0)
        {
            insert_input_end(sub_expr, curr->data);
        }

        curr = curr->next;
    }

    *curr_ptr = curr; // Update pointer to position after closing parenthesis
    return sub_expr;
}

//====================evaluate helper for sum/product==========================//
double evaluate_expression(inputList *expr_list)
{
    List *var_list = input_convertion(expr_list);
    List *postfix = postfix_conversion(var_list);
    double result = evaluate_equation(postfix);
    free_list(var_list);
    free_list(postfix);
    return result;
}

//====================handle sum function======================================//
double handle_sum(inputNode **curr_ptr)
{
    inputNode *curr = *curr_ptr;

    // Skip "sum("
    while (curr && curr->data != '(')
        curr = curr->next;
    if (!curr)
        return 0;
    curr = curr->next; // Skip '('

    // Parse start value
    inputList *start_expr = create_inputlist();
    while (curr && curr->data != ',')
    {
        insert_input_end(start_expr, curr->data);
        curr = curr->next;
    }
    int start = (int)evaluate_expression(start_expr);
    free_input_list(start_expr);

    if (curr)
        curr = curr->next; // Skip ','

    // Parse end value
    inputList *end_expr = create_inputlist();
    while (curr && curr->data != ',')
    {
        insert_input_end(end_expr, curr->data);
        curr = curr->next;
    }
    int end = (int)evaluate_expression(end_expr);
    free_input_list(end_expr);

    if (curr)
        curr = curr->next; // Skip ','

    // Parse expression with 'n'
    inputList *n_expr_template = create_inputlist();
    int depth = 1;
    while (curr && depth > 0)
    {
        if (curr->data == '(')
            depth++;
        if (curr->data == ')')
            depth--;

        if (depth > 0)
        {
            insert_input_end(n_expr_template, curr->data);
        }
        curr = curr->next;
    }

    // Calculate sum
    double sum = 0;
    for (int n = start; n <= end; n++)
    {
        // Replace 'n' with actual value
        inputList *n_expr = create_inputlist();
        inputNode *template_curr = n_expr_template->head;

        while (template_curr)
        {
            if (template_curr->data == 'n')
            {
                // Insert the number
                char num_str[20];
                sprintf(num_str, "%d", n);
                for (int i = 0; i < strlen(num_str); i++)
                {
                    insert_input_end(n_expr, num_str[i]);
                }
            }
            else
            {
                insert_input_end(n_expr, template_curr->data);
            }
            template_curr = template_curr->next;
        }

        sum += evaluate_expression(n_expr);
        free_input_list(n_expr);
    }

    free_input_list(n_expr_template);
    *curr_ptr = curr;
    return sum;
}

//====================handle product function===================================//
double handle_product(inputNode **curr_ptr)
{
    inputNode *curr = *curr_ptr;

    // Skip "pro("
    while (curr && curr->data != '(')
        curr = curr->next;
    if (!curr)
        return 1;
    curr = curr->next; // Skip '('

    // Parse start value
    inputList *start_expr = create_inputlist();
    while (curr && curr->data != ',')
    {
        insert_input_end(start_expr, curr->data);
        curr = curr->next;
    }
    int start = (int)evaluate_expression(start_expr);
    free_input_list(start_expr);

    if (curr)
        curr = curr->next; // Skip ','

    // Parse end value
    inputList *end_expr = create_inputlist();
    while (curr && curr->data != ',')
    {
        insert_input_end(end_expr, curr->data);
        curr = curr->next;
    }
    int end = (int)evaluate_expression(end_expr);
    free_input_list(end_expr);

    if (curr)
        curr = curr->next; // Skip ','

    // Parse expression with 'n'
    inputList *n_expr_template = create_inputlist();
    int depth = 1;
    while (curr && depth > 0)
    {
        if (curr->data == '(')
            depth++;
        if (curr->data == ')')
            depth--;

        if (depth > 0)
        {
            insert_input_end(n_expr_template, curr->data);
        }
        curr = curr->next;
    }

    // Calculate product
    double product = 1;
    for (int n = start; n <= end; n++)
    {
        // Replace 'n' with actual value
        inputList *n_expr = create_inputlist();
        inputNode *template_curr = n_expr_template->head;

        while (template_curr)
        {
            if (template_curr->data == 'n')
            {
                // Insert the number
                char num_str[20];
                sprintf(num_str, "%d", n);
                for (int i = 0; i < strlen(num_str); i++)
                {
                    insert_input_end(n_expr, num_str[i]);
                }
            }
            else
            {
                insert_input_end(n_expr, template_curr->data);
            }
            template_curr = template_curr->next;
        }

        product *= evaluate_expression(n_expr);
        free_input_list(n_expr);
    }

    free_input_list(n_expr_template);
    *curr_ptr = curr;
    return product;
}

//====================convertion from string to variable=========================//
List *input_convertion(inputList *l)
{
    List *output = create_list();
    inputNode *curr = l->head;
    variable temp;

    while (curr != NULL)
    {
        if (curr->data == ' ')
        {
            curr = curr->next;
            continue;
        }

        // Handle sum function
        if (curr->data == 's' && curr->next && curr->next->data == 'u' &&
            curr->next->next && curr->next->next->data == 'm')
        {
            double sum_result = handle_sum(&curr);
            temp.number = sum_result;
            temp.operation = '0';
            temp.is_num = true;
            temp.priority = 0;
            insert_end(output, temp);
            continue;
        }

        // Handle product function
        if (curr->data == 'p' && curr->next && curr->next->data == 'r' &&
            curr->next->next && curr->next->next->data == 'o')
        {
            double prod_result = handle_product(&curr);
            temp.number = prod_result;
            temp.operation = '0';
            temp.is_num = true;
            temp.priority = 0;
            insert_end(output, temp);
            continue;
        }

        if (is_integer(curr->data) || curr->data == '.')
        {
            double num = 0;

            if (is_integer(curr->data))
            {
                while (curr != NULL && curr->data >= '0' && curr->data <= '9')
                {
                    num = num * 10 + (curr->data - '0');
                    curr = curr->next;
                }
            }

            if (curr != NULL && curr->data == '.')
            {
                curr = curr->next;
                int n = 1;
                while (curr != NULL && curr->data >= '0' && curr->data <= '9')
                {
                    num = num + (curr->data - '0') / pow(10, n);
                    curr = curr->next;
                    n++;
                }
            }

            temp.number = num;
            temp.operation = '0';
            temp.is_num = true;
            temp.priority = 0;
            insert_end(output, temp);
            continue;
        }

        if (check_if_operator(curr->data))
        {
            temp.number = 0;
            temp.operation = curr->data;
            temp.is_num = false;
            temp.priority = get_priority(curr->data);
            insert_end(output, temp);
            curr = curr->next;
            continue;
        }

        if (curr->data == '(' || curr->data == ')')
        {
            temp.number = 0;
            temp.operation = curr->data;
            temp.is_num = false;
            temp.priority = 0;
            insert_end(output, temp);
            curr = curr->next;
            continue;
        }

        // Handle absolute value |expr|
        if (curr->data == '|')
        {
            // Transform to A function (like cos, sin)
            temp.number = 0;
            temp.operation = 'A';
            temp.is_num = false;
            temp.priority = 4;
            insert_end(output, temp);
            curr = curr->next;
            continue;
        }

        if (curr->data == 'c' && curr->next && curr->next->data == 'o' &&
            curr->next->next && curr->next->next->data == 's')
        {
            temp.number = 0;
            temp.operation = 'c';
            temp.is_num = false;
            temp.priority = 4;
            insert_end(output, temp);
            curr = curr->next->next->next;
            continue;
        }

        if (curr->data == 's' && curr->next && curr->next->data == 'i' &&
            curr->next->next && curr->next->next->data == 'n')
        {
            temp.number = 0;
            temp.operation = 's';
            temp.is_num = false;
            temp.priority = 4;
            insert_end(output, temp);
            curr = curr->next->next->next;
            continue;
        }

        if (curr->data == 't' && curr->next && curr->next->data == 'a' &&
            curr->next->next && curr->next->next->data == 'n')
        {
            temp.number = 0;
            temp.operation = 't';
            temp.is_num = false;
            temp.priority = 4;
            insert_end(output, temp);
            curr = curr->next->next->next;
            continue;
        }

        curr = curr->next;
    }

    return output;
}

//====================postfix convertion=======================================//
List *postfix_conversion(List *input)
{
    Stack *stack = create_stack(list_size(input));
    List *output = create_list();

    Node *curr = input->head;

    while (curr != NULL)
    {
        if (curr->data.is_num)
        {
            insert_end(output, curr->data);
        }
        else if (curr->data.operation == '(')
        {
            push(stack, curr->data);
        }
        else if (curr->data.operation == ')')
        {
            while (!if_empty_stack(stack) && peek(stack).operation != '(')
            {
                variable temp = pop(stack);
                insert_end(output, temp);
            }
            if (!if_empty_stack(stack))
            {
                pop(stack);
            }
        }
        else
        {
            while (!if_empty_stack(stack) &&
                   peek(stack).operation != '(' &&
                   peek(stack).priority >= curr->data.priority)
            {
                variable temp = pop(stack);
                insert_end(output, temp);
            }
            push(stack, curr->data);
        }
        curr = curr->next;
    }

    while (!if_empty_stack(stack))
    {
        variable temp = pop(stack);
        if (temp.operation != '(' && temp.operation != ')')
        {
            insert_end(output, temp);
        }
    }

    free_stack(stack);
    return output;
}

//=========================evaluate postfix===============================//
double evaluate_equation(List *list)
{
    Node *curr = list->head;
    Stack *stack = create_stack(list_size(list));

    while (curr != NULL)
    {
        if (curr->data.is_num)
        {
            push(stack, curr->data);
        }
        else
        {
            // For unary operators (cos, sin, tan, abs)
            if (curr->data.operation == 'c' || curr->data.operation == 's' ||
                curr->data.operation == 't' || curr->data.operation == 'A')
            {
                variable operand = pop(stack);
                double result = 0;

                switch (curr->data.operation)
                {
                case 'c': // cos
                    result = cos(operand.number);
                    break;
                case 's': // sin
                    result = sin(operand.number);
                    break;
                case 't': // tan
                    result = tan(operand.number);
                    break;
                case 'A': // abs
                    result = fabs(operand.number);
                    break;
                }

                variable res_var;
                res_var.number = result;
                res_var.is_num = true;
                res_var.operation = '0';
                res_var.priority = 0;
                push(stack, res_var);
            }
            else
            {
                // Binary operators
                variable operand2 = pop(stack);
                variable operand1 = pop(stack);
                double result = 0;

                switch (curr->data.operation)
                {
                case '+':
                    result = operand1.number + operand2.number;
                    break;
                case '-':
                    result = operand1.number - operand2.number;
                    break;
                case '*':
                    result = operand1.number * operand2.number;
                    break;
                case '/':
                    if (operand2.number != 0)
                        result = operand1.number / operand2.number;
                    else
                    {
                        printf("Error: Division by zero\n");
                        free_stack(stack);
                        return 0;
                    }
                    break;
                case '^':
                    result = pow(operand1.number, operand2.number);
                    break;
                default:
                    break;
                }

                variable res_var;
                res_var.number = result;
                res_var.is_num = true;
                res_var.operation = '0';
                res_var.priority = 0;
                push(stack, res_var);
            }
        }
        curr = curr->next;
    }

    variable final = pop(stack);
    double final_result = final.number;
    free_stack(stack);
    return final_result;
}

//=========================main function=======================================//
int main()
{
    char input[512];

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          ADVANCED CALCULATOR - C Implementation              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("Supported Operations:\n");
    printf("  • Basic: +, -, *, /, ^ (power)\n");
    printf("  • Trigonometric: sin(x), cos(x), tan(x)\n");
    printf("  • Absolute Value: |expression|\n");
    printf("  • Sum: sum(start,end,expression) - e.g., sum(1,10,n)\n");
    printf("  • Product: pro(start,end,expression) - e.g., pro(1,5,n)\n");
    printf("  • Parentheses: ( )\n\n");


    printf("═══════════════════════════════════════════════════════════════\n\n");

    while (1)
    {
        printf("Enter expression (or 'quit' to exit): ");

        if (!fgets(input, sizeof(input), stdin))
        {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Check for quit command
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "q") == 0)
        {
            printf("\nThank you for using the calculator!\n");
            break;
        }

        // Skip empty input
        if (strlen(input) == 0)
        {
            continue;
        }

        // Create input list
        inputList *inp_list = create_inputlist();
        for (int i = 0; i < strlen(input); i++)
        {
            insert_input_end(inp_list, input[i]);
        }

        // Convert to variable list
        List *var_list = input_convertion(inp_list);

        if (!var_list || !var_list->head)
        {
            printf("Error: Invalid expression\n\n");
            free_input_list(inp_list);
            if (var_list)
                free_list(var_list);
            continue;
        }

        printf("\n┌─────────────────────────────────────────────────────────────┐\n");
        printf("│ Infix Expression:                                           │\n");
        printf("└─────────────────────────────────────────────────────────────┘\n  ");
        Node *curr = var_list->head;
        while (curr)
        {
            if (curr->data.is_num)
                printf("%.2f ", curr->data.number);
            else
                printf("%c ", curr->data.operation);
            curr = curr->next;
        }
        printf("\n");

        // Convert to postfix
        List *postfix = postfix_conversion(var_list);

        printf("\n┌─────────────────────────────────────────────────────────────┐\n");
        printf("│ Postfix Expression:                                         │\n");
        printf("└─────────────────────────────────────────────────────────────┘\n  ");
        curr = postfix->head;
        while (curr)
        {
            if (curr->data.is_num)
                printf("%.2f ", curr->data.number);
            else
                printf("%c ", curr->data.operation);
            curr = curr->next;
        }
        printf("\n");

        // Evaluate
        double result = evaluate_equation(postfix);

        printf("\n┌─────────────────────────────────────────────────────────────┐\n");
        printf("│ RESULT:                                                     │\n");
        printf("└─────────────────────────────────────────────────────────────┘\n");
        printf("  %.6f\n", result);
        printf("\n═══════════════════════════════════════════════════════════════\n\n");

        // Cleanup
        free_input_list(inp_list);
        free_list(var_list);
        free_list(postfix);
    }

    return 0;
}
