#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define memMsg printf("There is not enough memory")
#define FIRST_SIZE 5
#define MAX_STRING 100
#define MAX_STACK 100
#define MAX_OP 10
#define FILE_NAME "input.txt"

typedef struct D{
	enum {float_, char_, op_} type;
	union{
		char ch;
		float fl;
	};
}Data;

typedef struct V{
	char name;
	float val;
}Var;

typedef struct M{
	Var *data;
	int size,
		lastEl;
}Memo;

typedef struct S{
	Data *data;
	int lasEl,
		size,
		type;
}Stack;

Stack *createStack(int size); // To create an empty stack
void printStack(Stack *stack); // To print stack members
int isFull(Stack *stack);  // Controlling stack
int isEmpty(Stack *stack); // state
Data pop(Stack *stack); // To pop an element from stack
void push(Stack *stack, Data data); // To push an element to stack

Memo *createMemo(); // Create memory for keeping variables and its values
float getMemo(Memo *memo, char name); // Get the value of given variable
void extendMemo(Memo *memo); // To extend the maximum variable capacity
void addMemo(Memo *memo, char name, float val); // Adding new variable to memory
void printMemo(Memo *memo); // Printing all variables and its values in memory

Data *createPostFix(char *expr); // Create a postfix expression from infix expression
void printPostFix(Data *data); // Print postfix expression
Data getNumStr(char *str, int strNum); // Getting a number value from inside of a string
float solvePostFix(Data *postFix, Memo *memo); // Solve the postfix expression and return the result
float operation(float val1, float val2, char op); // Make the given operation between given two values
int priority(char ch); // Return the priority of the operation
int isChar(char ch); // Control the char by is it
int isDigit(char ch); // a letter or is it a digit
Data  **turnPostFix(char **expr); // Turning given multiple expressions to the postfix array

char **getFile(); // Reading lines to the string array from the file
char *delSpace(char *expr); // Deleting space characters from the string

int main(){
	int i;
	float tmpVal;
	Data **pFix;
	char **expr;
	Memo *memo;
	memo = createMemo();
	
	
	printf("[INFIX-POSTFIX DONUSUMU]\n");
	expr = getFile();
	pFix = turnPostFix(expr);
	i = 0;
	printf("Postfix Ifadeler: \n");
	while(pFix[i] != NULL){
		printf("%c=",expr[i][0]);
		printPostFix(pFix[i]);
		printf("\n");
		i++;
	}
	
	printf("\n[POSTFIX IFADENIN COZUMU]\n");
	i=0;
	while(pFix[i] != NULL){
		printf("%c=",expr[i][0]);
		printPostFix(pFix[i]);
		printf(" icin cozum:\n");
		tmpVal = solvePostFix(pFix[i], memo);
		addMemo(memo,expr[i][0],tmpVal);
		printf("memory:");
		printMemo(memo);
		printf("\n\n");
		i++;
	}
	
	printf("\n[TOPLAM SONUC]\n");
	printMemo(memo);
	
	return 0;
}

/* STACK FUNCTIONS */
Stack *createStack(int size){
	Stack *ret;
	if((ret = (Stack *)malloc(sizeof(Stack))) != NULL){
		ret->size = size;
		ret->lasEl = 0;
		if((ret->data = (Data *)malloc(sizeof(Data)*size)) == NULL){
			memMsg;
			return NULL;
		}
	} else {
		memMsg;
	}
	return ret;
}

void printStack(Stack *stack){
	int i;
	for(i=0;i<stack->lasEl;i++){
		if(stack->data[i].type == char_)
			printf("%c ",stack->data[i].ch);
		else if(stack->data[i].type == float_)
			printf("%.2f ",stack->data[i].fl);
	}
}

void push(Stack *stack, Data data){
	if(!isFull(stack)){
		stack->data[stack->lasEl] = data;
		stack->lasEl++;
	}
}

Data pop(Stack *stack){
	Data data;
	data.ch = '\0';
	if(!isEmpty(stack)){
		stack->lasEl--;
		data = stack->data[stack->lasEl];
	}
	return data;
}

int isEmpty(Stack *stack){
	if(stack->lasEl == 0){
		return 1;
	} else
		return 0;
}

int isFull(Stack *stack){
	if(stack->lasEl == stack->size){
		return 1;
	} else
		return 0;
}
/* STACK FUNCTIONS END */

/* VARIABLE SAVING FUNCTIONS */
void addMemo(Memo *memo, char name, float val){
	int i = 0,
		flag = 0;
	while(i<memo->lastEl && !flag){
		if(memo->data[i].name == name){
			memo->data[i].val = val;
			flag = 1;
		}
		i++;
	}
	if(!flag){
		extendMemo(memo);
		memo->data[memo->lastEl].name = name;
		memo->data[memo->lastEl].val = val;
		memo->lastEl++;
	}
}

void extendMemo(Memo *memo){
	Var *temp;
	int i;
	if(memo->size == memo->lastEl){
		if((temp = (Var *)malloc(sizeof(Var)*memo->size*2)) == NULL){
			memMsg;
			exit(0);
		}
		for(i=0;i<memo->size;i++){
			temp[i].name = memo->data[i].name;
			temp[i].val = memo->data[i].val;
		}
		free(memo->data);
		memo->data = temp;
		memo->size *= 2;
	}
}

float getMemo(Memo *memo, char name){
	int i;
	for(i=0;i<memo->lastEl;i++){
		if(memo->data[i].name == name)
			return memo->data[i].val;
	}
	return -33.77;
}

Memo *createMemo(){
	Memo *ret;
	ret = (Memo *)malloc(sizeof(Memo));
	ret->size = FIRST_SIZE;
	ret->lastEl = 0;
	ret->data = (Var *)malloc(sizeof(Var)*FIRST_SIZE);
	return ret;
}

void printMemo(Memo *memo){
	int i;
	for(i=0;i<memo->lastEl-1;i++){
		printf("%c=%.1f , ",memo->data[i].name,memo->data[i].val);
	}
	printf("%c=%.1f;",memo->data[i].name,memo->data[i].val);
}

/* VARIABLE SAVING FUNCTIONS END */

/* POSTFIX OPERATIONS */
Data *createPostFix(char *expr){
	int i = 2,
		cnt = 0,
		flag,
		size = FIRST_SIZE;
	Stack *stack;
	stack = createStack(MAX_STACK);
	Data *ret,
		 temp;
	ret = (Data *)malloc(sizeof(Data)*FIRST_SIZE);
	while(expr[i] != ';'){
		if(cnt == size){
			ret = realloc(ret,size*2*sizeof(Data));
			size *= 2;
		}
		
		// Control for add next character to expression or stack
		if(isDigit(expr[i])){
			// transform digits to integer and add it to postfix
			temp = getNumStr(expr, i);
			ret[cnt].type = float_;
			ret[cnt].fl = temp.fl;
			i = temp.ch - '0';
			cnt++;
		} else if(isChar(expr[i])){
			// add variable to postfix
			ret[cnt].type = char_;
			ret[cnt].ch = expr[i];
			cnt++;
		} else {
			if(expr[i] == '('){
				temp.type = char_;
				temp.ch = expr[i];
				push(stack,temp);
			} else if(expr[i] == ')'){
				if(!isEmpty(stack))
					flag = 0;
				temp = pop(stack);
				while((!flag) && (temp.ch != '(')){
					ret[cnt] = temp;
					cnt++;
					if(isEmpty(stack))
						flag = 1;
					temp = pop(stack);
				}
			} else { // When last symbol of expression is operation "+-/*" symbol
				if(!isEmpty(stack)){
					flag = 0;
					temp = pop(stack);
					while((!flag) && (temp.ch != '(') &&  (priority(temp.ch) >= priority(expr[i]))){
						ret[cnt] = temp;
						cnt++;
						if(isEmpty(stack))
							flag = 1;
						else
							temp = pop(stack);
					}
					if(!flag)
						push(stack,temp);
				}
				
				// Adding last operation symbol of expression to stack
				temp.type = char_;
				temp.ch = expr[i];
				push(stack,temp);
			}
		}
		
		// Showing steps on screen
		ret[cnt+1].ch = '\0';
		printf("Postfix: ");
		printPostFix(ret);
		printf("\tStack: ");
		printStack(stack);
		printf("\n");
		
		// Moving character iterator of expression string to next element
		i++;
	}
	
	while(!isEmpty(stack)){ // Adding remain elements on stack to postfix
		ret[cnt] = pop(stack);
		
		// Showing steps on screen
		ret[cnt+1].ch = '\0';
		printf("Postfix: ");
		printPostFix(ret);
		printf("\tStack: ");
		printStack(stack);
		printf("\n");
		
		cnt++;
	}
	ret = realloc(ret,size*(cnt+1));
	ret[cnt].ch = '\0'; // Adding an ending symbol to postfix
	return ret;
}

int priority(char ch){
	if(ch == '(')
		return 3;
	if(ch == ')')
		return 3;
	if(ch == '*')
		return 1;
	else if(ch == '/')
		return 1;
	else if(ch == '+')
		return 0;
	else if(ch == '-')
		return 0;
	else
		return -2;
}

Data getNumStr(char *str, int strNum){
	int i = strNum,
		val = 0;
	Data ret; // In here data type is used for just passing multiple information to other function
	char *temp;
	while(isDigit(str[i])){
		val += (str[i] -'0');
		val *= 10;
		i++;
	}
	i--;
	val /= 10;
	ret.fl = (float)val;
	ret.ch = i + '0';
	return ret;
}

void printPostFix(Data *data){
	int i = 0;
	while(data[i].ch != '\0'){
		if(data[i].type == char_)
			printf("%c",data[i].ch);
		else if(data[i].type == float_)
			printf("%.0f",data[i].fl);
		i++;
	}
}

float operation(float val1, float val2, char op){
	if(op == '*'){
		return val1*val2;
	}
	if(op == '/'){
		return val1/val2;
	}
	if(op == '+'){
		return val1+val2;
	}
	if(op == '-'){
		return val1-val2;
	}
}

float solvePostFix(Data *postFix, Memo *memo){
	int i = 0;
	Stack *stack;
	Data temp;
	stack = createStack(MAX_STACK);
	while(postFix[i].ch != '\0'){
		if(postFix[i].type == char_){
			if(isChar(postFix[i].ch)){
				temp.type = float_;
				temp.fl = getMemo(memo, postFix[i].ch);
				if(temp.fl == (float)-33.77){
					printf("Symbol is wrong!");
					exit(0);
				}
				push(stack,temp);
			} else {
				temp.type = float_;
				temp.fl = operation(pop(stack).fl,pop(stack).fl,postFix[i].ch);
				push(stack,temp);
			}
		} else if(postFix[i].type == float_){
			push(stack,postFix[i]);
		}
		printf("Stack: ");
		printStack(stack);
		printf("\n");
		i++;
	}
	return pop(stack).fl;
}

Data  **turnPostFix(char **expr){
	int i;
	Data **pFix;
	pFix = (Data **)malloc(sizeof(Data*)*MAX_OP);
	
	for(i=0;i<MAX_OP;i++){
		pFix[i] = (Data *)malloc(sizeof(char)*MAX_STRING);
	}
	
	i = 0;
	while(expr[i] != NULL){
		// Getting and transforming expression to postfix
		expr[i] = delSpace(expr[i]);
		pFix[i] = createPostFix(expr[i]);
		
		printf("Ara cikis: ");
		printPostFix(pFix[i]);
		printf("\n\n");
		i++;
	}
	pFix[i] = NULL;
	return pFix;
}
/* POSTFIX OPERATIONS END */

/* OTHER FUNCTIONS */
int isDigit(char ch){
	if(ch - '0'>= 0 && '9' - ch >= 0)
		return 1;
	else 
		return 0;
}

int isChar(char ch){
	if(ch - 'a'>= 0 && 'z' - ch >= 0)
		return 1;
	else 
		return 0;
}

char *delSpace(char *expr){
	int i,
		j;
	char *ret;
	ret = (char *)malloc(sizeof(char)*MAX_STRING);
	
	i = 0;
	j = 0;
	while(expr[i] != '\0'){
		if(expr[i] == '–'){
			ret[j] = '-';
			j++;
		}
		else if(expr[i] != ' '){
			ret[j] = expr[i];
			j++;
		}
		i++;
	}
	
	return ret;
}

char **getFile(){
	char **expr;
	FILE *input;
	int i;
	
	expr = (char **)malloc(sizeof(char *)*MAX_OP);
	for(i=0;i<MAX_OP;i++){
		expr[i] = (char *)malloc(sizeof(char)*MAX_STRING);
	}
	if((input = fopen(FILE_NAME,"r"))==NULL){
		printf("File couldn't open!\n");
		exit(0);
	}
	
	i=0;
	while(fgets(expr[i++],MAX_STRING,input) != NULL){}
	expr[i-1] = NULL;
	return expr;
}
/* OTHER FUNCTIONS END */
