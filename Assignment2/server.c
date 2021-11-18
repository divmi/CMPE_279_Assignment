// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h> 
#include <sys/wait.h>
#define PORT 8090

int main(int argc, char const *argv[])
{   

    //printing argc value
    printf("argc value -->  %d ",argc);
    //end of argc
     if(argc == 1){
        int server_fd, new_socket;
        //valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1024] = {0};
        //char *hello = "Hello from server";
        const char * nobodyUser = "nobody"; 

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        if (bind(server_fd, (struct sockaddr *)&address,
        sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        
        // start
        printf("Creating fork\n");
        pid_t childId = fork();
        if( childId == 0 ){
            struct passwd *pw = getpwnam(nobodyUser);
            if (pw == NULL) {
                printf("Cannot find UID for name %s\n", nobodyUser);
                return -1;
            }
            int dropID = setuid(pw->pw_uid );

            if(dropID < 0){
                printf("Cannot drop privileges");
                return -1;
            }
            printf("Privileges dropped for child\n" );
            
            //Re-execing---------------------------------
            char fileDescriptor_arg [10];
            char port_arg [10];
            snprintf(fileDescriptor_arg,10, "%d", server_fd);
            snprintf(port_arg,10,"%d",PORT);

            char *args[]={fileDescriptor_arg, port_arg, NULL};

            //call execv
            execv("./server",args);

            //--------End---------------------------------

            return 0;

        }else if( childId < 0 ){
            printf("Cannot fork child process\n");
            return -1;
        } 
        else{
            //Wait for all child process to exit
            wait(NULL); // returns -1 if child exits
            printf("Child process terminated.\nParent process terminated.\n" );
        } 
        //End
     }else{
        // check if argc > 1
        //printing argc value
        printf("argc value --> %d ",argc);
        //end of argc
        int new_socket, valread;
        struct sockaddr_in address;
        int addressLength = sizeof(address);
        char buffer[1024] = {0};
        char *hello = "Hello from server";

        int server_fd = atoi(argv[0]);
        int port = atoi(argv[1]);

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( port );

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                    (socklen_t*)&addressLength))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
        }

        valread = read( new_socket , buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");

     }
}
