#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct User{
	int id;
	int old_id;
	char name1[100];
	char name2[100];
	char password[15];
	float balance;
	char type[20];
} User;

User getUser (int ID){
    int n = ID;
	User user;
	int fd = open("db", O_RDONLY, 0744);

	struct flock lock;
    // Setting read-lock for the user to edit the account
	lock.l_type = F_RDLCK; 
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);
	lock.l_len = sizeof(User);	             
	lock.l_pid = getpid();

    // Locking the selected record
	int ret = fcntl(fd, F_SETLKW, &lock);	

    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &user, sizeof(User));
	if(user.id != n) user.id = -1;
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);

	return user;
}

int checkUser (User user){
	int n = user.id ;
	int fd = open("db", O_RDONLY, 0744);
	int result;
	User temp;

	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    	     
	lock.l_len = sizeof(User);	            
	lock.l_pid = getpid();

    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	

    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &temp, sizeof(User));
	if(!strcmp(temp.password, user.password)) {
        if(strcmp(temp.type, "admin") == 0) {
            result = 3;
        }
        else if(strcmp(temp.type, "user") == 0) {
            result = 1;
        }
        else if(strcmp(temp.type, "joint") == 0) {
            result = 2;
		}
		else{
			result = -1;
		}
	}
	else {
        result = -1;
    }

	if(r == -1) result = -1;

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);

	return result;
}

bool depositMoney ( int ID, float amount) {
	int n = ID ;
	int fd = open("db", O_RDWR, 0744);
	bool result;
	struct flock lock;
    // Setting a write lock
	lock.l_type = F_WRLCK; 
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    
	lock.l_len = sizeof(User);	            
	lock.l_pid = getpid();
    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	
	User currUser;
    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &currUser, sizeof(User));
	currUser.balance += amount;
	lseek(fd, sizeof(User)*(-1), SEEK_CUR);
	write(fd, &currUser, sizeof(User));
	result = true;
	// printf("%d\n",fl);
	// getchar();
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);

	return result;
}

bool withdrawMoney ( int ID, float amount) {
	int n = ID ;
	int fd = open("db", O_RDWR, 0744);
	bool result;
	struct flock lock;
    // Setting a write lock
	lock.l_type = F_WRLCK; 
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    
	lock.l_len = sizeof(User);	            
	lock.l_pid = getpid();
    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	
	User currUser;
    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &currUser, sizeof(User));
    if(currUser.balance - amount>=0){
        currUser.balance -= amount;
        result = true;
    }else{
        result = false;
    }
	
	lseek(fd, sizeof(User)*(-1), SEEK_CUR);
	write(fd, &currUser, sizeof(User));
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
	return result;
	
}

float getBalance ( int ID) {
	int n = ID ;
    float result;
	int fd = open("db", O_RDONLY, 0744);
	struct flock lock;
    // Setting a read lock
	lock.l_type = F_RDLCK; 
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    
	lock.l_len = sizeof(User);	            
	lock.l_pid = getpid();
    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	
	User currUser;
    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &currUser, sizeof(User));
	result = currUser.balance;
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
	return result;
}

bool changePassword ( int ID, char newpassword[15]) {
	int n = ID ;
	int fd = open("db", O_RDWR, 0744);
	bool result;
	struct flock lock;
    // Setting a write lock
	lock.l_type = F_WRLCK; 
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    
	lock.l_len = sizeof(User);	            
	lock.l_pid = getpid();
    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	
	User currUser;
    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &currUser, sizeof(User));
	strcpy(currUser.password, newpassword);
	lseek(fd, sizeof(User)*(-1), SEEK_CUR);
	write(fd, &currUser, sizeof(User));
	result = true;
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
	return result;
}

int addUser (User record) {
	int fd = open("db", O_RDWR, 0744);
	int result;

	struct flock lock;
    // Setting a write lock
	lock.l_type = F_WRLCK;
    // Going to the end and reading the last record
	lock.l_whence = SEEK_END;
	lock.l_start = (-1)*sizeof(User);    
	lock.l_len = sizeof(User);	             
	lock.l_pid = getpid();

    // Locking the last record
	int fl = fcntl(fd, F_SETLKW, &lock);	

	User newUser;
    // Seeking to the position of the last record and reading
	lseek(fd, (-1)*sizeof(User), SEEK_END);  
	read(fd, &newUser, sizeof(User));
	if(newUser.id == -1) 
	record.id = newUser.old_id + 1;
	else record.id = newUser.id + 1;
    
	int ret = write(fd, &record, sizeof(User));
	if (ret == -1) {
        result = 0;
    }
	else {
        result = record.id;
    }

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);

	return result;
}


bool deleteUser (int ID) {
    int n = ID ;
	int fd = open("db", O_RDWR, 0744);
	bool result;

	struct flock lock;
    // Setting a write lock
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    
	lock.l_len = sizeof(User);	             
	lock.l_pid = getpid();

    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	

	User temp;
    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &temp, sizeof(User));
	if(temp.id != n){ 
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		return false;
	}
    temp.balance = 0;
	temp.id=-1;
	temp.old_id = n;
    lseek(fd, (-1)*sizeof(User), SEEK_CUR);
    int ret = write(fd, &temp, sizeof(User));
    if (ret != -1) {
        result = true;
    }
    else {
		printf("false");
        result = false;
    }

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);

	return result;
}


bool modifyUser (User user) {
    int n = user.id ;
	int fd = open("db", O_RDWR, 0744);
	bool result = false;

	struct flock lock;
    // Setting a write lock
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
    // Locking the nth record
	lock.l_start = n*sizeof(User);    
	lock.l_len = sizeof(User);	             
	lock.l_pid = getpid();

    // Locking the selected record
	int fl = fcntl(fd, F_SETLKW, &lock);	

	User newUser;
    // Seeking to the position of the record we want and reading
	lseek(fd, n*sizeof(User), SEEK_SET);  
	int r = read(fd, &newUser, sizeof(User));
	if(r == -1 || newUser.id != n) { 
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
		close(fd);
		return false;
	}
    lseek(fd, (-1)*sizeof(User), SEEK_CUR);
    int ret = write(fd, &user, sizeof(User));
    if (ret != -1) {
        result = true;
    }
    else {
        result = false;
    }
    

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	close(fd);

	return result;
}

void serverTask(int nsd){
	int choice, type, option, currUserID;
	bool result;
	while(1) {
		int result;
		User currUser1;
		read(nsd, &currUser1, sizeof(User));
		printf("UserID: %d\n", currUser1.id);
		printf("Password: %s\n", currUser1.password);
		currUserID = currUser1.id;
		result = checkUser(currUser1);
		write(nsd, &result, sizeof(result));
		if(result!=-1) {
			read(nsd, &option, sizeof(option));
			printf("Option: %d\n", option);
            break;
        }
	}

	while(1) {
		read(nsd, &choice, sizeof(int));
		if(option == 1 || option == 2) {
			if(choice == 1) {
				float amt;
				read(nsd, &amt, sizeof(float));
				result = depositMoney( currUserID, amt);
				write(nsd, &result, sizeof(result));
			}
			else if(choice == 2) {
				float amt;
				read(nsd, &amt, sizeof(float));
				result = withdrawMoney( currUserID, amt);
				write(nsd,&result,sizeof(result));
			}
			else if(choice == 3) {
				float amt;
				amt = getBalance( currUserID);
				printf("Balance = %f\n", amt);
				write(nsd, &amt, sizeof(float));
			}
			else if(choice == 4) {
				char pwd[15];
				read(nsd, pwd, sizeof(pwd));
				result = changePassword( currUserID, pwd);
				write(nsd, &result, sizeof(result));
			}
			else if(choice == 5) {
				User user1 = getUser(currUserID);
				write(nsd, &user1, sizeof(User));
			}
			else if(choice == 6) {
                break;
            }
		}
		else if(option == 3) {
			if(choice == 1) {
                int result;
				User newUser1;
				read(nsd, &newUser1, sizeof(User));
				result = addUser(newUser1);
				write(nsd, &result, sizeof(result));
			}
			else if(choice == 2) {
				int delUserID1;
				read(nsd, &delUserID1, sizeof(int));
				result = deleteUser(delUserID1);
				write(nsd, &result, sizeof(result));
			}
			else if(choice == 3) {
				User modUser1;
				int modUserID,preresult;
				read(nsd, &modUserID, sizeof(int));
				User user1 = getUser(modUserID);
				if(user1.id == -1){
					preresult = -1;
				}
				else if(strcmp(user1.type, "admin") == 0) {
        		    preresult = 3;
        		}
        		else if(strcmp(user1.type, "user") == 0) {
        		    preresult = 1;
        		}
        		else if(strcmp(user1.type, "joint") == 0) {
        		    preresult = 2;
				}
				else{
					preresult = -1;
				}
				write(nsd, &preresult, sizeof(result));
				if(preresult == 1 || preresult == 2){
					read(nsd, &modUser1, sizeof(User));
					result = modifyUser(modUser1);
					write(nsd, &result, sizeof(result));
				}
				
			}
			else if(choice == 4) {
				User searchUser1;
				int userID1;
				read(nsd, &userID1, sizeof(int));
				searchUser1 = getUser(userID1);
				write(nsd, &searchUser1, sizeof(User));
			}
			else if(choice == 5) {
                break;
            }
		}
	}
	close(nsd);
	printf( "Client session has come to an end\n");
}

void *connection_handler(void *nsd) {
	int nsfd = *(int*)nsd;
	serverTask(nsfd);
}

int main(){
	struct sockaddr_in server, client;
	int sd, nsd, len;
	pthread_t threads;
	
	sd = socket(AF_INET, SOCK_STREAM, 0); 

	server.sin_family = AF_INET; // internet domain
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(5555);

	bind(sd, (void *)(&server), sizeof(server)); // binds. gives it its local address. only server binds.
	listen(sd,5);	//queue length 5 : 5 clients per connection (this is the maximum possible) 

	printf("Waiting for the client.....\n");
	while(1){
        len = sizeof(client);
		nsd=accept(sd,(void *)(&client), &len);

		printf("Connected to the client.....\n");
		if (pthread_create(&threads, NULL, connection_handler, (void *)(&nsd)) < 0) {
			perror("thread fail");
			return 1;
		}
	}
	pthread_exit(NULL);
	close(sd);
	return 0;
}








