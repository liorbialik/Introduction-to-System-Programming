#include <stdlib.h>
/*oOoOoOoOoOoOoOoOoOoOoOoOoOo Includes OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#include <stdio.h>
#include <stdlib.h> /* for malloc(), free() */

//for memory leak tests
#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define MemoryLeak


/*oOoOoOoOoOoOoOoOoOoOoOoOoOo Constants oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define SMALL_ARR_SIZE 5
#define MAX_SIZE 100
#define END_OF_INPUT (-1)
#define TRUE 1
#define FALSE 0



/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*			1. Memory Arrangement and Pointers							  */
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/**
* <ISP> MemoryArrangementExample shows how variables (actually the fields
* of a struct) are arranged in memory. See the printf() in the function for
* instructions on usage.
*/
void MemoryArrangementExample()
{
	struct ExampleStruct {
		char c1;
		char c2;
		int i;
		short s;
		int j;
	};

	struct ExampleStruct A;

	A.c1 = 'a';   /* decimal 97, hexa 0x61, binary 01100001 */
	A.c2 = 'b';
	A.i = 72536; /* hexa 0x00011B58, binary 00000000 00000001 00011011 01011000 */
	A.s = 1;     /* hexa 0x0001, binary 00000000 00000001 */
				 //A.j = 999;			 /* A.j remains uninitialized */
	printf("A's address: %x\n", &A);

	printf("Place a breakpoint here. Then (in Visual Studio 2010):\n"
		"1) Start Debugging (F5) \n"
		"2) Goto 'Debug' menu -> Windows -> Memory -> Memory 1 \n"
		"3) In the 'address' line enter &(A) \n\n");
}


/**
* <ISP> Demonstration of using an uninitialized pointer
*/
void EvilUninitializedPointer1()
{
	int a = 1;
	int b = 2;
	int *p; /* <ISP> Beware! uninitialized! Contains unknown garbage! */

	//How to init the pointer p?
	//*p = a;			//Wrong
	p = &a;			//Correct

	a = b;
	b = *p;

	printf("%d,%d\n", a, b); /* output depends on exactly what the garbage */
							 /* contained in p was. This function may work */
							 /* correctly many times and then suddenly fail */
}



/**
* <ISP> Demonstrates allocation of dynamic memory.
*/
void DynamicMemoryExample()
{
	int i;
	int *FibonacciSeries = NULL;
	int NumOfMembers;

	printf("which Fibonacci number would you like to calculate? ");
	scanf("%d", &NumOfMembers);

	FibonacciSeries = (int*)malloc(NumOfMembers*sizeof(int));
	if (FibonacciSeries == NULL)
	{
		printf("Could not allocate memory. Calculation Failed.");
		return;
	}

	FibonacciSeries[0] = 1;
	FibonacciSeries[1] = 1;

	for (i = 2; i < NumOfMembers; i++)
		FibonacciSeries[i] = FibonacciSeries[i - 1] + FibonacciSeries[i - 2];

	printf("The number is %d\n", FibonacciSeries[NumOfMembers - 1]);

	//free(FibonacciSeries);  //Don't forget to release the memory allocated
}


/**
* <ISP> Helper function, used by PointerToFreedMemoryExample
*/
void DoStuffWith(int* pi)
{
	/* writing, reading, etc. */
}

/**
* <ISP> Demonstrates what happens when two pointers point to the same
*       dynamically allocated memory.
*/
void PointerToFreedMemoryExample()
{
	int* A = (int*)malloc(SMALL_ARR_SIZE * sizeof(int));
	int *B = A;

	free(A); /* ok */

	DoStuffWith(B); /* Error! B is now pointing to an un-allocated part of */
					/* the heap. Writing there will corrupt the heap. */

	free(B); /* Error! freeing memory that was already freed! */
}



int main() {


	//Examples from Rec03. Call single function every time.
	
/*4 examples of the first section of the class */
	//MemoryArrangementExample();
	//EvilUninitializedPointer1();
	//DynamicMemoryExample();
	//PointerToFreedMemoryExample();

/*example of lifecycle of a process- using Win32 API functions */
	//CreateProcessSimpleMain();

#ifdef MemoryLeak
	_CrtDumpMemoryLeaks();  //Call this function to view the unrelease memory
#endif


}