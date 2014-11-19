#include	"../key/unp.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
using namespace std;
struct Msg
{
	string otkogo;
	string komu;
	string message;
};

#define from 1
#define to 0

class History
{
public:
	History(string name_) : name(name_)
	{
		FILE *f;
		if((f=fopen(name.c_str(),"a"))==NULL)
		{
			cout << "Ne sozdalsya fail";
			exit(1);
		};
		fclose(f);
	}

	~History()
	{}

	void DownHistory()
	{
		ifstream i(name.c_str());
		if(!i)
		{
			cout << "File doesn't open";
			exit(1);
		}
		string ist;
		//proveryaem oustoi fail ili net
		i.seekg(0,ios_base::end);
		size_t size = i.tellg();
		i.seekg(0,ios_base::beg);
		//esle fail pustoi to vivodim pustuyu stroku
		if(size==0)
		{
			cout << endl;
			return;
		}
		while(!i.eof())
		{
			getline(i,ist,'\n');
			cout << ist << endl;	
		}
		i.close();
	}
	
	void WrToHistory(Msg const m, int num)
	{
		if(num==1)
		{
			ofstream o(name.c_str(),std::ios_base::app);
			o << left << setw(sizeof(m.komu)/sizeof(string)+2) << m.komu << ": ";
			o << m.message;
			o << endl;
			o.close();
			return;
		}
		if(num==0)
		{
			ofstream o(name.c_str(),std::ios_base::app);
			o << left << setw(sizeof(m.otkogo)/sizeof(string)+2) << m.otkogo << ": ";
			o << m.message;
			o << endl;
			o.close();
			return;
		}
		cout << "Smth goes wrong";
	}

private:
	string name;
};

void Convert(size_t size, unsigned char *a)
{
	
	if (size>=65500) throw "too much";
	size_t tmp,mod=0;
	tmp=size/256; 
	mod=size-256*tmp; 
	a[0]=tmp;
	a[1]=mod;
}

size_t Getlen(unsigned char *a,size_t n)
{
	return 256*static_cast<size_t>(a[n])+1*static_cast<size_t>(a[n+1]);
}
void pars(string namefrom,string nameto, string message, unsigned char* arr)
{
	arr[0]=1;
	arr[1]=0;
	unsigned char b[2];
	int num=9+namefrom.size()+nameto.size()+message.size();
	Convert(num,b);
	arr[2]=b[0]; //celaya chast'
	arr[3]=b[1]; //ostatok
	arr[4]=1;
	num=namefrom.size();
	Convert(num,b);
	arr[5]=b[0];
	arr[6]=b[1];
	for(size_t i=7, j=0; i<7+namefrom.size();i++, j++)
	{
		arr[i]=namefrom[j];
	}
	arr[7+namefrom.size()]=0;
	num=nameto.size();
	Convert(num,b);
	arr[8+namefrom.size()]=b[0];
	arr[9+namefrom.size()]=b[1];
	for(size_t i=10+namefrom.size(), j=0; i<10+namefrom.size()+nameto.size();i++, j++)
	{
		arr[i]=nameto[j];
	}
	arr[10+namefrom.size()+nameto.size()]=2;
	num=message.size();
	Convert(num,b);
	arr[11+namefrom.size()+nameto.size()]=b[0];
	arr[12+namefrom.size()+nameto.size()]=b[1];
	for(size_t i=13+namefrom.size()+nameto.size(), j=0; i<13+namefrom.size()+nameto.size()+message.size();i++, j++)
	{
		arr[i]=message[j];
	}
}

void findresult(unsigned char *a,Msg &msg)
{
	if(a[0]==1 && a[1]==0)
	{
		size_t len=Getlen(a,2);//static_cast<size_t>(a[2]);
		if(len>9)
		{
			if(a[4]==1)
			{
				size_t size=Getlen(a,5);//static_cast<size_t>(a[4]);
				if(size>0)
				{
					char arr[size+1];
					for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
					{
						arr[j]=a[i];
					}
					arr[size]='\0';
					msg.otkogo=static_cast<string>(arr);
				}
			}
			else 
				if(a[4]==0)
				{
					size_t size=Getlen(a,5);//static_cast<size_t>(a[4]);
					if(size>0)
					{
						char arr[size+1];
						for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
						{
							arr[j]=a[i];
						}
						arr[size]='\0';
						msg.komu=static_cast<string>(arr);
					}
				}
				else
					if(a[4]==2)
					{
						size_t size=Getlen(a,5);//static_cast<size_t>(a[4]);
						if(size>0)
						{
							char arr[size+1];
							for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
							{
							arr[j]=a[i];
							}
							arr[size]='\0';
							msg.message=static_cast<string>(arr);
						}
					}
					else throw "Ne podderjivaetsya etot protokol!";
			if(a[7+Getlen(a,5)]==0)
			{
				size_t size=Getlen(a,8+Getlen(a,5));
				{
					char arr[size];
					for(size_t i=10+Getlen(a,5), j=0;i<10+Getlen(a,5)+size;i++,j++)
					{
						arr[j]=a[i];
					}
					arr[size]='\0';
					msg.komu=static_cast<string>(arr);
				}
			}
			else
				if(a[7+Getlen(a,5)]==1)
				{
					size_t size=Getlen(a,8+Getlen(a,5));
					{
						char arr[size];
						for(size_t i=10+Getlen(a,5), j=0;i<10+Getlen(a,5)+size;i++,j++)
						{
							arr[j]=a[i];
						}
						arr[size]='\0';
						msg.otkogo=static_cast<string>(arr);
					}
				}
				else
					if(a[7+Getlen(a,5)]==2)
					{
						size_t size=Getlen(a,8+Getlen(a,5));
						{
							char arr[size];
							for(size_t i=10+Getlen(a,5), j=0;i<10+Getlen(a,5)+size;i++,j++)
							{
								arr[j]=a[i];
							}
							arr[size]='\0';
							msg.message=static_cast<string>(arr);
						}
					}
					else throw "Ne podderjivaetsya etot protocol";

			if(a[10+Getlen(a,5)+Getlen(a,8+Getlen(a,5))]==2)
			{
				size_t size=Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)));//static_cast<size_t>(a[8+static_cast<size_t>(a[4])+static_cast<size_t>(a[6+static_cast<size_t>(a[4])])]);
				if(size>0)
				{
					char arr[size];
					for(size_t i=13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))/*Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)))*/, j=0;i<13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))+size;i++,j++)
					{
						arr[j]=a[i];
					}
					arr[size]='\0';
					msg.message=static_cast<string>(arr);
				}
			}
			else
				if(a[10+Getlen(a,5)+Getlen(a,8+Getlen(a,5))]==1)
				{
					size_t size=Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)));//static_cast<size_t>(a[8+static_cast<size_t>(a[4])+static_cast<size_t>(a[6+static_cast<size_t>(a[4])])]);
					if(size>0)
					{
						char arr[size];
						for(size_t i=13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))/*Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)))*/, j=0;i<13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))+size;i++,j++)
						{
							arr[j]=a[i];
						}
						arr[size]='\0';
						msg.otkogo=static_cast<string>(arr);
					}
				}
				else 
					if(a[10+Getlen(a,5)+Getlen(a,8+Getlen(a,5))]==0)
					{
						size_t size=Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)));//static_cast<size_t>(a[8+static_cast<size_t>(a[4])+static_cast<size_t>(a[6+static_cast<size_t>(a[4])])]);
						if(size>0)
						{
							char arr[size];
							for(size_t i=13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))/*Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)))*/, j=0;i<13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))+size;i++,j++)
							{
								arr[j]=a[i];
							}
							arr[size]='\0';
							msg.komu=static_cast<string>(arr);
						}
					}
					else throw "Ne podderjivaetsya etot protocol";
		}
		else throw "Ne podderjivaetsya etot protocol";
	}
	else throw "ne podderjivaetsya etot protocol";
}


int main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
	std::cout << "lol";//	err_quit("usage: tcpcli <IPaddress>");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

std::cout<<"connection"<<std::endl;
	int i=0;
	Msg k;
	cout << "enter your name: ";
	cin >> k.otkogo;
	cout << "enter name for: ";
	cin >> k.komu;
	while(true)
	{

		string s;
		Msg m,n;
		m.otkogo=k.otkogo;
		m.komu=k.komu;
		History obj(k.komu+k.otkogo);
		if(i==0)
			obj.DownHistory();
		if(i!=0)
			cout << "Put the message: ";

		i+=1;
		getline(std :: cin,s);
		m.message=s;
		if(m.message.size()!=0)
			obj.WrToHistory(m,to);
		else continue;
		int len=13+m.message.size()+k.otkogo.size()+k.komu.size();
		unsigned char a[len];
		pars(m.otkogo,m.komu,m.message,a);
		write(sockfd, a, sizeof(a));	

		unsigned char line[10000];	
		read(sockfd,line,9999);
		findresult(line,n);
		obj.WrToHistory(n,from);
		std :: cout << n.otkogo << ": " << n.message << std :: endl;
	}
}
