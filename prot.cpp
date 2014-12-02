#include <iostream>
#include <cstring>
#include <vector>
#include <boost/shared_array.hpp>
using namespace std;

struct Msg
{
	string otkogo;
	string komu;
	string message;
};

void Convert(size_t size, char *a)
{
	
	if (size>=16250) throw "too much";
	size_t tmp,mod=0;
	tmp=size/127; 
	mod=size-127*tmp; 
	a[0]=tmp;
	a[1]=mod;
}

void pars(string namefrom,string nameto, string message, char* arr)
{
	arr[0]=1;
	arr[1]=0;
	char b[2];
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
	arr[13+namefrom.size()+nameto.size()+message.size()]='\0';
}

void Convert(size_t size, boost::shared_array<char> a)
{
	
	if (size>=16250) throw "too much";
	size_t tmp,mod=0;
	tmp=size/127; 
	mod=size-127*tmp; 
	a[0]=tmp;
	a[1]=mod;
}

class Error
{};

class Parser
{
public:
	vector<Msg> vmes;
	size_t Getlen(boost::shared_array<char>  a,size_t n)
	{
		return 127*static_cast<size_t>(a[n])+1*static_cast<size_t>(a[n+1]);
	}

	void vecttoarr(boost::shared_array<char> a)
	{
		for(size_t i=0; i<v.size(); i++)
			a[i]=v[i];
	}

	void parseData(char *data,size_t size)
	{
		Msg msg;
		Error err;
		if(size==0)
			return;
		for(size_t i = 0; i < size; i++)
			v.push_back(data[i]);
		while(true)
		{
			if(v.size()<4) // t.k. ne doshli daje do razmera
				return;
			boost::shared_array<char> a(new char[v.size()]);
			vecttoarr(a);//pishem vse chto v vectore v massiv, chtobi obrabativat'
			if(a[0]!=1){ 			//ne nasha versiya protokola
				throw err;
			}
			if(a[1]!=0){			//ne nasha versiya protokola
				throw err;
			}
			size_t len=Getlen(a,2); //dlinu ostavsheisya chasti poluchili, posze obshego size
			if(len<9) // nevozmojnii variant, t.k. minimum 9 yacheek
				throw err;
			if(v.size()<len+4)//ne prishlo polnoe soobshenie
				return;	
			if(a[4]==1)
			{
				size_t size=Getlen(a,5);//static_cast<size_t>(a[4]);
				if(size>0)
				{
					boost::shared_array<char> arr(new char [size+1]);
					for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
					{
						arr[j]=a[i];
					}
					arr[size]='\0';
					msg.otkogo=static_cast<string>(arr.get());
				}
			}
			else 
				if(a[4]==0)
				{
					size_t size=Getlen(a,5);//static_cast<size_t>(a[4]);
					if(size>0)
					{
						boost::shared_array<char> arr(new char [size+1]);
						for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
						{
							arr[j]=a[i];
						}
						arr[size]='\0';
						msg.komu=static_cast<string>(arr.get());
					}
				}
				else
					if(a[4]==2)
					{
						size_t size=Getlen(a,5);//static_cast<size_t>(a[4]);
						if(size>0)
						{
							boost::shared_array<char> arr(new char [size+1]);
							for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
							{
								arr[j]=a[i];
							}
							arr[size]='\0';
							msg.message=static_cast<string>(arr.get());
						}
					}
					else{
						throw err; //
					}
			if(a[7+Getlen(a,5)]==0)
			{
				size_t size=Getlen(a,8+Getlen(a,5));
				{
					boost::shared_array<char> arr(new char [size+1]);
					for(size_t i=10+Getlen(a,5), j=0;i<10+Getlen(a,5)+size;i++,j++)
					{
						arr[j]=a[i];
					}
					arr[size]='\0';
					msg.komu=static_cast<string>(arr.get());
				}
			}
			else
				if(a[7+Getlen(a,5)]==1)
				{
					size_t size=Getlen(a,8+Getlen(a,5));
					{
						boost::shared_array<char> arr(new char[size]);
						for(size_t i=10+Getlen(a,5), j=0;i<10+Getlen(a,5)+size;i++,j++)
						{
							arr[j]=a[i];
						}
						arr[size]='\0';
						msg.otkogo=static_cast<string>(arr.get());
					}
				}
				else
					if(a[7+Getlen(a,5)]==2)
					{
						size_t size=Getlen(a,8+Getlen(a,5));
						{
							boost::shared_array<char> arr(new char [size]);
							for(size_t i=10+Getlen(a,5), j=0;i<10+Getlen(a,5)+size;i++,j++)
							{
								arr[j]=a[i];
							}
							arr[size]='\0';
							msg.message=static_cast<string>(arr.get());
						}
					}
					else{
						throw err;
					}//
			if(a[10+Getlen(a,5)+Getlen(a,8+Getlen(a,5))]==2)
			{
				size_t size=Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)));
				if(size>0)
				{
					boost::shared_array<char> arr(new char [size+1]);
					for(size_t i=13+Getlen(a,5)+Getlen(a,8+Getlen(a,5)), j=0;i<13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))+size;i++,j++)
					{
						arr[j]=a[i];
					}
					arr[size]='\0';
					msg.message=static_cast<string>(arr.get());
				}
			}
			else
				if(a[10+Getlen(a,5)+Getlen(a,8+Getlen(a,5))]==1)
				{
					size_t size=Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)));
					if(size>0)
					{
						boost::shared_array<char> arr(new char [size]);
						for(size_t i=13+Getlen(a,5)+Getlen(a,8+Getlen(a,5)), j=0;i<13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))+size;i++,j++)
						{
							arr[j]=a[i];
						}
						arr[size]='\0';
						msg.otkogo=static_cast<string>(arr.get());
					}
				}
				else 
					if(a[10+Getlen(a,5)+Getlen(a,8+Getlen(a,5))]==0)
					{
						size_t size=Getlen(a,11+Getlen(a,5)+Getlen(a,8+Getlen(a,5)));//static_cast<size_t>(a[8+static_cast<size_t>(a[4])+static_cast<size_t>(a[6+static_cast<size_t>(a[4])])]);
						if(size>0)
						{
							boost::shared_array<char> arr(new char [size]);
							for(size_t i=13+Getlen(a,5)+Getlen(a,8+Getlen(a,5)), j=0;i<13+Getlen(a,5)+Getlen(a,8+Getlen(a,5))+size;i++,j++)
							{
								arr[j]=a[i];
							}
							arr[size]='\0';
							msg.komu=static_cast<string>(arr.get());
						}
					}
					else{
						throw err; //
					}
			//esli doshli do etogo momenta, to est' minimum 1 soobshenie i ego kidaem v vector, udalyaem iz vectora v elementi 1 soobsheniya
			vmes.push_back(msg);
			v.erase(v.begin(),v.begin()+len+4); //udalili vse elementi 1 soobsheniya
		}
	}
	size_t vectlen()
	{
		return v.size();
	}
private:
	vector<char> v;
};

int main()
{
	string message="message";
	string nameto="komu";
	string namefrom="otkogo";
	Msg m;
	size_t len=13+message.size()+namefrom.size()+nameto.size();
	char *a=new char[len];
	pars(namefrom,nameto,message,a);
	Parser parser;
	a[0]=0;
	
	try{
		for(size_t i=0; i<30; ++i)
		{
			parser.parseData(a+i,1);
			if(parser.vmes.size()!=0)
			{
				m=parser.vmes[0];
			}
		}
	}
	catch(Error )
	{
		cerr << "ne podderjivaetsya" << endl;
	}
	

	cout << parser.vectlen() << endl;
	cout<<m.otkogo<<endl;
	cout<<m.komu<<endl;
	cout<<m.message<<endl;

return 0;
}
