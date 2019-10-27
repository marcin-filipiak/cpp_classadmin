/* 
 * File:   main.cpp
 * Author: mfilipiak
 *
 * Created on 21 maj 2013, 13:43
 * 
 * Licence: Creative Commons CC BY
 * 
 */

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>

using namespace std;

/*
 Ustawienie kursora we wskazanym miejscu
 */
void gotoxy(int x,int y){
  printf("%c[%d;%df",0x1B,y,x);
}

/*
 wykonuje w linii polecen i zwraca wynik
 */
std::string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

/*
 zamienia stringa na tablice, elementy rozdziela delimiter
 */
void splitstring(string text, string array[], char delimiter){
    //element tablicy
    int u = 0;
    //pobranie kazdego znaku
    for(int x=0; x<text.size();x++){
        //pobranie znaku
        char z = text[x];
        //znaleziono delimiter, bedzie kolejny element tablicy
        if (z == delimiter){
            //nastepny element
            u++;
        }
        else{
                array[u] = array[u]+z;
        }
    }    
}

/*
 usuwa spacje z tekstu
 wymagany #include <algorithm>
*/
string delSpaces(string str) 
{
   str.erase(remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

/*
 sprawdzenie czy user uruchamiajacy program jest rootem
 */
bool iam_root(){
    string user = exec("whoami");
    if (user.find("root") == 0) return true;
    else return false;
}

/*
 wybor usera z systemu
 */
string select_user(){
   string list =  exec("ls /home");
   string users[]={" "," "," "," "," "," "};
    
   splitstring(list,users,'\n');
   
   
   
    
   //wyswietlenie
   for (int x=0;x<6;x++){
       if (users[x]!=" "){
               cout<<x<<")"<<users[x]<<endl;
       }
   }
   cout<<"========================================="<<endl;
   cout<<"Wybierz uzytkownika:"<<endl;
   int x;
   cin>>x;
   return users[x];
}

/*
 Zalozenie folderu programu w /root
 Zainstalowanie skryptu user_renew.sh
 */
int install_script(string user_login){
    if (user_login != ""){
        mkdir("/root/class_admin",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        fstream plik( "/root/class_admin/user_renew.sh", std::ios::out);

        if( plik.good() == true )
        {
            cout << "Skrypt zapisany!" << endl;
            string fcmd;
            fcmd = "#!/bin/sh \n";
            fcmd += "rm -R /home/"+user_login+" \n";
            fcmd += "cp -R /root/class_admin/"+user_login+" /home/"+user_login+" \n";
            fcmd += "chmod 777 -R /home/"+user_login+" \n";

            plik.write( & fcmd[ 0 ], fcmd.length() );
        } 
        else {
                cout << "Brak uprawnien root'a!\n Zaloguj sie jako administrator i uruchom program ponownie.\n";
        }
    }
    else {
        cout << "Nie podano nazwy uzytkownika.";
    }
}

/*
 dodanie do crontab by wykonywal
 sie przy kazdym starcie komputera
 */
int install_crontab(){
    system("echo \"@reboot root sh /root/class_admin/user_renew.sh\" >>  /etc/crontab");
}

/*
 * 
 */
int main(int argc, char** argv) {
std::system("clear");
//jesli jest rootem
if (iam_root()){
    gotoxy(1,6);
    cout<<"========================================="<<endl;
    cout<<"         CLASS ADMINISTRATOR 1.2   "<<endl;
    cout<<"         (CC BY)Marcin Filipiak       "<<endl;
    cout<<"             NoweEnergie.org       "<<endl;
    cout<<"========================================="<<endl;
    cout<<"Program zabezpieczajacy konto uzytkownika"<<endl;
    cout<<"przed wprowadzaniem przez niego zmian."<<endl;
    cout<<"========================================="<<endl;
    
    string user_login = select_user();
    user_login = delSpaces(user_login);
    
    //std::string user_login;    
    //getline( std::cin, user_login );    

    int menu;  
    std::string cc;
    do {
        std::system("clear");
        cout<<"1-instalacja\n2-backup stanu '"+user_login+"' do ktorego bedzie przywracany\n3-Uruchamiaj przywracanie przy starcie komputera\n4-koniec\n";  
        cin>>menu; 
        switch( menu )
        {
            case 1:
                install_script(user_login);
                break;
            case 2:
                 cc = "cp -R /home/";
                 cc +=user_login;
                 cc +=" /root/class_admin/";
                 system(cc.c_str());
                 break;
            case  3:
                install_crontab();
                break;
            default:
                break;
        }
    } while (menu!=4);  
    std::system("clear");
}
//jesli nie jest rootem
else {
    cout<<"\nUruchom program jako root.\n";
}
return 0;
}
