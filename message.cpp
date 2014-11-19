#include <iostream>
#include <cstring>

class Message
{
	private:
		std::string from;
		std::string to;
		std::string text;
		unsigned char* protocol_form;

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

	public:

		Message (std::string in_from,std::string in_to, std::string in_text)
		{
			from = in_from;
			to   = in_to;
			text = in_text;

			//Создаем протокольное сообщение
			protocol_form[0] = 1;
			protocol_form[1] = 0;

			unsigned char b[2];
			int num = 9 + from.size() + to.size() + text.size();
			Convert(num,b);

			protocol_form[2] = b[0]; //целая часть
			protocol_form[3] = b[1]; //остаток
			protocol_form[4] = 1;

			num = from.size();
			Convert(num,b);
			protocol_form[5] = b[0];
			protocol_form[6] = b[1];

			for(size_t i = 7, j = 0; i < 7+from.size(); i++, j++)
			{
				protocol_form[i] = from[j];
			}
			protocol_form[7 + from.size()] = 0;

			num = to.size();
			Convert(num,b);
			protocol_form[8 + from.size()] = b[0];
			protocol_form[9 + from.size()] = b[1];

			for(size_t i = 10+from.size(), j = 0; i<10+from.size()+to.size();i++, j++)
			{
				protocol_form[i] = to[j];
			}
			protocol_form[10 + from.size() + to.size()] = 2;

			num = text.size();
			Convert(num,b);
			protocol_form[11 + from.size() + to.size()] = b[0];
			protocol_form[12 + from.size() + to.size()] = b[1];
			for(size_t i = 13+from.size()+to.size(), j = 0; i<13+from.size()+to.size()+text.size();i++, j++)
			{
				protocol_form[i] = text[j];
			}
		}

		Message (unsigned char* a)
		{
			protocol_form = a;
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
							from=static_cast<std::string>(arr);
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
								to=static_cast<std::string>(arr);
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
									text=static_cast<std::string>(arr);
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
							to=static_cast<std::string>(arr);
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
								from=static_cast<std::string>(arr);
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
									text=static_cast<std::string>(arr);
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
							text=static_cast<std::string>(arr);
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
								from=static_cast<std::string>(arr);
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
									to=static_cast<std::string>(arr);
								}
							}
							else throw "Ne podderjivaetsya etot protocol";
				}
				else throw "Ne podderjivaetsya etot protocol";
			}
			else throw "ne podderjivaetsya etot protocol";
		}

		std::string get_from()
		{
			return from;
		}

		std::string get_to()
		{
			return to;
		}

		std::string get_text()
		{
			return text;
		}

		unsigned char* get_protocol_form()
		{
			return protocol_form;
		}

};

int main()
{
	//тестируем новый код
	Message msg1 ("Astrid","Eret","Careful what you wish for!");
	Message msg2 (msg1.get_protocol_form());
	std::cout << "From: "<<msg2.get_from()<<std::endl;
	std::cout << "To:"   <<msg2.get_to()  <<std::endl;
	std::cout << "Text:" <<msg2.get_text()<<std::endl;
	return 0;
}
