#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<limits>
#include<sstream>
#include<ios>

using namespace std;

void headermaker(string &header,string clientnametocomm){
	if(clientnametocomm=="DNS")
	{
		header="DNS:";
		return;
	}
	header="msg:";
	header+=clientnametocomm;
	header+=":";

}

void removeheader(string& payload){

	payload.erase(0,8);
}

int main()
{
	char configbuff[256];
	printf("\nRouter1:Port Number 6001\nRouter2:Port Number 6002\nRouter3:Port Number 6003\nRouter4:Port Number 6004\nEnter port number to connect with router:");
	//return 0;
	
	int portnumber;
	cin>>portnumber;

	cout<<portnumber<<"."<<endl;
	// cin.clear();
	// cin.ignore(INT_MAX);
    
	//cout<<"\nport number"<<portnumber;


	int sockfd = socket(AF_INET,SOCK_STREAM,0);	
	
	assert(sockfd != -1 );

	//Set Address Information
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(portnumber);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//cout<<sockfd;
	//Link to server
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);

	//code to get port number
	struct sockaddr_in temp_sock;
	socklen_t temp_len=sizeof(temp_sock);
	getsockname(sockfd, (struct sockaddr *) &temp_sock, &temp_len);
	int myPort = ntohs(temp_sock.sin_port);
	//cout<<myPort<<endl;
	///

	cin.ignore(numeric_limits<streamsize>::max(),'\n');
	//cin.sync();
	
	 //avilable clients to communicate;
	recv(sockfd,configbuff,100,0); 
    cout<<"\nFollowing are available clients to communicate:-\n";
	printf("RecvBuff:%s\n",configbuff);   //C00,C02,C03 etc.
	cout<<"\nFor details see router table\n";

	//enter client name to communicate;
	//bool pathflag=true;
	
	
	
	//while(pathflag){

	//}


	while(1)
	{
		string header;  //this will be header to communicate
		
		cout<<"\nEnter client name to communicate or type DNS for DNS service:";
		string clientnametocomm;
		cin>>clientnametocomm;
		cin.ignore(numeric_limits<streamsize>::max(),'\n');
		headermaker(header,clientnametocomm);
		//
		if(clientnametocomm=="DNS"){ //dns communication
			char dnsname[128];
			cout<<"\nEnter domain to get IP:";
			fgets(dnsname,128,stdin);
			string str_dnsname(dnsname);
			str_dnsname=header+str_dnsname;
			strcpy(dnsname,str_dnsname.c_str());
			send(sockfd,dnsname,100,0);

			recv(sockfd,dnsname,127,0);

				//converting into string and removing header and display  it.
				string temp_buff(dnsname);
				removeheader(temp_buff);
				strcpy(dnsname,temp_buff.c_str());
				//now buff has only payload 

			printf("RecvBuff:%s\n",dnsname);
			printf("\n");

			continue;
		}

		//

		bool continuecommunication=true;
		cout<<"\nStart communication\n";
		do   //exit to exit.
		{
			
			char buff[128] = {0};
			printf("Please Input(or hit enter to recv msgs):");
			fgets(buff,128,stdin);
			if(strncmp(buff,"end",3) ==0 )
			{
				continuecommunication=false;
			}
			else{

				string tosend(header);
				string payload(buff);
				tosend+=payload;
				memset(buff,0,128);
				strcpy(buff,tosend.c_str());
				
				send(sockfd,buff,strlen(buff),0);
				memset(buff,0,128);
				recv(sockfd,buff,127,0);
				
				//converting into string and removing header and display  it.
				string temp_buff(buff);
				removeheader(temp_buff);
				strcpy(buff,temp_buff.c_str());
				//now buff has only payload 

				printf("RecvBuff:%s\n",buff);
				printf("\n");
				//cin.ignore(numeric_limits<streamsize>::max(),'\n');
			}

		}while(continuecommunication);
	}
	close(sockfd);
}