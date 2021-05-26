#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


typedef struct User{
	int id;
	int old_id;
	char name1[100];
	char name2[100];
	char password[15];
	float balance;
	char type[20];
} User;

int main(){
    // Creating the admin file.
	int fd = open("db", O_RDWR | O_CREAT, 0744); 

	for(int i = 0; i<3 ; i++ ){
		User newUser;
		char i_no[5];
		sprintf(i_no, "%d", i);
		strcpy(newUser.name1 , "admin");
		strcat(newUser.name1,i_no);
		strcpy(newUser.password , "root");
		newUser.id = i;
		strcpy(newUser.type , "admin");
		write(fd, &newUser, sizeof(User));
	}
	for(int i = 0; i<5 ; i++ ){
		User newUser;
		char i_no[5];
		sprintf(i_no, "%d", i);
		strcpy(newUser.name1 , "user");
		strcat(newUser.name1,i_no);
		strcpy(newUser.password , "user");
		newUser.id = i+3;
		strcpy(newUser.type , "user");
		newUser.balance = 1000;
		write(fd, &newUser, sizeof(User));
	}
	for(int i = 0; i<5 ; i++ ){
		User newUser;
		char i_no[5];
		sprintf(i_no, "%d", i);
		strcpy(newUser.name1 , "usera");
		strcpy(newUser.name2 , "userb");
		strcat(newUser.name1,i_no);
		strcat(newUser.name2,i_no);
		strcpy(newUser.password , "joint");
		newUser.id = i+8;
		strcpy(newUser.type , "joint");
		newUser.balance = 1000;
		write(fd, &newUser, sizeof(User));
	}
	close(fd);
	return 0;
}





