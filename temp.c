#include <string.h>
#include <stdio.h>

void swap_(void* x, void* y, size_t n)
{
	char buffer[n];
	memcpy(buffer, x, n);
	memcpy(x, y, n);
	memcpy(y, buffer, n);
}

void swap_ends(void *ptr, size_t nelems, size_t nbytes) 
{
	memcpy(ptr, (char *)ptr + (nelems - 1) * nbytes, nbytes);
}



int main() {
	char* a = "hello";
	char* b = "world";
	swap_(&a, &b, sizeof(a));
	printf("%s %s \n", a, b);

}
