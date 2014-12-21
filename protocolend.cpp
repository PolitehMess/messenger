#include <iostream>
#include <boost/function.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>
#include <vector>

using namespace std;

struct Msg{
	string from;
	string to;
	string text;
};
struct Tab{
	int tag;
	boost::function< std::pair<vector<char>::iterator,Msg> (vector<char>:: iterator it, Msg msg, vector<char>:: iterator end)> func;
};

/*----------To pack the message--------*/

class wrongprot: public std::exception{
public:
	void print()
	{
		cerr << "Wrong version of protocol. " << endl;
	}
};
//to conver number for 2 cells
void Convert(size_t size, boost::shared_array<char> a)
{
	wrongprot err;
	if (size>=16256){
		throw err;
	}
	size_t tmp,mod=0;
	tmp=size/127; 
	mod=size-127*tmp; 
	a[0]=tmp;
	a[1]=mod;
}
//to pack the message
void pack(string namefrom,string nameto, string message, boost::shared_array<char> arr)
{
	arr[0]=1;
	arr[1]=0;
	boost::shared_array<char> b(new char[2]);
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
/*-------------------------------------------------------------------------------------------*/
//to find size
size_t Getlen(vector<char>::iterator &it,vector<char>::iterator end){
	wrongprot err;
	if(++it==end)
		throw err;
	--it;
	return 127*static_cast<size_t>(*it)+1*static_cast<size_t>(*(++it));
}

// Parser!
std::pair<vector<char>::iterator,Msg> ParseFrom(vector<char>::iterator &current, Msg &msg, vector<char>::iterator end){
	++current;
	size_t size=Getlen(current,end);
	++current;
	if((size)>0){
		boost::shared_array<char> arr(new char [size]);
		for(size_t i=0;i<size && current<end;i++)
		{
			arr[i]=*current;
			++current;
		}
		arr[size]='\0';
		msg.from=static_cast<string>(arr.get());
		current--;
		return make_pair(current,msg);
	}
	current--;
	return make_pair(current,msg);
}

std::pair<vector<char>::iterator,Msg> ParseTo(vector<char>::iterator &current, Msg msg, vector<char>::iterator end){
	++current;
	size_t size=Getlen(current,end);
	++current;
	if((*current)>0){
		boost::shared_array<char> arr(new char [size]);
		for(size_t i=0;i<size && current<end;i++)
		{
			arr[i]=*current;
			current++;
		}
		arr[size]='\0';
		msg.to=static_cast<string>(arr.get());
		current--;
		return make_pair(current,msg);
	}
	current--;
	return make_pair(current,msg);
}

std::pair<vector<char>::iterator,Msg> ParseText(vector<char>::iterator current, Msg &msg, vector<char>::iterator end){
	++current;
	size_t size=Getlen(current,end);
	++current;
	if((*current)>0){
		boost::shared_array<char> arr(new char [size]);
		for(size_t i=0;i<size && current<end;i++)
		{
			arr[i]=*current;
			current++;
		}
		arr[size]='\0';
		msg.text=static_cast<string>(arr.get());
		current--;
		return make_pair(current,msg);
	}
	current--;
	return make_pair(current,msg);
}

class Parser{
public:

	vector<Msg> messages(){
		return vmes;
	}

	void ParseData(char *data,size_t size){
		wrongprot err;
		if(size==0){
			return;
		}
		v.insert(v.end(), data, data + size); //put the data into he vector
		
		while(true){
			if(v.size()<2){
				return;
			}
			vector<char>::iterator current=v.begin();
			Msg msg;
			if(*(current)!=1){ 			//not our version of protocol
				throw err;
			}
			current++;
			if(*(current)!=0){			//not our version of protocol
				throw err;
			}
			current++;

			if(v.size()<4){ //there are not enough elements
				return;
			}

			size_t len=Getlen(current);		//size of line, it must be > than 9
			
			if(len<9){		// not enough for our protocol
				throw err; 
			}
			if(v.size()<len+4){
				return;
			}
			//iterator for distance
			vector<char>::iterator beg=current;
			while(true){
				PutTab();
				if(static_cast<size_t>(std::distance(beg,current))==len){			// if all flags = 1 then stop parsing
					vmes.push_back(msg);
					v.erase(v.begin(),++current); //iterator -> end()
					break;
				}
				if(static_cast<size_t>(std::distance(beg,current))>len){			// too much
					throw err;
				}
				current ++;

				vector<Tab>::iterator parser=find_if(tab.begin(),tab.end(), comp(*current));
				if(parser==tab.end()){ //not our version of protocol
					throw err;
				}

				std::pair<vector<char>::iterator,Msg> p=parser->func(current,msg,v.end());
				current=p.first;
				msg=p.second;
			}
		}
	}
private:
	class comp
	{
	public:
		int tags_;
		comp(int tags): tags_(tags) 
		{}
		bool operator()(const Tab &t)
		{
			return t.tag==tags_;
		}
	};
	size_t Getlen(vector<char>::iterator &it){
		wrongprot err;
		if(++it==v.end()){
			throw err;
		}
		--it;
		size_t len=127*static_cast<size_t>(*it)+1*static_cast<size_t>(*(++it));
		return len;
	}
	vector<Msg> vmes;
	vector<char> v;
	vector<Tab> tab;
	
	void PutTab(){
		Tab tabs;
		tabs.tag=0;
		tabs.func=ParseTo;
		tab.push_back(tabs);
		tabs.tag=1;
		tabs.func=ParseFrom;
		tab.push_back(tabs);
		tabs.tag=2;
		tabs.func=ParseText;
		tab.push_back(tabs);
	}
};

int main(){
	string message="all message that client senfd";
	string nameto="to someone wjdfahslhbfvafskjdn";
	string namefrom="from ssssssssssssssssssssssssss";
	size_t len=13+message.size()+namefrom.size()+nameto.size();
	boost::shared_array<char> a(new char[len]);
	pack(namefrom,nameto,message,a);
	Parser parser;
	try{
		for(size_t i=0; i<len; ++i)
		{
			parser.ParseData(a.get()+i,1);
		}
	}
	catch(wrongprot e)
	{
		e.print();
	}

	vector<Msg> m=parser.messages();

	cout << "From: " << m[0].from << endl;
	cout << "To: " << m[0].to << endl;
	cout << "Text: " << m[0].text << endl;

return 0;
}
