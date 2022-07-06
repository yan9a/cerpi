#include<stdio.h>
#include<zint.h>
int main()
{
	struct zint_symbol *my_symbol;
	my_symbol = ZBarcode_Create();
	if(my_symbol != NULL){
		printf("Symbol successfully created!\n");
	}
	ZBarcode_Encode(my_symbol, "87654321",0);
	ZBarcode_Print(my_symbol,0);
	ZBarcode_Delete(my_symbol);
	return 0;
}
