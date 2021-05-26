#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
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

int option, currUserID;

// Function implementations
void showMenu(int fd);
void chooseOption(int fd);

// This functions sends a request to the server for the login of a normal user
void UserLogin(int fd){
	int result;
	User currUser;
	printf("Please enter your user ID: ");
	scanf("%d", &currUser.id);
	currUserID = currUser.id;
	printf("Please enter your password: ");
	scanf("%s", currUser.password);

	// Requests to the server
	write(fd,&currUser,sizeof(User));

	// Response from the server
	read(fd,&result,sizeof(result)); 

	if(result == -1){
		printf("Invalid login \n");
		chooseOption(fd);
	}
	else{
		printf("Login was successful \n");
		option = result;
        write(fd,&option,sizeof(int));
		printf("Going to menu \n");
		showMenu(fd);
	}
}

// This functions sends a request to the server for depositing money to an account
void deposit(int fd){
	float amount;
	int choice = 1;
	bool result;

	printf("Enter the amount you wish to deposit: Rs.");
	scanf("%f", &amount);

	while(amount<=0){
		printf("Enter a valid amount\n");
		printf("Amount to deposit: Rs.");
		scanf("%f", &amount);
	}

	// Requests to the server
	write(fd, &choice, sizeof(int));
	write(fd, &amount, sizeof(float));

	// Response from the server
	read(fd, &result, sizeof(result));

	if(!result){
		printf("An error was encountered while depositing\n");
	}
	else{
		printf("Your amount was deposited successfully\n");
	}
	showMenu(fd);
}

// This functions sends a request to the server for withdrawing money to an account
void withdraw(int fd){
	float amount;
	int choice = 2;
	bool result;

	printf("Enter the amount you wish to withdraw: Rs.");
	scanf("%f", &amount);

	while(amount<=0){
		printf("Enter a valid amount\n");
		printf("Amount to withdraw: Rs.");
		scanf("%f", &amount);
	}

	// Requests to the server
	write(fd, &choice, sizeof(int));
	write(fd, &amount, sizeof(float));

	// Response from the server
	read(fd, &result, sizeof(result));

	if(!result){
		printf("An error was encountered while withdrawing\n");
	}
	else{
		printf("Your amount was withdrawn successfully\n");
	}
	showMenu(fd);
}

// This functions sends a request to the server for retrieving the balance of an account
void balanceEnquiry(int fd){
	float amount;
	int choice = 3;

	// Requests to the server
	write(fd, &choice, sizeof(int));
	
	// Response from the server
	read(fd, &amount, sizeof(float));

	printf("Available Balance:: Rs."); 
	printf("%f\n", amount);

	showMenu(fd);
}

// This functions sends a request to the server for changing the password of an account
void changePassword(int fd){
	int choice = 4;
	char newPassword[15];
	bool result;

	printf("Enter your new password (max 15 characters): ");
	scanf("%s", newPassword);

	// Requests to the server
	write(fd, &choice, sizeof(int));
	write(fd, newPassword, sizeof(newPassword));

	// Response from the server
	read(fd, &result, sizeof(result));

	if(!result){
		printf("An error was encountered while changing your password\n");
	}
	else{
		printf("Your password was changed successfully\n");
	}
	showMenu(fd);
}

// This functions sends a request to the server for retrieving all details of an account
void viewDetails(int fd){
	int select = 5;

	// Request to the server
	write(fd, &select, sizeof(int));

	if(option == 1){
		User currUser1;

		// Response from the server
		read(fd, &currUser1, sizeof(User));
		
		printf("User ID: %d\n", currUser1.id);
		printf("Name: %s\n", currUser1.name1);
		printf("Available Balance: Rs.%f\n", currUser1.balance);
	}
	else if(option == 2){
		User currUser2;

		// Response from the server
		read(fd, &currUser2, sizeof(User));
		
		printf("User ID: %d\n", currUser2.id);
		printf("Main Account Holder's Name: %s\n", currUser2.name1);
		printf("Other Account Holder's Name: %s\n", currUser2.name2);
		printf("Available Balance: Rs.%f\n", currUser2.balance);
	}
	showMenu(fd);
}

// This functions sends a request to the server for creating a new account
void createAccount(int fd){
	int choice = 1;
	int type;
	int result;

	write(fd, &choice, sizeof(int));

	printf("Enter the type account (1: Normal Account 2: Joint Account) you wish to create: ");
	scanf("%d", &type);


	if(type==1){
		User user1;
		printf("Name of the account holder: ");
		scanf(" %[^\n]", user1.name1);
		printf("Password (max 10 characters): ");
		scanf("%s", user1.password);
		printf("Initial Deposit: Rs.");
		scanf("%f", &user1.balance);
		strcpy(user1.type , "user");
		write(fd, &user1, sizeof(User));
	}

	if(type==2){
		User user2;
		printf("Name of the primary account holder: ");
		scanf(" %[^\n]", user2.name1);
		printf("Name of the other account holder: ");
		scanf(" %[^\n]", user2.name2);
		printf("Password (max 10 characters): ");
		scanf("%s", user2.password);
		printf("Initial Deposit: Rs.");
		scanf("%f", &user2.balance);
		strcpy(user2.type , "joint");
		write(fd, &user2, sizeof(User));
	}
	
	// Response from the server
	read(fd,&result,sizeof(result));

	if(!result){
		printf("An error was encountered while creating the account\n");
	}
	else{
		printf("The account was created successfully with ID: %d\n",result);
	}
	showMenu(fd);
}

// This functions sends a request to the server for removing an account
void removeAccount(int fd){
	int choice = 2;
	int type, id;
	bool result;

	write(fd, &choice, sizeof(int));

	printf("User ID: ");
	scanf("%d", &id);
	write(fd, &id, sizeof(int));
	
	// Response from the server
	read(fd,&result,sizeof(result)); 

	if(!result){
		printf("An error was encountered while removing the account, please re-check the User ID you provided\n");
	}
	else{
		printf("The account was removed successfully\n");
	}
	showMenu(fd);
}

// This functions sends a request to the server for modifying an account
void modifyAccount(int fd){
	int choice = 3;
	int type;
	bool result;
	int id,preresult;

	write(fd, &choice, sizeof(int));
	printf("User ID: ");
	scanf("%d", &id);
	write(fd, &id, sizeof(int));
	read(fd,&preresult,sizeof(result));
	if(preresult == 1){
		printf("Enter details to modify: \n");
		User user1;
		user1.id = id;
		printf("Name of the new account holder: ");
		scanf(" %[^\n]", user1.name1);
		printf("New password (max 15 characters): ");
		scanf("%s", user1.password);
		printf("New balance: ");
		scanf("%f", &user1.balance);
		strcpy(user1.type , "user");
		write(fd, &user1, sizeof(User));
	}
	else if(preresult == 2){
		User user2;
		user2.id = id;
		printf("Enter details to modify: \n");
		printf("Name of the new primary account holder: ");
		scanf(" %[^\n]", user2.name1);
		printf("Name of the new secondary account holder: ");
		scanf(" %[^\n]", user2.name2);
		printf("New password (max 10 characters): ");
		scanf("%s", user2.password);
		printf("New balance: ");
		scanf("%f", &user2.balance);
		strcpy(user2.type,"joint");
		write(fd, &user2, sizeof(User));
	}
	else if(preresult == 3){
		printf("Can not modify admin details\n");
	}
	else{
		printf("User ID provided doesn't exist\n");
	}
	
	// Response from the server
	if(preresult == 1 || preresult == 2 ){
		read(fd,&result,sizeof(result));

		if(result==-1){
			printf("User ID provided doesn't exist\n");
		}
		else{
			printf("The account was modified successfully\n");
		}
	}
	showMenu(fd);
}

// This functions sends a request to the server for searching an account
void searchAccount(int fd){
	int choice = 4;
	int type, len;
	bool result;

	write(fd, &choice, sizeof(int));

	User user1;
	int id;
	printf("User ID: ");
	scanf("%d", &id);
	write(fd, &id, sizeof(int));
	
	len = read(fd, &user1, sizeof(User));
	if(user1.id == -1){
		printf("Please re-check the User ID, Doesn't exist\n");
	}
	else{
		printf("User ID: %d\n", user1.id);
		printf("Name: %s\n", user1.name1);
		if(strcmp(user1.type, "joint") == 0)
		printf("Secondary account holder's name: %s\n", user1.name2);
		printf("Available balance: Rs.%f\n", user1.balance);
	}

	showMenu(fd);
}

void chooseOption(int fd){
	printf("Login:\n");
	UserLogin(fd);	
}

// This function shows the menu options specific to the type of user using the system
void showMenu(int fd){
	int choice;
	if(option==1 || option==2){
		printf("Choose one of the following options:\n");
		printf("1: Deposit\n");
		printf("2: Withdraw\n");
		printf("3: Balance Enquiry\n");
		printf("4: Password Change\n");
		printf("5: View Details\n");
		printf("6: Exit\n");

		scanf("%d",&choice);
    	printf("The option you selected was: %d\n", choice);

		switch(choice){
		case 1 :
			deposit(fd);
			break;
		case 2 :
			withdraw(fd);
			break;
		case 3 :
			balanceEnquiry(fd);
			break;
		case 4 :
			changePassword(fd);
			break;
		case 5 :
			viewDetails(fd);
			break;
		case 6 :
			write(fd,&select,sizeof(int));
			printf("Thank you for using the system!\n");
			exit(0);
		default :
			printf("Invalid option.\n");
			showMenu(fd);
			break;
		}
	}
    else if(option==3){
        printf("Choose one of the following options:\n");
		printf("1: Create Account\n");
		printf("2: Remove Account\n");
		printf("3: Modify Account\n");
		printf("4: Search Account\n");
		printf("5: Exit\n");

		scanf("%d",&choice);
		printf("The option you selected was: %d\n", choice);

		switch(choice){
		case 1 :
			createAccount(fd);
			break;
		case 2 :
			removeAccount(fd);
			break;
		case 3 :
			modifyAccount(fd);
			break;
		case 4 :
			searchAccount(fd);
			break;
		case 5 :
			write(fd,&choice,sizeof(int));
			printf("Thank you for using the system!\n");
			exit(0);
		default :
			printf("Invalid option.\n");
			showMenu(fd);
			break;
		}
	}
}


int main(){
	struct sockaddr_in server;
	int sd, msgLength;
	char buff[50];
	char result;

	// Establishing connection
	sd = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;

	//server.sin_addr.s_addr = inet_addr("172.16.81.54"); //other machine
	server.sin_addr.s_addr = INADDR_ANY; //same machine
	// server.sin_addr.s_addr = inet_addr("127.0.0.1"); //same machine
	server.sin_port = htons(5555);
	connect(sd, (struct sockaddr *)(&server), sizeof(server));

	chooseOption(sd);
	showMenu(sd);	

	close(sd);

	return 0;
}
















