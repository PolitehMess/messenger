#include <iostream>
#include <cstring>
using namespace std;

struct  Msg
{
	string to;
	string from;
	string text;
};

class Message
{
	public:
		Message (Msg in_msg) 
		{
			msg = in_msg;
		}

		Message (string in_from, string in_to, string in_text)
		{
			msg.from = in_from;
			msg.to = in_to;
			msg.text = in_text;
		}

		Message (unsigned char * in_protocol_form)
		{
			unpack(in_protocol_form);
		}

		~Message()
		{}

		string get_from()
		{
			return msg.from;
		}

		string get_to()
		{
			return msg.to;
		}

		string get_text()
		{
			return msg.text;
		}

		Msg get_msg()
		{
			return msg;
		}
		
		unsigned char* get_protocol_form()
		{
			return pack();
		}

		Message operator = (Msg const & m)
		{
			msg.to=m.to;
			msg.from=m.from;
			msg.text=m.text;
			return msg;
		}

	private:

		Msg msg;

		void Convert(size_t size, unsigned char *a)
		{
			if (size >= 65500) throw "too much";
			size_t tmp,mod = 0;
			tmp = size/256; 
			mod = size-256*tmp; 
			a[0] = tmp;
			a[1] = mod;
		}

		size_t Getlen(unsigned char *a,size_t n)
		{
			return 256*static_cast<size_t>(a[n])+1*static_cast<size_t>(a[n+1]);
		}

		/*
		 * Баг функции pack()
		 * Утечка памяти на protocol_form
		 * мы не вызываем деструктор
		 * нужен умный указатель на массив с разделенным владением	
		 */

		unsigned char* pack()
		{
			int length = 13 + msg.to.size() + msg.from.size() + msg.text.size();
			unsigned char* protocol_form = new unsigned char[length];

			protocol_form[0]=1;
			protocol_form[1]=0;
			unsigned char b[2];
			int num=9+msg.from.size()+msg.to.size()+msg.text.size();
			Convert(num,b);
			protocol_form[2]=b[0]; //celaya chast'
			protocol_form[3]=b[1]; //ostatok
			protocol_form[4]=1;
			num=msg.from.size();
			Convert(num,b);
			protocol_form[5]=b[0];
			protocol_form[6]=b[1];
			for(size_t i=7, j=0; i<7+msg.from.size();i++, j++)
			{
				protocol_form[i]=msg.from[j];
			}
			protocol_form[7+msg.from.size()]=0;
			num=msg.to.size();
			Convert(num,b);
			protocol_form[8+msg.from.size()]=b[0];
			protocol_form[9+msg.from.size()]=b[1];
			for(size_t i=10+msg.from.size(), j=0; i<10+msg.from.size()+msg.to.size();i++, j++)
			{
				protocol_form[i]=msg.to[j];
			}
			protocol_form[10+msg.from.size()+msg.to.size()]=2;
			num=msg.text.size();
			Convert(num,b);
			protocol_form[11+msg.from.size()+msg.to.size()]=b[0];
			protocol_form[12+msg.from.size()+msg.to.size()]=b[1];
			for(size_t i=13+msg.from.size()+msg.to.size(), j=0; i<13+msg.from.size()+msg.to.size()+msg.text.size();i++, j++)
			{
				protocol_form[i]=msg.text[j];
			}
			return protocol_form;
		}

		void unpack(unsigned char * protocol_form)
		{
			if(protocol_form[0]==1 && protocol_form[1]==0)
				{
					size_t len=Getlen(protocol_form,2);//static_cast<size_t>(protocol_form[2]);
					if(len>9)
					{
						if(protocol_form[4]==1)
						{
							size_t size=Getlen(protocol_form,5);//static_cast<size_t>(protocol_form[4]);
							if(size>0)
							{
								char arr[size+1];
								for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
								{
									arr[j]=protocol_form[i];
								}
								arr[size]='\0';
								msg.from=static_cast<string>(arr);
							}
						}
						else 
							if(protocol_form[4]==0)
							{
								size_t size=Getlen(protocol_form,5);//static_cast<size_t>(protocol_form[4]);
								if(size>0)
								{
									char arr[size+1];
									for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
									{
										arr[j]=protocol_form[i];
									}
									arr[size]='\0';
									msg.to=static_cast<string>(arr);
								}
							}
							else
								if(protocol_form[4]==2)
								{
									size_t size=Getlen(protocol_form,5);//static_cast<size_t>(protocol_form[4]);
									if(size>0)
									{
										char arr[size+1];
										for(size_t i=7, j=0;i<7+size && j<size ;i++,j++)
										{
										arr[j]=protocol_form[i];
										}
										arr[size]='\0';
										msg.text=static_cast<string>(arr);
									}
								}
								else throw "Ne podderjivaetsya etot protokol!";
						if(protocol_form[7+Getlen(protocol_form,5)]==0)
						{
							size_t size=Getlen(protocol_form,8+Getlen(protocol_form,5));
							{
								char arr[size];
								for(size_t i=10+Getlen(protocol_form,5), j=0;i<10+Getlen(protocol_form,5)+size;i++,j++)
								{
									arr[j]=protocol_form[i];
								}
								arr[size]='\0';
								msg.to=static_cast<string>(arr);
							}
						}
						else
							if(protocol_form[7+Getlen(protocol_form,5)]==1)
							{
								size_t size=Getlen(protocol_form,8+Getlen(protocol_form,5));
								{
									char arr[size];
									for(size_t i=10+Getlen(protocol_form,5), j=0;i<10+Getlen(protocol_form,5)+size;i++,j++)
									{
										arr[j]=protocol_form[i];
									}
									arr[size]='\0';
									msg.from=static_cast<string>(arr);
								}
							}
							else
								if(protocol_form[7+Getlen(protocol_form,5)]==2)
								{
									size_t size=Getlen(protocol_form,8+Getlen(protocol_form,5));
									{
										char arr[size];
										for(size_t i=10+Getlen(protocol_form,5), j=0;i<10+Getlen(protocol_form,5)+size;i++,j++)
										{
											arr[j]=protocol_form[i];
										}
										arr[size]='\0';
										msg.text=static_cast<string>(arr);
									}
								}
								else throw "Ne podderjivaetsya etot protocol";

						if(protocol_form[10+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))]==2)
						{
							size_t size=Getlen(protocol_form,11+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5)));//static_cast<size_t>(protocol_form[8+static_cast<size_t>(protocol_form[4])+static_cast<size_t>(protocol_form[6+static_cast<size_t>(protocol_form[4])])]);
							if(size>0)
							{
								char arr[size];
								for(size_t i=13+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))/*Getlen(protocol_form,11+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5)))*/, j=0;i<13+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))+size;i++,j++)
								{
									arr[j]=protocol_form[i];
								}
								arr[size]='\0';
								msg.text=static_cast<string>(arr);
							}
						}
						else
							if(protocol_form[10+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))]==1)
							{
								size_t size=Getlen(protocol_form,11+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5)));//static_cast<size_t>(protocol_form[8+static_cast<size_t>(protocol_form[4])+static_cast<size_t>(protocol_form[6+static_cast<size_t>(protocol_form[4])])]);
								if(size>0)
								{
									char arr[size];
									for(size_t i=13+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))/*Getlen(protocol_form,11+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5)))*/, j=0;i<13+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))+size;i++,j++)
									{
										arr[j]=protocol_form[i];
									}
									arr[size]='\0';
									msg.from=static_cast<string>(arr);
								}
							}
							else 
								if(protocol_form[10+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))]==0)
								{
									size_t size=Getlen(protocol_form,11+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5)));//static_cast<size_t>(protocol_form[8+static_cast<size_t>(protocol_form[4])+static_cast<size_t>(protocol_form[6+static_cast<size_t>(protocol_form[4])])]);
									if(size>0)
									{
										char arr[size];
										for(size_t i=13+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))/*Getlen(protocol_form,11+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5)))*/, j=0;i<13+Getlen(protocol_form,5)+Getlen(protocol_form,8+Getlen(protocol_form,5))+size;i++,j++)
										{
											arr[j]=protocol_form[i];
										}
										arr[size]='\0';
										msg.to=static_cast<string>(arr);
									}
								}
								else throw "Ne podderjivaetsya etot protocol";
					}
					else throw "Ne podderjivaetsya etot protocol";
				}
				else throw "ne podderjivaetsya etot protocol";
		}
};

int main()
{
	Message msg1 ("Astrid","Eret","Careful what you wish for!");
	Message msg2 (msg1.get_protocol_form());
	std::cout << "From: "<<msg2.get_from()<<std::endl;
	std::cout << "To:   "<<msg2.get_to()  <<std::endl;
	std::cout << "Text: "<<msg2.get_text()<<std::endl;

	return 0;
}
