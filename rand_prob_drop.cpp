#include<iostream>
#include <vector>
using namespace std;
int main()
{
    vector<int>dropped,seq;
    int window_size=150,last_packet_number=3;
    int number_of_dropped=window_size*0.01;
    bool flag=false;
    if(number_of_dropped>=1)
    {
        for(int i=0;i<number_of_dropped;i++)
            dropped.push_back(rand()%(window_size+1)+last_packet_number);
        for(int i=last_packet_number;i<last_packet_number+1+window_size;i++)
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
        for(int i=last_packet_number;i<=last_packet_number+1+window_size;i++)
            seq.push_back(i);
    }
    for(int packet:dropped)
        cout<<packet<<" ";
    cout<<endl;
    for(int packet:seq)
        cout<<packet<<" ";
    return 0;
}