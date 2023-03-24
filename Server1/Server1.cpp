#include <iostream>
#include "Connection.h"
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char* argv[])
{
    
    if (argc == 1)
    {
        cout << "no flags argc!" << endl;
        cout << "Usage: Server.exe <ip> <port> <Folder>" << endl;
        exit(1);
    }
    string timeout_ = "500";//задаем таймаут
    Connection* con = new Connection(timeout_.c_str());
    con->InitServer(argv[1], atoi(argv[2]));//стартует сервер с TCP сокетом
    con->ReceiveServer();//получаем данные с портом udp от клиента
    int portudp = atoi(con->GetBuffer());
    con->InitServerUDP(argv[1], portudp); //стартует сервер с UDP сокетом
    con->ReceiveServer();//Получаем имя файла
    string FileName = con->GetBuffer();
    if (FileName.size() > FILENAME_MAX)
        cout << "File name is so long" << endl;
    con->ReceiveServer();//получаем кол-во строк в файле
    int j = atoi(con->GetBuffer());
    int i = 0, jj;
    std::string id = "";
    cout << "Starting transmission" << endl;//передача
    while (i < j)
    {
        jj = 0;
        con->ReceiveServerUDP();
        while (con->GetVec()[i][jj++] != 'n')
            ;
        while (con->GetVec()[i][jj] != 'E')
            id += con->GetVec()[i][jj++];//парсим строку
        con->SendServer(id);//выделяем id пакета и отправляем его клиенту
        cout << "i=" << i << endl;
        if ( (atoi(id.c_str()) == i) );//сравниваем id пакета и номер строки
        {
            i++;
        }
        id = "";
    }
    cout << "Transmission is end" << endl;
    ofstream out;          // поток для записи
    string path = argv[3];
    system("mkdir \"C:\\temp\\\"");//создаем директорию
    path += '\\' + FileName;
    if (path.size() > FILENAME_MAX)
        cout << "File name is so long" << endl;
    cout << "Creating file" << endl;
    out.open(path); // открываем файл для записи
    if (out.is_open())
    {
        string strfile = "";
        int jj;
        for (int i = 0; i < con->GetVec().size(); i++)
        {
            strfile = "";
            jj = 0;
            while (con->GetVec()[i][jj++] != 'd')
                ;
            for (jj; jj < con->GetVec()[i].size(); jj++)
                strfile += con->GetVec()[i][jj];//Вырезаем заголовки udp
            out << strfile;// и пишем в файл
        }
        out.close();
    }
    con->ServerClose();
    con->~Connection();
    cout << "Done!\n";
}