#include <iostream>
#include <thread>
#include <string>
#include <condition_variable>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>


class Buffer
{
  public:
	void PassLine(std::string &line)
	{
		std::unique_lock<std::mutex> lock(mtx);  
		buffer = line;
		data_ready = true;
		std::cout << "======== Данные переданы в буфер. ======== " << std::endl;
		cv.notify_all();
	}

	std::string GetLine()
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this](){return data_ready;}); 
		data_ready = false;
		return buffer;
	}

  private:
	std::string buffer;
	bool data_ready = false;
	std::mutex mtx;
	std::condition_variable cv;
};    

class ProcessData
{
public:
    void Process(Buffer &data)
    {
        while (true) {
            std::string line = data.GetLine();
            int sum = Sum(line);
            SendSum(sum);
        }
    }

private:
    int Sum(const std::string &line)
    {
        int ResultSum = 0;
        for (char c : line)
        {
            if (std::isdigit(c))
            {
                ResultSum += (c - '0');
            }
        }
        return ResultSum;
    }

    void SendSum(int sum)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        return;
    }

    std::string sum_str = std::to_string(sum);
    send(sock, sum_str.c_str(), sum_str.size(), 0);
    close(sock);
}
};


class InputCheck
{
  public:
	InputCheck(Buffer &data) : Data(data) {}

	void InputLine()
	{
		while (true)
		{
			std::cout << "\nВведите строку, состоящую из цифр, имеющую размер не более 64 символов:" << std::endl;
			std::cin >> line;
			Exit(line);
			int result = toggle(line); // переключатель проверяет строку на ключевую команду
			int ResultCheck = CheckLine(line); // переменная присваивает значение кода проверки

			if (ResultCheck != 0)
			{
				PrintError(ResultCheck, result); // после выполнения условия запускается функция вывода ошибки
				continue;						 // которая может быть отключена
			}
			
			else
			{
				std::cout << "======== Проверка пройдена =========\n";
				bubbleSort(line);	 // Сортировка по убыванию методом пузырька;
				Change(line);		 // Функция замены четных элементов строки;
				Data.PassLine(line); // Передача строки в buffer
			}
		}
	}

  private:
	Buffer &Data;
	std::string line; // Переменная для хранения вводимой строки
	bool flag = true; // Флаг для контроля переключателя вывод ошибок

	int CheckLine(const std::string &line)
	{
		size_t N = line.length();
		if (N > 64)
		{
			return -1;
		}
		for (size_t i = 0; i < N; ++i)
		{
			if (!std::isdigit(line[i]))
			{
				return 1;
			}
		}
		return 0;
	}

	void PrintError(int Error, int toggle)
	{
		if (flag && toggle != 0)
		{
			switch (Error)
			{
				case 1:
					std::cout << "Вы ввели строку, содержащую буквы, повторите попытку\n";
					break;
				case -1:
					std::cout << "Вы ввели более 64 символов, повторите попытку\n";
					break;
			}
		}
	}

	int toggle(const std::string &line)
	{
		if (line == "PrintErrorOn")
		{
			flag = true;
			std::cout << "\n========= Print error on =========\n" << std::endl;
			return 0;
		}
		else if (line == "PrintErrorOff")
		{
			flag = false;
			std::cout << "\n========= Print error off =========\n" << std::endl;
			return 0;
		}
		return 1;
	}

	void bubbleSort(std::string &line)
	{
		int n = line.length();
		for (int i = 0; i < n - 1; ++i)
		{
			for (int j = 0; j < n - i - 1; ++j)
			{
				if (line[j] < line[j + 1])
				{
					std::swap(line[j], line[j + 1]);
				}
			}
		}
	}

	void Change(std::string &line)
	{
		std::string ChangeLine;
		for (char c : line)
		{
			if ((c - '0') % 2 == 0)
			{
				ChangeLine += "KB";
			}
			else
			{
				ChangeLine += c;
			}
		}
		line = ChangeLine;
	}

	void Exit(std::string line){
		if (line == "Exit"){
			exit(EXIT_SUCCESS);
		}
	}
};

int main()
{
	setlocale(LC_ALL, "Russian");
	Buffer Data;
	InputCheck Input(Data);
	ProcessData Process_Data;

	std::thread th1(&InputCheck::InputLine, &Input); 
	std::thread th2(&ProcessData::Process, &Process_Data, std::ref(Data));

	th1.join();
	th2.join();

	return 0;
}