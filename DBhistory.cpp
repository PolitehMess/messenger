#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
using namespace std;

struct Msg
{
	string komu;
	string otkogo;
	string mes;
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
			o << m.mes;
			o << endl;
			o.close();
			return;
		}
		if(num==0)
		{
			ofstream o(name.c_str(),std::ios_base::app);
			o << left << setw(sizeof(m.otkogo)/sizeof(string)+2) << m.otkogo << ": ";
			o << m.mes;
			o << endl;
			o.close();
			return;
		}
		cout << "Smth goes wrong";
	}

private:
	string name;
};

int main()
{	
	Msg m,n;
	//Dannuyu structuru poluchili, t.e. nashe imya Igor
	m.komu="Igor";
	m.otkogo="Stas";
	m.mes="lalalalalall sadsaasdfczx qwefdas waeasd zswdasw dasgfzgdsgdfcxvserdgcvdf";
	
	for(int i=0;i<5;i++)
	{
		History obj(m.komu+m.otkogo);
		if(i==0)
			obj.DownHistory();
		obj.WrToHistory(m,from);
		obj.WrToHistory(m,to);
	}
	
	return 0;
}
