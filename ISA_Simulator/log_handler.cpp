#include <bits/stdc++.h>
#include <ctime>
#include <iomanip>

using namespace std;

void logger1(string user_entry)
{
     ofstream logfile("log.txt", ios::app);

     if (!logfile.is_open())
     {
          cerr << "Error: Could not open log file\n";
          return;
     }

     time_t now = time(0);
     tm *ltm = localtime(&now);

     logfile << "["
             << 1900 + ltm->tm_year << "-"
             << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
             << setw(2) << setfill('0') << ltm->tm_mday << " "
             << setw(2) << setfill('0') << ltm->tm_hour << ":"
             << setw(2) << setfill('0') << ltm->tm_min << ":"
             << setw(2) << setfill('0') << ltm->tm_sec
             << "] ";

     logfile << user_entry << endl;

     logfile.close();
}
