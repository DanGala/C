#include <stdio.h>
#include <unistd.h>

int main(){

	FILE *ptr1, *ptr2, *ptr3;
	ptr1 = fopen("./timing1.txt","r");
	ptr2 = fopen("./timing2.txt","r");
	ptr3 = fopen("./timing3.txt","r");
	if(!ptr1 || !ptr2 || !ptr3){
		printf("Error abriendo archivos de tiempos\n");
		return 1;
	}

	FILE *file;
	file = fopen("./global.txt","w");
	if(!file){
		printf("Error abriendo archivo de tiempos maximos\n");
		return 1;
	}

	int item;
	int max = 0;
	while(fscanf(ptr1, "%d", &item)==1){
		if(item>max) max=item;
	}
	fprintf(file, "El tiempo maximo de respuesta de la tarea 1 es: %d ns\n", max);
	max=0;
	while(fscanf(ptr2, "%d", &item)==1){
		if(item>max) max=item;
	}
	fprintf(file, "El tiempo maximo de respuesta de la tarea 2 es: %d ns\n", max);
	max=0;
	while(fscanf(ptr3, "%d", &item)==1){
		if(item>max) max=item;
	}
	fprintf(file, "El tiempo maximo de respuesta de la tarea 3 es: %d ns\n", max);

	fclose(ptr1);
	fclose(ptr2);
	fclose(ptr3);
	fclose(file);
	
	return 0;
}
