#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<string.h>
#include<sstream>
#include<assert.h>
#include<fstream>

using namespace std;
#define MAXFD 20	//Size of fds array
#define PORT 6003


struct NameStruct{
	string name;
	bool flag;

	NameStruct()
	{

		flag=true;
	}
};

//function to convert whole routing table to string.
int portTable(int i){
        return (i%10)-1;
}

void fds_add(int fds[],int fd)	//Add a file descriptor to the fds array
{
	int i=0;
	for(;i<MAXFD;++i)
	{
		if(fds[i]==-1)
		{
	      fds[i]=fd;
		  break;
		}
	}
}

bool check_server_fd(int i){ //useful to make checks for server explicit
	return (i==1/*||i==2||i==3*/);
}

void numberToString(string &obj,int num){
   stringstream sobj;
   sobj<<num;
   sobj>>obj;
}

void stringToNumber(int &num,string obj){
	stringstream s(obj);
	s>>num;
}

/*void addClientPort(string &m1,int temp_port){
	string str_port;
	numberToString(str_port,temp_port);
    m1=m1+str_port;

}*/

///////////////////////////////////////////routing table

struct Rtable  //routing table
{  //bool for server is it utilize or not.
	//static int counter;
	string cname;    //of 3 char.
	int  client_port;
	int  server_port; //direct connected server
	int fds;//use this file descriptor to communicate //not shown to routing table.
	string path;
	int hop;


	// Rtable(){ 
	// }
		
	// Rtable(string obj){
	// }

	void insert(string obj,int counter,int fds=-1){//"con:C01,30391,65712,path,-1"  ,fds is int and assigned separately
       //////////////////////////clearing

	    this->client_port=-1;
		this->cname="\0";
		this->server_port=-1;
		this->hop=-1;
		this->path="\0";
		this->fds=-1;
	  /////////////////////////////////	

	  //printf("\nDemo tuts counter(%d) string(below) \n",counter);
	  //cout<<obj<<endl;
		
		
		this->fds=fds;
       // counter--;    ////////////////find error
        obj=obj.substr(obj.find(':')+1,obj.length());
        //cout<<obj;
        //obj.find(':');
        string converted_counter=obj.substr(0,obj.find(','));
        //cname="C";
        //if(counter<10) cname+="0";
        //numberToString(converted_counter,counter);
        cname+=converted_counter;

		//cout<<"\nthis is cname"<<cname<<endl;

        obj=obj.substr(obj.find(',')+1,obj.length());  //erase C01,

        
        stringToNumber(client_port,obj.substr(0,obj.find(',')));
        //cout<<client_port<<endl;
        
        obj=obj.substr(obj.find(',')+1,obj.length());

        stringToNumber(server_port,obj.substr(0,obj.find(',')));
        
        obj=obj.substr(obj.find(',')+1,obj.length());

        //path making code.
            this->path.push_back((65+portTable(PORT)));
            path+="->";
            path+=obj.substr(0,obj.find(','));
        //
        //cout<<path<<endl;
        
        obj=obj.substr(obj.find(',')+1,obj.length());
        stringToNumber(hop,obj);
        hop+=1;
        //cout<<cname<<":"<<hop;
	}


	void stringify(string &obj){  //obj contain output string.
        obj="con:";
        obj+=this->cname;
        obj+=",";
        string temp;
        numberToString(temp,this->client_port);
        obj+=temp;
        obj+=",";
        numberToString(temp,this->server_port);
        obj+=temp;
        obj+=",";
        obj+=this->path;
        obj+=",";
        numberToString(temp,this->hop);
        obj+=temp;
	}

    void Rstringify(string &obj){
        //string obj;
        obj+="    ";
        obj+=this->cname;
        obj+="       |    ";
        string temp;
        numberToString(temp,this->client_port);
        obj+=temp;
		if(temp.length()==4) obj+=" ";
        obj+="      |    ";
        numberToString(temp,this->server_port);
        obj+=temp;
		if(temp.length()==4) obj+=" ";
        obj+="      |  ";
        numberToString(temp,this->hop);
        obj+=temp;        
        obj+="   |";
        obj+=this->path;
    }


    void Rheadings(){
            cout<<"Routing table:-\n"
        <<"Client's Name|||Client's Port|||Server's Port|||Hops|||Path\n";

    }

};




int findClient(string cname,Rtable* arr,int counter){
        for(int i=0;i<counter ;i++){
            if(cname==arr[i].cname) return arr[i].fds;
        }
        assert(0);  //program will generate error message
        //return -1;
}


void displayRouter(Rtable* rarry,int counter){
	rarry[0].Rheadings();

	for(int i=0;i<counter;i++){
		string temp;
		rarry[i].Rstringify(temp);
		cout<<temp<<endl;

	}
}

////////////////////////////////////////////////////

void deletewithfds(Rtable* rarry,int fds,int &counter,string &deleted_cname){
  int index=-1;
  for(int i=0;i<counter;i++){
	  if(rarry[i].fds==fds){
		  index=i;
		  deleted_cname=rarry[i].cname;
	  } 

  }
      if(index==-1) {
		  cout<<"\nThere is error";
		  return ;
	  }

	//int alphacheck;
	  //scanf("asdasd(%d)",&alphacheck);
  
	for(int k=index;k<counter;k++){
	  rarry[k].cname="\0";
	  rarry[k].client_port=0;
	  rarry[k].server_port=0;
	  rarry[k].fds=0;
	  rarry[k].path="\0";
	  string temp;
	  rarry[k+1].stringify(temp);
	  //b/c is blindly append ->
	  

	  rarry[k].insert(temp,k+1,rarry[k+1].fds);
	  rarry[k].path.erase(0,3);
	  rarry[k].hop--;

	}
	//   rarry[index].cname="\0";
	//   rarry[index].client_port=0;
	//   rarry[index].server_port=0;
	//   rarry[index].fds=0;
	//   rarry[index].path="\0";
	//   string temp;
	//   rarry[counter-1].stringify(temp);
	//   rarry[index].insert(temp,index+1,rarry[counter-1].fds);
	  counter--;
   
	//cout<<"value of deleted_cname"<<deleted_cname<<endl;
}


void deletewithfdsbutname(Rtable* rarry,int fds,int &counter,string &deleted_cname,string butname){
  int index=-1;
  for(int i=0;i<counter;i++){
	  if(rarry[i].cname ==butname){
		  index=i;
		  deleted_cname=rarry[i].cname;
	  } 

  }
      if(index==-1) {
		  cout<<"\nThere is error";
		  return ;
	  }

	//int alphacheck;
	  //scanf("asdasd(%d)",&alphacheck);
  
	for(int k=index;k<counter;k++){
	  rarry[k].cname="\0";
	  rarry[k].client_port=0;
	  rarry[k].server_port=0;
	  rarry[k].fds=0;
	  rarry[k].path="\0";
	  string temp;
	  rarry[k+1].stringify(temp);
	  //b/c is blindly append ->
	  

	  rarry[k].insert(temp,k+1,rarry[k+1].fds);
	  rarry[k].path.erase(0,3);
	  rarry[k].hop--;

	}
	//   rarry[index].cname="\0";
	//   rarry[index].client_port=0;
	//   rarry[index].server_port=0;
	//   rarry[index].fds=0;
	//   rarry[index].path="\0";
	//   string temp;
	//   rarry[counter-1].stringify(temp);
	//   rarry[index].insert(temp,index+1,rarry[counter-1].fds);
	  counter--;
   
	//cout<<"value of deleted_cname"<<deleted_cname<<endl;
}


void resetNameOfnamearray(NameStruct* arr,string cname){
//counter =15;
  for(int i=0;i<15;i++){
	  if(arr[i].name==cname){
		  arr[i].flag=true;
	  }
  }
}

int firstMostNameOfnamearray(NameStruct* arr){
	for(int i=0;i<15;i++){
		if(arr[i].flag==true){
			return i;
		}
	}
	return -1;

}

void setNameOfnamearray(NameStruct* arr,string cname){
//counter =15;
  for(int i=0;i<15;i++){
	  if(arr[i].name==cname){
		  arr[i].flag=false;
	  }
  }

}


void nameListGenerator(NameStruct* arr,string &namelisttosend,string elsenametosend){
	for(int i=0;i<15;i++){
		if(arr[i].flag==false && !(arr[i].name==elsenametosend)){
				namelisttosend+=arr[i].name;
				namelisttosend+=",";
		}
	}
	// if(namelisttosend.length()!=0) namelisttosend.pop_back();
	//cout<<namelisttosend;
	
}

void generatedDNSreply(string &payload){
		string bymsg="[msg generated by DNS server]";
		//
			//logic of file reading and writing.
		///
		fstream handler;
		handler.open("DNSfile.txt",ios::in);
		string temp="domain not found";
		while(!handler.eof()){
				//handler.getline(temp,'\n');
				//getline(handler,temp,'\n');
				string dname;
				string ipvalue;
				handler>>dname;
				handler>>ipvalue;
				if(dname==payload){
					temp=ipvalue;
				};
		}
		payload=temp;
		payload+=bymsg;
}

int main()
{

//////////////
    bool isDNS=true;
	NameStruct namearray[15];
	for(int i=0;i<15;i++){
		namearray[i].flag=true;
		namearray[i].name="C";
		if(i<10) namearray[i].name+="0";
		string temp;
		numberToString(temp,i);
		namearray[i].name+=temp;
	}

	// for(int i=0;i<15;i++){
	// 	if(namearray[i].flag==true){
	// 		cout<<namearray[i].name<<endl;
	// 	}

	// }
	// //return 0;


//////////////
	
	//intizlizers for routing tables
	int counter=0;
    Rtable rarry[50];
	/////////////////////////


	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd!=-1);
	
    printf("sockfd=%d\n",sockfd);
    
	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(PORT);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int res=bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res!=-1);
	
	//Create listening queue
	listen(sockfd,5);
	
   //Define fdset collection
    fd_set fdset;
	
	//Define fds array
    int fds[MAXFD];
    int i=0;
    for(;i<MAXFD;++i)
    {
	  	fds[i]=-1;
    }
	
	//Add a file descriptor to the fds array
    fds_add(fds,sockfd);



	int sockfd2 = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd2 != -1 );

	//Set Address Information
	struct sockaddr_in saddr2;
	memset(&saddr2,0,sizeof(saddr2));
	saddr2.sin_family = AF_INET;
	saddr2.sin_port = htons(6001);
	saddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
	//Link to server
	int res2 = connect(sockfd2,(struct sockaddr*)&saddr2,sizeof(saddr2));
	assert(res2 != -1);

	fds_add(fds,sockfd2);

	printf("\ndebug:This is sockfd of server1(%d)",sockfd2);


	int sockfd3 = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd3 != -1 );

	//Set Address Information
	struct sockaddr_in saddr3;
	memset(&saddr3,0,sizeof(saddr3));
	saddr3.sin_family = AF_INET;
	saddr3.sin_port = htons(6002);
	saddr3.sin_addr.s_addr = inet_addr("127.0.0.1");
	//Link to server
	int res3 = connect(sockfd3,(struct sockaddr*)&saddr3,sizeof(saddr3));
	assert(res3 != -1);

	fds_add(fds,sockfd3);

	printf("\ndebug:This is sockfd of server1(%d)",sockfd3);


	// int sockfd4 = socket(AF_INET,SOCK_STREAM,0);	
	// assert(sockfd4 != -1 );

	// //Set Address Information
	// struct sockaddr_in saddr4;
	// memset(&saddr4,0,sizeof(saddr4));
	// saddr4.sin_family = AF_INET;
	// saddr4.sin_port = htons(6003);
	// saddr4.sin_addr.s_addr = inet_addr("127.0.0.1");
	// //Link to server
	// int res4 = connect(sockfd4,(struct sockaddr*)&saddr4,sizeof(saddr4));
	// assert(res4 != -1);

	// fds_add(fds,sockfd4);

	// printf("\ndebug:This is sockfd of server1(%d)",sockfd4);


	while(1)
    {
		FD_ZERO(&fdset);//Clear the fdset array to 0

		int maxfd=-1;

		int i=0;

		//For loop finds the maximum subscript for the ready event in the fds array
		for(;i<MAXFD;i++)
		{
			if(fds[i]==-1)
			{
				continue;
			}

			FD_SET(fds[i],&fdset);

			if(fds[i]>maxfd)
			{
				maxfd=fds[i];
			}
		}

		struct timeval tv={5,0};	//Set timeout of 5 seconds

		int n=select(maxfd+1,&fdset,NULL,NULL,&tv);//Selectect system call, where we only focus on read events
		if(n==-1)	//fail
		{
			perror("select error");
		}
		else if(n==0)//Timeout, meaning no file descriptor returned
		{
			//printf("time out\n");
		}
		else//Ready event generation
		{
		//Because we only know the number of ready events by the return value of select, we don't know which events are ready.
		//Therefore, each file descriptor needs to be traversed for judgment
			for(i=0;i<MAXFD;++i)
			{
				if(fds[i]==-1)	//If fds[i]==-1, the event is not ready
				{
					continue;
				}// continue for here.
				if(FD_ISSET(fds[i],&fdset))	//Determine if the event corresponding to the file descriptor is ready
				{
			   
				//There are two kinds of cases for judging file descriptors
			   
					if(fds[i]==sockfd)	//A file descriptor is a socket, meaning accept if a new client requests a connection
					{
						//accept
						struct sockaddr_in caddr;
						socklen_t len=sizeof(caddr);

						int c=accept(sockfd,(struct sockaddr *)&caddr,&len);	//Accept new client connections
						if(c<0)
						{
							continue;
						}
					
						printf("accept c=%d\n",c);
						fds_add(fds,c);//Add the connection socket to the array where the file descriptor is stored

						if(c==6) continue;
		//////////////////////////////////////
						////////////
						int tempm1_index=firstMostNameOfnamearray(namearray);
						
						string m1="con:";
						m1+=namearray[tempm1_index].name;
						setNameOfnamearray(namearray,namearray[tempm1_index].name);
						m1+=",";

						//sending connected client information to connected clients.
						string elsenametosend=namearray[tempm1_index].name;
						string namelisttosend;
						nameListGenerator(namearray,namelisttosend,elsenametosend);
						char char_namelisttosend[256];
						strcpy(char_namelisttosend,namelisttosend.c_str());
                        //cout<<"\nfinal checking"<<char_namelisttosend<<endl;
						//for(int i=4;i<maxfd;i++){
							send(c,char_namelisttosend,100,0);
						//}
						//changes are here						
						

							// struct sockaddr_in temp_sock;
							// socklen_t temp_len=sizeof(temp_sock);
							// getsockname(c, (struct sockaddr *) &temp_sock, &temp_len);
							// int myPort = ntohs(temp_sock.sin_port);
							// cout<<myPort<<endl;

						
						struct sockaddr_in address;
						socklen_t addrlen=sizeof(address);
								
						getpeername(c , (struct sockaddr*)&address ,(socklen_t*)&addrlen);   
						int temp_client_port=ntohs(address.sin_port);
									// printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
						string temp;
						numberToString(temp,temp_client_port);
						m1+=temp;
						numberToString(temp,PORT);
						m1+=",";
						m1+=temp;
						m1+=",dest,";
						numberToString(temp,-1);
						m1+=temp;
						//m1+=temp_
						//addClientPort(m1,temp_port);
						//m1.push_back(',');

						/////////////////////////////////////

						
						//m1+="6002"; //port number of client. file des is add against each client seperately.
						
						//cout<<m1<<"\n";
						rarry[counter].insert(m1,counter,c);
						counter++;
						string connection_msg;
						rarry[counter-1].stringify(connection_msg);
						
						
						
						//now this msg will be sent to all other in the class.
						//cout<<connection_msg;
						
						char connection_arr_msg[256];
						strcpy(connection_arr_msg,connection_msg.c_str());
						

						//cout<<connection_arr_msg;
						for(int i=0;i<maxfd;i++){
							
							//printf("\nindex(%d) value(%d)",i,fds[i]);
							//cout<<endl<<fds[i];
						
							if(i==1 || i==2 || i==3/*or(fds[i]==4,5,6,)*/){   //this is demo explicit check b/c of demo architechture
									
								send(fds[i],connection_arr_msg,100,0);
								
							}
						}
		////////////////////////////////////////////////

						displayRouter(rarry,counter);
						/////////
						
							// char buff[128] = {0};
							// printf("Please Input:");
							// fgets(buff,128,stdin);
							// if(strncmp(buff,"end",3) ==0 )
							// {
							// 	//break;
							// }
							// send(sockfd2,buff,strlen(buff),0);
							// memset(buff,0,128);

						//	recv(sockfd2,buff,127,0);
						//	printf("RecvBuff:%s\n",buff);
						//	printf("\n");

					}
					else   //Receive data recv when an existing client sends data
					{
						char buff[128]={0};
						int res=recv(fds[i],buff,127,0);
						if(res<=0)
						{
							if(counter==1){
								//cout<<"\nalpha debug\n";

								string deleted_cname;
								deleted_cname=rarry[0].cname;

								//
								resetNameOfnamearray(namearray,deleted_cname);
								//


								char chardeleted_msg[20];
								string deleted_msg="del:";
								deleted_msg+=deleted_cname;
								//cout<<"\nvalue of deleted value current"<<deleted_cname<<endl;
								strcpy(chardeleted_msg,deleted_msg.c_str());

								//critical msg to all 
									//paste your code
									send(4,chardeleted_msg,100,0);
									send(5,chardeleted_msg,100,0);
									send(6,chardeleted_msg,100,0);
								//crtical msg to all
								

								counter--;  //we overwrite on first index
								//displayRouter(rarry,counter);
							}
							else if(counter>1){
								string deleted_cname;
								//cout<<"\nbeta debug\n";
								deletewithfds(rarry,fds[i],counter,deleted_cname);

								//
								resetNameOfnamearray(namearray,deleted_cname);
								//sending deleting msg to connected server.
															

								//sending deleting msg to connected server.
								char chardeleted_msg[20];
								string deleted_msg="del:";
								deleted_msg+=deleted_cname;
								//cout<<"\nvalue of deleted value current"<<deleted_cname<<endl;
								strcpy(chardeleted_msg,deleted_msg.c_str()); 

								//critical msg send to all
								send(4,chardeleted_msg,100,0);
								send(5,chardeleted_msg,100,0);
								send(6,chardeleted_msg,100,0);
								//critical msg send to all
							} 


							/////////////////

							//////////////
							close(fds[i]);
							fds[i]=-1;
							printf("one client over\n");
							displayRouter(rarry,counter);
						}
						else
						{
							printf("recv(%d)=%s\n",fds[i],buff);	//Output Client Sent Information
                            int senderfds=fds[i];

							string temp(buff);
							string check_temp;
							check_temp=temp.substr(0,temp.find(':'));
							if(check_temp=="con")
							{ //this is msg for connection
								
								rarry[counter].insert(temp,counter,fds[i]);
								setNameOfnamearray(namearray,rarry[counter].cname);
								counter++;
								displayRouter(rarry,counter);
								
								string connection_msg;
								rarry[counter-1].stringify(connection_msg);
								
								//sending this connection msg to adjacent servers
								
								//cout<<connection_msg;  //for debuging

								char char_connection_msg[255];
								strcpy(char_connection_msg,connection_msg.c_str());
								for(int j=1;j<=3;j++){
									if(i!=j && fds[j]!=-1){
										send(fds[j],char_connection_msg,100,0);
									}
								}
								/////////
								displayRouter(rarry,counter);




							}
							else if(check_temp=="del"){
										//del:(C00clientname)

								//////this will be sent to other connected clients
								string del_msg(buff);
								char char_del_msg[255];
								strcpy(char_del_msg,del_msg.c_str());
								//^ dont interfare

								if(counter==1)
								{
									temp=temp.substr(temp.find(':')+1,temp.length());
									resetNameOfnamearray(namearray,temp);
									counter--;
								}
								else if(counter>1){
									temp=temp.substr(temp.find(':')+1,temp.length());
									int index=-1;
									//printf("\ncname(%s)\n",temp.c_str());//cout<<temp<<endl;

									resetNameOfnamearray(namearray,temp);

									for(int i=0;i<counter;i++){
										if(rarry[i].cname==temp){
											
											//cal delete msg here with all passing arguments
											index=i;
										}
									}

									string voidname;
									printf("\nvalue of file descriptor is(%d)\n",rarry[index].fds);
									deletewithfdsbutname(rarry,rarry[index].fds,counter,voidname,temp);

									// cout<<"\nThis is final index"<<index<<endl;
									// cout<<endl;
									displayRouter(rarry,counter);

								}


								//string del_msg(buff);
								//char char_del_msg[255];
								//strcpy(char_del_msg,del_msg.c_str());
								for(int j=1;j<=3;j++){
									if(i!=j && fds[j]!=-1){
										send(fds[j],char_del_msg,100,0);
									}
								}										
										
							}
							else if(check_temp=="DNS"){
								if(senderfds!=3 && senderfds!=4 && senderfds!=5 && senderfds!=6){
									string ownmsg(buff);
									string adder_temp;
									adder_temp+=":";
									for(int m=0;m<counter;m++){
										if(rarry[m].fds==senderfds){
											adder_temp+=rarry[m].cname;
										}
									}
									ownmsg.insert(ownmsg.length()-1,adder_temp);
									strcpy(buff,ownmsg.c_str());

								}
								
								//cout<<"\ninside dns check:"<<buff;
								
								if(isDNS==true){//not DNS server
									char charconverted_msg[256];
									int fdsofsource_client_name;

									string main_msg;

									string payload;
									string source_client_name(buff);
									
									source_client_name.erase(0,4);
									payload=source_client_name.substr(0,source_client_name.find(':'));
									source_client_name.erase(0,source_client_name.find(':')+1);
									source_client_name=source_client_name.substr(0,3);

									// cout<<"\npayload:"<<payload;
									// cout<<"\nsource client name:"<<source_client_name<<endl;
									//return 0;
									// for(int p=0;p<maxfd;p++){
									// 	if(rarry[p].cname==source_client_name){
									// 		fdsofsource_client_name=rarry[p].fds;
									// 	}
									// }
									generatedDNSreply(payload);

									main_msg+="msg:";
									main_msg+=source_client_name;
									main_msg+=":";
									main_msg+=payload;    //payload contains(ip[msg from DNS server])
									
									
									

									strcpy(charconverted_msg,main_msg.c_str());
									send(4,charconverted_msg,100,0);
									// string main_msg(buff);
									// string sender_address;
									// sender_address+=":";
									// for(int l=0;l<maxfd;l++){
									// 	if(rarry[l].fds==senderfds){
									// 		sender_address+=rarry[l].cname;
									// 	}
									// }
									// main_msg.insert(main_msg.length()-1,sender_address);
									// //sender_address+="]";
									// strcpy(buff,main_msg.c_str());
									// //cout<<"\ninitial dest:"<<buff<<endl;
									// send(4,buff,100,0);

								}



							}							
							else{  //this is msg of communication
								
							//////logic for sending latest
							int totemp;	
							temp.erase(0,4);
							temp=temp.substr(0,temp.find(':'));
							for(int i=0;i<counter;i++){
								if(rarry[i].cname==temp){
									totemp=rarry[i].fds;
								}
							}


							//adding sender address
							if(senderfds!=3 && senderfds!=4 && senderfds!=5 && senderfds!=6/*only file descriptor of direct clients*/){
								string main_msg(buff);
								string sender_address;
								sender_address+="[msg from:";
								for(int l=0;l<maxfd;l++){
									if(rarry[l].fds==senderfds){
										sender_address+=rarry[l].cname;
									}
								}
								sender_address+="]";
								if(main_msg.length()>9) main_msg.insert(main_msg.length()-1,sender_address);
								sender_address=main_msg;
								memset(buff,0,128);
								strcpy(buff,sender_address.c_str());
							}
														

							send(totemp,buff,100,0);





							}
							//cout<<check_temp;
							//cout<<endl<<maintomain;
							//if( !check_server_fd(i) ) send(fds[i],"OK",2,0);	//Reply message to client
							// for(int j=0;j<maxfd;j++){
							// 	if(j!=i && fds[j]!=-1 && fds[j]!=sockfd){
							// 		send(fds[j],"msg from other client\n",2,0);
							// 	}
							// }



							//if( !check_server_fd(i) ) send(fds[i],"OK",2,0);	//Reply message to client
							// for(int j=0;j<maxfd;j++){
							// 	if(j!=i && fds[j]!=-1 && fds[j]!=sockfd){
							// 		send(fds[j],"msg from other client\n",2,0);
							// 	}
							// }
						}
					}
				}
			}
		}
	}
}





