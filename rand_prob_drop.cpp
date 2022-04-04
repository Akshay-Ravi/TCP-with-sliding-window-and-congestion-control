#include<iostream>
#include <vector>
using namespace std;
int main()
{
    vector<int>dropped,seq;
    int no_of_packets=150,first_packet_number=3;
    int number_of_dropped=no_of_packets*0.01;
    bool flag=false;
    if(number_of_dropped>=1)
    {
        for(int i=0;i<number_of_dropped;i++)
            dropped.push_back(rand()%(no_of_packets+1)+first_packet_number);
        for(int i=first_packet_number;i<first_packet_number+1+no_of_packets;i++)
        {
            flag=false;
            for(int j=0;j<dropped.size();j++)
            {
                if(dropped[j]==i)
                {
                    flag=true;
                    break;
                }
            }
            if(flag==false) //it is not there
                seq.push_back(i);
        }
    }
    else
    {
        for(int i=first_packet_number;i<=first_packet_number+1+no_of_packets;i++)
            seq.push_back(i);
    }
    for(int packet:dropped)
        cout<<packet<<" ";
    cout<<endl;
    for(int packet:seq)
        cout<<packet<<" ";
    return 0;
}
