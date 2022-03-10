/**
 * @file server.cpp
 * @author Stefan Gajdosik (xgajdo30@fit.vutbr.cz)
 * @brief Simple server
 * @version Sigma build 0.2.2
 * @date 2022-02-07
 * 
 * 
 */

#include <iostream>
using namespace std;
#include <sys/socket.h>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <cstring>
#include <strings.h>
#include <unistd.h>
#include <bits/stdc++.h>



//13
#define BUFFER_LEN 1000

/**
 * @brief Function for geting name of the processor
 * 
 * @param name variable in which name will be stored
 */
void get_proc_name(char* name) {
    FILE *f;
    f = fopen("/proc/cpuinfo", "r");
    char line[200]; 
    char string_to_find[] = "model name"; 
    while (!feof(f)) {
        fgets(line, sizeof(line), f); 
        if (strstr(line, string_to_find) != NULL) {
            int i = 13;
            int j = 0;
            while(line[i] != '\n'){
                name[j++] = line[i++];
            }
            name[j] = '\0';
            return;
        }
    }
    fclose(f);
}


vector <unsigned long long> get_CPU_times(){
    
    ifstream procesor_file("/proc/stat");
    vector <unsigned long long> result;
    string line;
    string aux;
    vector <unsigned long long> times;
    getline(procesor_file, line);
    stringstream parsed_line(line);

    /// first time purely for getting rid of "cpu" at start of the line
    parsed_line >> aux;
    while(parsed_line >> aux){
        times.push_back(stoll(aux));
    }

    times[0] -= times[8];
    times[1] -= times[9];
 
    unsigned long long idle = times[3] + times[4];
    unsigned long long nonidle = times[0] + times[1] + times[2] + times[5] + times[6] + times[7];
    unsigned long long totaltime = idle + nonidle;

   ////     user    nice   system  idle      iowait   irq   softirq  steal  guest  guest_nice
   ////cpu  0       1      2       3         4        5     6        7      8      9       


    result.push_back(idle);
    result.push_back(nonidle);
    result.push_back(totaltime);

    procesor_file.close();
    //for(int i =0;i<3;i++){
    //    cout << result[i] << flush;
    //    cout << "    " <<flush;
    //}
    return result;
}

double CPU_load(){
    vector <unsigned long long> previous = get_CPU_times();
    sleep(1);
    vector <unsigned long long> actual = get_CPU_times();

    /// [0] = idle
    /// [1] = nonidle
    /// [2] = total

    double totald = actual[2] - previous[2];
    double idled = actual[0] - previous[0];

    double result = ((totald - idled) * 100) / totald;

    return result;
}




/**
 * @brief Function to parse request
 * 
 * @param full_request full read from socket
 * @return string Parsed request 
 */
string find_request(string full_request){
    stringstream ss(full_request);
    
    string final_request;
    string aux;
    string aux2;
    /// lines of request
    vector <string> lines;
    /// words in first line of request
    vector <string> words;

    while(getline(ss, aux, '\n')){
        lines.push_back(aux);
    }
    final_request = lines[0];
    
    stringstream end(final_request);
    while(end >> aux2){
        words.push_back(aux2);
    }

    
    /// cheching if request is GET
    if(words[0] != "GET")return "ERROR";


    final_request = words[1];

    return final_request;
}




/**
 * @brief Main function for running server
 * 
 * @param argc Argument count
 * @param argv Argument array
 * @return status
 */
int main(int argc, char const **argv){
    
    if(argc != 2){
        fprintf(stderr, "Wrong number of start arguments!\n");
        return 1;
    }

    int PORT = atoi(argv[1]);
    

    int server_socket, new_socket; int optval;
    
    /// Processor name getting
    /// implementation limit 200 chars (should be enough for model name of processor)
    char * CPU_name = (char *)malloc(200);
    if(CPU_name == NULL){
        fprintf(stderr, "Unable to allocate memmory!\nInternal ERROR\n");
        exit(EXIT_FAILURE);
    }
    get_proc_name(CPU_name);
    /// alocated buffer for hostname
    char * host = (char *) malloc(200);
    if(host == NULL){
        fprintf(stderr, "Unable to allocate memmory!\nInternal ERROR\n");
        exit(EXIT_FAILURE);}
    gethostname(host, 200);
    
    


    /// Socket creating
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("ERROR in socket");
        exit(EXIT_FAILURE);
    }
    optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, (const void *)&optval, sizeof(int));

    /// socket addres variable
    struct sockaddr_in server_adress;

    int addr_len = sizeof(server_adress);

    /// Bind
    bzero((char * )&server_adress, sizeof(server_adress));
    server_adress.sin_family = AF_INET;
    server_adress.sin_addr.s_addr = htonl(INADDR_ANY);
    server_adress.sin_port = htons(PORT);

    if(bind(server_socket, (struct sockaddr *)&server_adress, sizeof(server_adress)) < 0){
        perror("ERROR In Bind");
        exit(EXIT_FAILURE);
    }  


     if (listen(server_socket, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }




    /// Server infinite loop
    while(1){

        if((new_socket = accept(server_socket, (struct sockaddr *)&server_adress, (socklen_t*)&addr_len))< 0){
            perror("ERROR In Accept");
            exit(EXIT_FAILURE);
        }
    
        /// base string 
        /// will be concatenated with content according to request
        string start = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; Content-Length: ";
        string end = "\r\n\r\n";
        
        
        
        double percentage;
        char buffer[BUFFER_LEN] = {0};
        
        /// reading incoming
        read(new_socket, buffer, BUFFER_LEN);

        string buffercpp = buffer;
        string reqest = find_request(buffercpp);
        string text;
        
        if(reqest == "/hostname") text = host;
        else if (reqest == "/cpu-name") text = CPU_name;
        else if (reqest == "/load"){
            percentage = CPU_load();
            text = to_string(percentage) + "%";
            }
        
        else {
            ///if request is unknown
            const char* reply = "HTTP/1.1 400 Bad Request\r\n\r\n";
            write(new_socket,reply, strlen(reply));
            close(new_socket);
        }

        int len = text.length();

        string reply_cpp = start + to_string(len) + end + text;
        
        
        const char* reply = reply_cpp.c_str();
        //cout << buffercpp;

        write(new_socket, reply, strlen(reply));
        close(new_socket);
    
    
    }

    
    free(CPU_name);
    return 0;
}
