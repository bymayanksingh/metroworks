//run using g++ -std=c++11 Dijkstras.cpp

#include<bits/stdc++.h>
using namespace std;

template<typename T>
class Graph{
    
    map<T, list<pair<T,float> > > m;
    
public:

    void addEdge(T u,T v,float dist,bool bidir=true)
    {
        m[u].push_back(make_pair(v,dist)); 
        if(bidir)
        {
            m[v].push_back(make_pair(u,dist));
        }
        
    }
    
    void DijkstraGetShortestPathTo(T destination, map<T,T> &prev)
    {
        list<T> path;
        for( ; destination!=""; destination = prev[destination])
        {
            path.push_back(destination);
        }
        path.reverse();
        copy(path.begin(), path.end(), ostream_iterator<T>(cout, "\n"));
    }

    void printAdj();
    void dijsktraSSSP(T,map<T,float> &dist, map<T,T> &prev);

};

    template<typename T>
    void Graph<T>::printAdj()
    {
        //Let try to print the adj list
        //Iterate over all the key value pairs in the map 
        for(auto j:m)
        {
            
            cout<<j.first<<"->";
            
            //Iterater over the list of cities
            for(auto l: j.second)
            {
                    cout<<"("<<l.first<<","<<l.second<<")";
            }
            cout<<endl;
        }
    
    }

    template<typename T>
    void Graph<T>::dijsktraSSSP(T src, map<T,float> &dist, map<T,T> &prev)
    {    
        
        set<pair<float, T> > s;
        //Set all distance to infinity
        prev.clear();
        for(auto j:m)
        {
            dist[j.first] = numeric_limits<float>::infinity();
            prev[j.first] = "";
        }
        
        //Make a set to find a out node with the minimum distance
        
        dist[src] = 0;
        s.insert(make_pair(0,src));
        
        while(!s.empty())
        {
            
            //Find the pair at the front.
            auto p =   *(s.begin());
            T node = p.second; // NODE
            
            float nodeDist = p.first; //NODEDIST
            s.erase(s.begin());
            //Iterate over neighbours/children of the current node
            for(auto childPair: m[node])
            {
                T dest = childPair.first;
                float weight = childPair.second;
                float distance_through_node = nodeDist + childPair.second;
                
                if(distance_through_node < dist[childPair.first])
                {    
                    //In the set updation of a particular is not possible
                    // we have to remove the old pair, and insert the new pair to simulation updation
                    
                    auto f = s.find( make_pair(dist[dest],dest));
                    if(f!=s.end())
                    {
                        s.erase(f);
                    }
                    //Insert the new pair
                    dist[dest] = distance_through_node;
                    prev[dest] = node;
                    s.insert(make_pair(dist[dest],dest));
                }
            }
        }
        //Lets print distance to all other node from src
        /*for(auto d:dist)
        {
            cout<<d.first<<",is located at distance of  "<<d.second<<endl;
        }*/
    }

int main()
{

    string source, destination;
    Graph<string> Metro;
    //red
    Metro.addEdge("Rithala","Netaji Subhash Place",5.2);
    Metro.addEdge("Netaji Subhash Place","Keshav Puram",1.2);
    Metro.addEdge("Keshav Puram","Kanhaiya Nagar",0.8);
    Metro.addEdge("Kanhaiya Nagar","Inderlok",1.2);
    Metro.addEdge("Inderlok","Shastri Nagar",1.2);
    Metro.addEdge("Shastri Nagar","Pratap Nagar",1.7);
    Metro.addEdge("Pratap Nagar","Pulbangash",0.8);
    Metro.addEdge("Pulbangash","Tis Hazari",0.9);
    Metro.addEdge("Tis Hazari","Kashmere Gate",1.1);
    Metro.addEdge("Kashmere Gate","Shastri Park",2.2);
    Metro.addEdge("Shastri Park", "Seelampur",1.6);
    Metro.addEdge("Seelampur","Welcome",1.1);
    //blue
    Metro.addEdge("Rajouri Garden","Ramesh Nagar" ,1);
    Metro.addEdge("Ramesh Nagar","Moti Nagar" ,1.2);
    Metro.addEdge("Moti Nagar","Kirti Nagar" ,1);
    Metro.addEdge("Kirti Nagar","Shadipur" ,0.7);
    Metro.addEdge("Shadipur","Patel Nagar" ,1.3);
    Metro.addEdge("Patel Nagar","Rajender Place" ,0.9);
    Metro.addEdge("Rajender Place","Karol Bagh" ,1);
    Metro.addEdge("Karol Bagh","Rajiv Chowk" ,3.4);
    Metro.addEdge("Rajiv Chowk","Barakhamba Road" ,0.7);
    Metro.addEdge("Barakhamba Road","Mandi House" ,1);
    Metro.addEdge("Mandi House","Pragati Maiden" ,0.8);
    Metro.addEdge("Pragati Maiden","Inderprastha" ,0.8);
    Metro.addEdge("Inderprastha","Yamuna Bank" ,1.8);
    //green
    Metro.addEdge("Madipur","Shivaji Park" ,1.1);
    Metro.addEdge("Shivaji Park","Punjabi Bagh" ,1.6);
    Metro.addEdge("Punjabi Bagh","Ashok Park" ,0.9);
    Metro.addEdge("Ashok Park","Inderlok" ,1.4);
    Metro.addEdge("Ashok Park","Sat Guru Ram Singh Marg"  ,1.1);
    Metro.addEdge("Sat Guru Ram Singh Marg","Kirti Nagar" ,1);
    //violet
    Metro.addEdge("Kashmere Gate","Lal Qila" ,1.5);
    Metro.addEdge("Lal Qila","Jama Masjid" ,0.8);
    Metro.addEdge("Jama Masjid","Delhi Gate" ,1.4);
    Metro.addEdge("Delhi Gate","ITO" ,1.3);
    Metro.addEdge("ITO","Mandi House" ,0.8);
    Metro.addEdge("Mandi House","Janptah" ,1.4);
    Metro.addEdge("Janptah","Central Secretariat" ,1.3);
    Metro.addEdge("Central Secretariat","Khan Market" ,2.1);
    Metro.addEdge("Khan Market","JL Nehru Stadium" ,1.4);
    Metro.addEdge("JL Nehru Stadium","Jangpura" ,0.9);
    //yellow
    Metro.addEdge("Vishwavidalaya","Vidhan Sabha" ,1);
    Metro.addEdge("Vidhan Sabha","Civil Lines" ,1.3);
    Metro.addEdge("Civil Lines","Kashmere Gate" ,1.1);
    Metro.addEdge("Kashmere Gate","Chandni Chowk" ,1.1);
    Metro.addEdge("Chandni Chowk","Chawri Bazar" , 1);
    Metro.addEdge("Chawri Bazar","New Delhi" ,0.8);
    Metro.addEdge("New Delhi","Rajiv Chowk" ,1.1);
    Metro.addEdge("Rajiv Chowk","Patel Chowk" ,1.3);
    Metro.addEdge("Patel Chowk","Central Secretariat" ,0.9);
    Metro.addEdge("Central Secretariat","Udyog Bhawan" ,0.3);
    Metro.addEdge("Udyog Bhawan","Lok Kalyan Marg" ,1.6);
    Metro.addEdge("Lok Kalyan Marg","Jor Bagh" ,1.2);
    map<string,float> dist; 
    map<string,string> prev;
    string sourcestn, deststn;
    cout<<"enter source station in capital case: ";
    getline(cin,sourcestn);
    cout<<"enter destination station in capital case: ";
    getline(cin,deststn);
    Metro.dijsktraSSSP(sourcestn, dist, prev);
    cout<<"Ditance from Madipur to Jangpura "<<dist[deststn]<<endl;
    cout<<"Path: "<<endl;
    Metro.DijkstraGetShortestPathTo(deststn,prev);
    return 0;
}
