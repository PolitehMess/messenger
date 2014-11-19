#include <iostream>
#include <cstring>

using namespace std;

struct Msg
{
	string otkogo;
	string komu;
	string message;
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

int main()
{
	string message="message";
	string nameto="komu";
	string namefrom="otkogo";
    Msg m;
    int len=13+message.size()+namefrom.size()+nameto.size();

	unsigned char a[len];
	pars(namefrom,nameto,message,a);
	findresult(a,m);
	
	cout<<m.otkogo<<endl;
	cout<<m.komu<<endl;
	cout<<m.message<<endl;
	cout << message.size() << " " << nameto.size() << " " <<namefrom.size();
	return 0;
}
