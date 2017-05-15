#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <queue>

using namespace std;

set <string> stopWords;
set <char> seprator;
vector <pair < pair< string, string >, vector <string> >> products;
unordered_map <string, vector <pair <string, vector <long long> >> > productMap;
unordered_map <string, vector <string> > productListings;
set <string> dictionary;
unordered_map <string, int> index;


//Returns the name and keywords of a product
pair < pair<string,string >, vector <string> > token(string s){

	vector <string> r;
	string name = "";
	string manu = "";
	bool ismanu = false;
	int counter = 0;
	string t = "";
	for(int i=0;i<s.size();i++){
		
		if( s[i] == '"'){
			counter++;
		}
		else if(counter == 3){
			name += s[i];
		}

		if(s[i]>='A' && s[i]<='Z'){
			s[i]=s[i]+('a'-'A');
	    }

		if( seprator.count(s[i]) != 0 ){
			if(t != "") {
				if(t == "announced" || t == "currency"){
					return make_pair(make_pair(name,manu), r);
				}
				if(stopWords.count(t) == 0){
					if(ismanu){
						manu = t;
						ismanu = false;
					}
					r.push_back(t);
				}
				else if(t == "manufacturer"){
					ismanu = true;
				}
			}
			t = "";
		} else {
			t += s[i];
		}
	}

	return make_pair(make_pair(name,manu), r);
}

vector <long long> mask(vector <string> v){
	vector <long long> r;
	for(int i=0;i<= (int)(dictionary.size()/64) ;i++){
		r.push_back(0ll);
	}
	for(int i=0;i<v.size();i++){
		auto it = index.find(v[i]);
		int in = (*it).second;
		r[in/64] |= (1ll<<(in%64));
	}

	return r;
}

string matchMask (string manu, vector <long long> v){
	priority_queue <pair <double, string> > q;

	auto pmap = productMap.find(manu);
	if(pmap == productMap.end())
		return "";
	

	for(int i=0;i<(*pmap).second.size();i++){

		double r1 = 0;
		double r2 = 0;

		for(int j=0;j<v.size();j++){
			r1 += __builtin_popcountl(v[j]&((*pmap).second[i].second[j]));
			r2 += __builtin_popcountl(((*pmap).second[i].second[j]));
		}

		q.push(make_pair((r1/r2),(*pmap).second[i].first));
	}

	if(q.size() == 0)
		return "";

	pair <double, string> p1 = q.top();
	q.pop();

	if(q.size() == 0)
		return p1.second;

	pair <double, string> p2 = q.top();
	q.pop();

	if(p1.first > p2.first && p1.first>=0.9){
		return p1.second;
	}

	return "";
}

int main(){

	ifstream fin("products.txt");

	stopWords.insert("product");
	stopWords.insert("name");
	stopWords.insert("manufacturer");
	stopWords.insert("model");
	stopWords.insert("family");
	stopWords.insert("date");
	stopWords.insert("title");

	seprator.insert('"');
	seprator.insert('{');
	seprator.insert('}');
	seprator.insert('(');
	seprator.insert(')');
	seprator.insert(':');
	seprator.insert('_');
	seprator.insert('-');
	seprator.insert(',');
	seprator.insert(' ');
	seprator.insert('/');

	string s;
	while(getline(fin, s)){
		
		pair<pair <string,string> , vector <string> > p = token(s);

		for(int i=0;i<p.second.size();i++){
			dictionary.insert(p.second[i]);
		}

		products.push_back(p);
	}

	int cou=0;
	for(auto it = dictionary.begin(); it != dictionary.end(); ++it){
		index.insert(pair<string,int>(*it,cou));
		cou++;
	}

	for(int i=0;i<products.size();i++){
		pair<string,string> name = products[i].first;
		vector <long long> productMask = mask(products[i].second);
		vector <string> tm;
		productListings.insert(pair<string,vector<string>>(name.first,tm));
		auto pmap = productMap.find(name.second);
		if(pmap == productMap.end()){
			vector <pair <string, vector <long long> >> t;
			t.push_back(make_pair(name.first,productMask));
			productMap.insert(pair<string, vector <pair <string, vector <long long> >> > (name.second,t));
		} else {
			(*pmap).second.push_back(make_pair(name.first,productMask));
		}
	}

	ifstream fin2("listings.txt");
	//int hit = 0;
	int cou2 = 0;

	while(getline(fin2, s)){
		cou2++;
		pair<pair<string,string> , vector <string> > p = token(s);
		vector <string> query;
		for(int i=0;i<p.second.size();i++){
			if(dictionary.count(p.second[i])!=0){
				query.push_back(p.second[i]);
			} else if (p.second[i].size()>2 && p.second[i].substr(p.second[i].size()-2,2) == "is") {
				if(dictionary.count(p.second[i].substr(0,p.second[i].size()-2))!=0){
					query.push_back(p.second[i].substr(0,p.second[i].size()-2));
					query.push_back("is");
				}
			}
		}

		vector <long long> queryMask = mask(query);
		string result;

		result = matchMask(p.first.second, queryMask);
		
		if(result != ""){
			auto prd = productListings.find(result);
			(*prd).second.push_back(s);
		}
	}


	for(int i=0;i<products.size();i++){
		string name = products[i].first.first;
		auto pmap = productListings.find(name);
		cout<<"{\"product_name\":\""<<name<<"\",\"listings\":[";
		if(pmap != productListings.end() and (*pmap).second.size() > 0){	
			for(int j=0;j<(*pmap).second.size()-1;j++){
				cout<<(*pmap).second[j]<<",";
			}
			cout<<(*pmap).second[(*pmap).second.size()-1];
		}
		cout<<"]}"<<endl;
	}

}