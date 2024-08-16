#define CURL_STATICLIB
#define BUILDING_LIBCURL
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <curl/curl.h>
#include <random>
#include <conio.h>
#include<iomanip>;
#include "json.hpp"



#include <iostream>
#include <string>
#include <algorithm>

#include <fstream>

#pragma comment (lib,"libcurl_debug.lib")
#pragma comment (lib,"wldap32.lib")
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib,"Crypt32.lib")
#pragma comment (lib, "winmm.lib")

using namespace std;
using json = nlohmann::json;


int AStart = -2;

/*
json


*/
struct card {
    int colour;  //黑桃 1 红桃 2 梅花3 方块4
    int num;

    friend void to_json(json& j, const card& c) {
        j = json{ {"colour", c.colour}, {"num", c.num} };
    }

    friend void from_json(const json& j, card& p) {
        j.at("colour").get_to(p.colour);
        j.at("num").get_to(p.num);
    }


};
vector<card> cards;
vector<card> mycards;
vector<card> hcards;
vector<card> lastcard;
bool coon = false;
int roomid;
bool player = false; //p1 true p2 false



//video
void noQuickEdit()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode &= ~ENABLE_INSERT_MODE;
    mode |= ENABLE_MOUSE_INPUT;
    SetConsoleMode(hStdin, mode);
    return;
}
void fullScreen(HWND hwnd)
{
    ShowWindow(hwnd, SW_MAXIMIZE);
}
void setFontSize(int size = 6)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;                   // Width of each character in the font
    cfi.dwFontSize.Y = size;                  // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void showCursor(bool state)
{
    // get buffer handle
    HANDLE hOutput;
    COORD coord = { 0, 0 };
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    // set visibility of the cursor
    CONSOLE_CURSOR_INFO cci = { 1,state };
    SetConsoleCursorInfo(hOutput, &cci);
}
inline void gotoxy(int x, int y)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)x, (short)y });
}

void playvideo(string target = "E:\\cards\\CardServer\\x64\\Debug\\output.txt", string tSound = "")
{
    noQuickEdit();
    showCursor(0);
    //fullScreen(FindWindowA("ConsoleWindowClass", "Console Video Player"));
    //setFontSize();

    //cout << "r";

    fstream fin;
    fin.open("output.txt");
    if (!fin.is_open())
    {
        cerr << "Failed to open target.\n";
        exit(1);
    }
    int fps;
    fin >> fps;
    register clock_t start, cur;
    register double delay = 1000.0 / fps;
    register unsigned long long currentFrame = 0;
    register char line[1024];
    memset(line, '\0', sizeof(line));



    ++AStart;
    //while (AStart) {};
    start = clock();
    int step = 0;
    while (!fin.eof())
    {
        ++currentFrame;
        gotoxy(0, 0);
        while (!fin.eof())
        {
            fin.getline(line, 1024);
            if (!strncmp(line, "[ENDFRAME]", 10))
            {
                break;
            }
            fputs(line, stdout);
            cout << "\n";
        }
        while (1.0 * (clock() - start) / CLOCKS_PER_SEC * 1000 / delay < currentFrame)
        {
            Sleep(1);

        }

        if (step / 1.95 >= 100) {
            break;
        }
        cout << "正在加载游戏 [";
        for (int i = 1; i <= 70; i++) {
            if (step / 1.95 >= i) {
                cout << "=";
            }
            else {
                cout << " ";
            }
        }
        cout << "] ";
        printf("%.5lf", step / 1.42);
        cout << "%";
        step = currentFrame / 15;
    }
}

//api 

//#define apiurl "http://tinywebdb.appinventor.space/api?user=cards&secret=014efc0c"

size_t write_data(void* buffer, size_t size, size_t nmemb, std::string* userp) {
    ((std::string*)userp)->append((char*)buffer, size * nmemb);
    return size * nmemb;
}

string getapi(string action, string tag, string value = "") {
    CURL* curl = curl_easy_init();
    if (curl) {
        string base = "http://tinywebdb.appinventor.space/api";
        string endpoint = "?user=cards&secret=014efc0c";
        string queryParam = "&action=";
        string tmp = "&tag=";
        string tmp2 = "&value=";
        string fullUrl = base + endpoint + queryParam + action + tmp + tag + tmp2 + value;
        //cout << fullUrl << endl;
        string response;
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            return "error";
        }
        else {
            //cout << "Response: " << response << endl;
            return response;
        }
        curl_easy_cleanup(curl);
    }
}


string dejson(string json, string title) {
    auto data = json::parse(json);
    return data[title];
}

string deapi(string action, string tag, string value = "") {
    if (action == "update" || action == "delete") {
        getapi(action, tag, value);
        return "";
    }
    return dejson(getapi(action, tag, value), tag);
}


//creat 


void washcard() {
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= 13; j++) {
            card now;
            now.colour = i;
            now.num = j;
            cards.push_back(now);
        }
    }

    for (int i = 1; i <= 1000; i++) {
        swap(cards[rand() % 51], cards[rand() % 51]);
    }


    vector<card> p1;
    vector<card> p2;
    int who = 1;
    for (auto i : cards) {
        if (who == 1) {
            p1.push_back(i);
            who++;
        }
        else if (who == 2) {
            p2.push_back(i);
            who++;

        }
        else if (who == 3) {
            who++;
        }
        if (who == 4) {
            who = 1;
        }

    }

    json data;
    data["cards"] = cards;
    data["p1c"] = p1;
    data["p2c"] = p2;

    data["p1online"] = true;
    data["p2online"] = false;
    data["run"] = false;

    data["who"] = "1";
    data["lastcard"] = lastcard;




    cout << data << endl;

    string tmp = data.dump();



    roomid = rand();
    string tmp2 = to_string(roomid);

    getapi("update", tmp2, tmp);


    cout << "房间名:" << roomid << endl;

}

bool wsmall(int a, int b) {
    if (a != b) {
        if (a == 2) {
            a = 1000;

        }
        if (a == 1) {
            a = 500;
        }
        if (b == 1) {
            b = 500;
        }
        if (b == 2) {
            b = 1000;
        }
        if (a > b) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

bool cheak(vector<card> now) {
    vector<card> la = lastcard;
    /*
    for (auto i : la) {
        cout << i.num << " ";
    }
    cout <<" 2"<< endl;
    for (auto i : now) {
        cout << i.num << " ";
    }
    cout << endl;*/
    cout << wsmall(2, 13);
    if (lastcard.size() == 0) {
        if (now.size() == 2 && now[0].num == now[1].num) {
            return true;
        }
        if (now.size() == 3 && now[0].num == now[1].num && now[1].num == now[2].num) {
            return true;

        }
        if (now.size() == 1) {
            return true;
        }
        return false;
    }
    if (now.size() == 4 && now[0].num == now[1].num && now[1].num == now[2].num && now[3].num == now[4].num) {
        return true;
    }
    if (now.size() == 2 && now[0].num == now[1].num) {
        if (la.size() == 2 && la[0].num == la[1].num) {
            if (wsmall(la[0].num , now[0].num)) {
                return true;
            }
        }
    }
    if (now.size() == 3 && now[0].num == now[1].num && now[1].num == now[2].num) {
        if (la.size() == 3 && la[0].num == la[1].num && la[1].num == la[2].num) {
            if (la[0].num < now[0].num) {
                return true;
            }
        }
    }
    if (now.size() == 1 && la.size() == 1 && wsmall(now[0].num , la[0].num)) {
        return true;

    }
    if (now.size() == 0) {
        return true;
    }
    return false;

}

void showcard() {

    for (auto i : mycards) {
        string t;

        if (i.colour == 1) {
            t = "黑桃";

        }
        else if (i.colour == 2) {
            t = "红桃";
        }
        else if (i.colour == 3) {
            t = "梅花";

        }
        else if (i.colour == 4) {
            t = "方块";

        }
        int op = i.num;


        if (op == 1) {
            t += "A";
        }
        else if (op == 11) {
            t += "J";
        }
        else if (op == 12) {
            t += "Q";
        }
        else if (op == 13) {
            t += "K";

        }
        else {
            t += to_string(op);
        }

        cout << t.c_str() << " ";

    }


}

void choose() {
flag:
    system("cls");
    vector<card> ou;

    int wz = 1;
    int qwz = -1;
    bool enter = true;
    while (true) {
        if (qwz != wz || enter == true) {
            system("cls");
            cout << wz << endl;
            cout << mycards.size() << endl;
            qwz = wz;
            showcard();
            cout << endl;

            for (int i = 1; i <= wz; i++) {
                if (i != wz) {

                    cout << "      ";
                }
                else {
                    cout << "    ^ ";
                }


            }
            cout << endl;
            cout << "上次对手出牌:";
            for (auto i : lastcard) {
                string t;

                if (i.colour == 1) {
                    t = "黑桃";

                }
                else if (i.colour == 2) {
                    t = "红桃";
                }
                else if (i.colour == 3) {
                    t = "梅花";

                }
                else if (i.colour == 4) {
                    t = "方块";

                }
                int op = i.num;


                if (op == 1) {
                    t += "A";
                }
                else if (op == 11) {
                    t += "J";
                }
                else if (op == 12) {
                    t += "Q";
                }
                else if (op == 13) {
                    t += "K";

                }
                else {
                    t += to_string(op);
                }

                cout << t.c_str() << " ";
            }
            cout << endl;
            cout << "当前已出 :";

            for (auto i : ou) {
                string t;

                if (i.colour == 1) {
                    t = "黑桃";

                }
                else if (i.colour == 2) {
                    t = "红桃";
                }
                else if (i.colour == 3) {
                    t = "梅花";

                }
                else if (i.colour == 4) {
                    t = "方块";

                }
                int op = i.num;


                if (op == 1) {
                    t += "A";
                }
                else if (op == 11) {
                    t += "J";
                }
                else if (op == 12) {
                    t += "Q";
                }
                else if (op == 13) {
                    t += "K";

                }
                else {
                    t += to_string(op);
                }

                cout << t.c_str() << " ";
            }
            enter = false;
        }



        //cout << endl;

        //A 75
        //D 77
        int ch;
        if (_kbhit()) {
            ch = _getch();
            if (ch == 75) {

                wz--;
                if (wz == 0) {
                    wz = 1;
                }
            }
            if (ch == 77) {
                if (wz == mycards.size()) {
                    wz = mycards.size() - 1;
                }
                wz++;

            }
            if (ch == 32 && mycards.size() != 0) {
                card tmmp;
                /*
                2
                1 2
                */
                tmmp = mycards[wz - 1];
                ou.push_back(tmmp);



                mycards.erase(mycards.begin() + wz - 1);

                wz = 1;




                enter = true;
                //mycards.erase(mycards.begin() + wz);
            }

            if (ch == 27) {
                break;
            }


        }



    }




    auto now = json::parse(deapi("get", to_string(roomid)));
    lastcard = now["lastcard"];

    if (!cheak(ou)) {
        MessageBox(NULL, L"无效出牌 \n 重新出牌", L"错误", MB_OK);
        goto flag;
    }

    now["lastcard"] = ou;
    if (player == true) {
        now["who"] = "2";
        now["p1c"] = mycards;
    }
    else {
        now["who"] = "1";
        now["p2c"] = mycards;
    }


    getapi("update", to_string(roomid), now.dump());


}

void play() {
    auto now = json::parse(deapi("get", to_string(roomid)));

    if (player == true) {
        mycards = now["p1c"];
        hcards = now["p2c"];
    }
    else {
        mycards = now["p2c"];
        hcards = now["p1c"];

    }


    while ((!mycards.empty()) || (!hcards.empty())) {
        Sleep(500);

        auto now1 = json::parse(deapi("get", to_string(roomid)));

        if (player == true) {
            mycards = now1["p1c"];
            hcards = now1["p2c"];
        }
        else {
            mycards = now1["p2c"];
            hcards = now1["p1c"];

        }

        auto now = json::parse(deapi("get", to_string(roomid)));
        if (now["who"] == "1") {
            if (player == true) {



                choose();
            }
            else {
                cout << endl;
                cout << "wait" << endl;
                while (true) {
                    Sleep(500);
                    auto nd = json::parse(deapi("get", to_string(roomid)));
                    if (nd["who"] == "2") {
                        lastcard = nd["lastcard"];
                        break;
                    }
                }

            }
        }
        else {
            if (player == false) {
                choose();
            }
            else {
                cout << "wait" << endl;
                while (true) {
                    Sleep(500);
                    auto nd = json::parse(deapi("get", to_string(roomid)));
                    if (nd["who"] == "1") {
                        lastcard = nd["lastcard"];
                        break;
                    }
                }
            }
        }
    }

}

void creatroom() {
    system("cls");
    washcard();

    player = true;

    cout << "等待对手加入" << endl;
    bool flag = false;
    while (true) {


        Sleep(2000);

        auto now = json::parse(deapi("get", to_string(roomid)));
        cout << now["p2online"] << endl;


        if (now["p2online"] == true) {
            flag = true;
            break;
        }

    }
    cout << "play" << endl;
    play();
}

void joinroom() {
    system("cls");
    cout << "房间名:";
    cin >> roomid;

    auto data = json::parse(deapi("get", to_string(roomid)));
    data["p2online"] = true;
    data["run"] = true;
    deapi("update", to_string(roomid), data.dump());

    player = false;

    play();

}

void showUi() {
    system("cls");
    if (coon == false) {
        cout << "测试连接" << endl;

        if (deapi("get", "ping") == "ping") {
            cout << "测试通过" << endl;
            system("cls");
            coon = true;
        }
        else {
            cout << "测试失败，检查网络连接" << endl;
            exit(0);
        }
    }

flag:

    cout << "1 创建房间" << endl;
    cout << "2 加入房间" << endl;
    cout << "选择操作" << endl;
    int op;
    cin >> op;
    if (op == 1) {
        creatroom();
    }
    else if (op == 2) {
        joinroom();
    }
    else {
        goto flag;
    }


}

int main() {

    //system("title by Gedh");
    // playvideo();
    srand((unsigned int)time(NULL));

    //creatroom();


    while (true) {
        showUi();
    }



    return 0;
}
