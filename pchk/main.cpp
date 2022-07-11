#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include "Rs232Device.h"
#include <windows.h>
#include "dt.h"


class Semaphore {
public:
	Semaphore(int count_ = 0)
		: count(count_)
	{
	}

	inline void notify(int tid) {
		std::unique_lock<std::mutex> lock(mtx);
		count++;
		std::cout << "thread " << tid << " notify" << std::endl;
		//notify the waiting thread
		cv.notify_one();
	}
	inline void wait(int tid) {
		std::unique_lock<std::mutex> lock(mtx);
		while (count == 0) {
			std::cout << "thread " << tid << " wait" << std::endl;
			
			//wait on the mutex until notify is called
			cv.wait(lock);
			std::cout << "thread " << tid << " run" << std::endl;
		}
		count--;
	}
private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};


int main()
{
	// Init
	Semaphore sem(1);

	Rs232Device device;

	device.Open();
	if (device.IsDeviceConnected()) {
		printf("Connected");
	}
	else {
		printf("No Connection");
	}
	device.CreateHandle("COM1");
	// Handle  wird mit 9600 baud initialisiert
	device.InitHandle(6, 8, device.EPARITY_NONE, device.STOPBITS_ONE, 1, 1);

	unsigned size;
	// Bei einer durchschnittlichen Telegramgröße von 8 byte passen in den buffer 32 Telegramme
	char input[256];
	//DT_ErrorCode validTelegram = false;
	//unsigned pos = 0;
	//unsigned char data[2] = { 0x00, 0x00 };
	DataTelegram dataTelegram;
	//dataTelegram.DtInit();
	//dataTelegram.DtCreate(0, false, false, 0, 0, 0x00, (char*)data, sizeof(data));
	//if (dataTelegram.DT_VALID == 0) {
	//	std::cout << dataTelegram.DtGetData() << std::endl;
	//}
	//std::cout << input << std::endl;
	/*
	std::thread s1([&]() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(5));
			sem.wait(1);
		}
	});
	*/
	// Serial Thread
	std::thread s2([&]() {
		while (device.IsReady()) {
			size = device.Read(input, sizeof(input));
			
			// Wenn keine 0 bytes gelesen wurden
			if (size > 0) {
				std::cout << "Bytes" << std::endl;
				//std::cout << input[1] << std::endl;
				dataTelegram.DtSetData(input, size);
			}
		
			
			//std::cout << "Dieser Thread ist aktiv" << std::endl;
			//sem.wait(2);
		}
	});
	/*
	std::thread s3([&]() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(600));
			sem.wait(3);
		}
	});
	*/
	/*
	std::thread s4([&]() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(5));
			sem.notify(4);
		}
	});
	*/


	//s1.join();
	
	s2.join();
	//s3.join();
	//s4.join();

}