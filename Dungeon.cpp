#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <random>
#include <cstdlib>
#include <ctime>
#include <chrono>
using namespace std;

static const double TIME_UNIT = 100;
static const string SEPERATE = "-------------------------------";
//dungeon
static const int DUNGEON_TYPES_NUM = 7;
static const int DUNGEON_DIVIDE = 100; 
struct Dungeon{
	int level;
	int lastTime;
	string name;
	bool operator>(const Dungeon& other) const{
		return this->level > other.level;
	}
	bool operator<(const Dungeon& other) const{
		return this->level < other.level;
	}
	bool operator==(const Dungeon& other) const{
		return this->level == other.level;
	}
};
const Dungeon DUNGEON_TYPES[DUNGEON_TYPES_NUM] = {{1, 30, "EASY"}, {2, 30, "MIDIUM"}, {3, 30, "HARD"}, 
												{4, 30, "SUPERHRAD"}, {5, 15, "SPECIAL"}, {6, 7, "BONUS"}, 
												{7, 180, "LOOP"}};
int transferIntoDungeonType(int code);

//event
class Event{
	friend ostream& operator<<(std::ostream& os, const Event& input);
public:
	Event(int event_num, Dungeon dungeon_type, int date){
		dungeon_ = dungeon_type;
		due_ = dungeon_.lastTime + date;
		event_num_ = event_num;
	}
	bool operator>(const Event& other) const{
		if (this->dungeon_ == other.dungeon_){
			return this->event_num_ > other.event_num_;
		}
		return this->dungeon_ > other.dungeon_;
	}
	bool operator<(const Event& other) const{
		if (this->dungeon_ == other.dungeon_){
			return this->event_num_ > other.event_num_;
		}
		return this->dungeon_ < other.dungeon_;
	}
	int getDue() const{
		return due_;
	}
	int getNum() const{
		return event_num_;
	}
	bool eventEnds(int date) const{
		return due_ == date;
	}
	void rebuildDue(int date){
		due_ = date += dungeon_.lastTime;
	}
private:
	int event_num_;
	int due_;
	Dungeon dungeon_;
};

ostream& operator<<(std::ostream& os, const Event& input) {
    return os << "{" << input.dungeon_.name << "}";
}

//main
int main(){
	//put many Event into the vector and the set in the order of level - 1
	
	//SETTINGS
	//vector
	vector<Event> events_vec;
	//set
	set<Event> events_set;
	//other settings
	const int EVENTS_NUMBER = 10;//first factor
	std::srand(std::time(nullptr));
	int date = 0;
	vector<Event> start_events;
	for (int i = 0; i < EVENTS_NUMBER; i++){
		int rd = rand() % DUNGEON_DIVIDE;
		start_events.push_back(Event(i, DUNGEON_TYPES[transferIntoDungeonType(rd)], date));
	}
	
	cout << "Proceeding Event Amount is " << EVENTS_NUMBER << endl; 
	cout << SEPERATE << endl;
	
	//BUILD
	//vector
	chrono::steady_clock::time_point startTime_vec, endTime_vec;
	startTime_vec = std::chrono::steady_clock::now();
	for (int i = 0; i < EVENTS_NUMBER; i++){//vector input
		bool done = false;
		if (i == 0){
			events_vec.push_back(start_events[i]);
		}
		else{//insertion sort
			for (int j = 0; j < i && !done; j++){
				if (start_events[i] < events_vec[j]){
					events_vec.insert(events_vec.begin() + j, start_events[i]);
					done = true;
				}
			}
			if (!done){
				events_vec.push_back(start_events[i]);
			}
		}
	}
	endTime_vec = std::chrono::steady_clock::now();
	chrono::duration<double> duration_vec = chrono::duration_cast<chrono::duration<double>>(endTime_vec - startTime_vec);
	//print
	cout << "[vector] used time unit = " << duration_vec.count() * TIME_UNIT << endl;
	for (const auto& v : events_vec){
		cout << v << " left " << v.getDue() - date << " days" << endl;
	}
	cout << SEPERATE << endl;
	
	//set
	chrono::steady_clock::time_point startTime_set, endTime_set;
	startTime_set = std::chrono::steady_clock::now();
	for (int i = 0; i < EVENTS_NUMBER; i++){//set input
		events_set.insert(start_events[i]);
	}
	endTime_set = std::chrono::steady_clock::now();
	chrono::duration<double> duration_set = chrono::duration_cast<chrono::duration<double>>(endTime_set - startTime_set);
	//print
	cout << "[set] used time unit = " << duration_set.count() * TIME_UNIT << endl;
	for (const auto& v : events_set){
		cout << v << " left " << v.getDue() - date << " days" << endl;
	}
	cout << SEPERATE << endl;
	
	
	//reduce the time of each Event for years - 2
	//SETTINGS
	const int RUN_THROUGH_DAY_NUM = 365;//second factor
	int limit_update_event_num = max(3, EVENTS_NUMBER / 20);//third factor
	int ready_event_num = RUN_THROUGH_DAY_NUM * limit_update_event_num;
	vector<Event> ready_events;
	int ready_event_next = 0;
	for (int i = 0; i < ready_event_num; i++){
		int rd = rand() % DUNGEON_DIVIDE;
		ready_events.push_back(Event(EVENTS_NUMBER + i + 1, DUNGEON_TYPES[transferIntoDungeonType(rd)], date));
	}
	
	//RUN
	//vector
	startTime_vec = std::chrono::steady_clock::now();
	{
		for (int i = 0; i < RUN_THROUGH_DAY_NUM; i++){
			date++;
			for (int j = 0; j < events_vec.size(); j++){
				if (events_vec[j].eventEnds(date)){
					events_vec.erase(events_vec.begin() + j);
					j--;
				}
			}
			//put on limited amount of new event everyday
			int already_put = 0;
			while (events_vec.size() < EVENTS_NUMBER && ready_event_next < ready_event_num && already_put < limit_update_event_num){
				bool done = false;
				ready_events[ready_event_next].rebuildDue(date);
				if (events_vec.size() == 0){
					events_vec.push_back(ready_events[ready_event_next]);
				}
				else{//insertion sort
					for (int j = 0; j < events_vec.size() && !done; j++){
						if (ready_events[ready_event_next] < events_vec[j]){
							events_vec.insert(events_vec.begin() + j, ready_events[ready_event_next]);
							done = true;
						}
					}
					if (!done){
						events_vec.push_back(ready_events[ready_event_next]);
					}
				}
				ready_event_next++;
				already_put++;
			}
		}
	}
	endTime_vec = std::chrono::steady_clock::now();
	duration_vec = chrono::duration_cast<chrono::duration<double>>(endTime_vec - startTime_vec);
	
	cout << "[vector] through "<< RUN_THROUGH_DAY_NUM <<" days used time unit = " << duration_vec.count() * TIME_UNIT << endl;//"<< endl << "
	for (const auto& v : events_vec){
		cout << v << " left " << v.getDue() - date << " days" << endl;
	}
	cout << SEPERATE << endl;
	
	//reset "date" and "next event"
	date = 0;
	ready_event_next = 0;
	
	//set
	startTime_set = std::chrono::steady_clock::now();
	{
		for (int i = 0; i < RUN_THROUGH_DAY_NUM; i++){
			date++;
			auto it = events_set.begin();
			while (it != events_set.end()) {
			    if (it->eventEnds(date)){
			    	it = events_set.erase(it);
				}
				else{
					it++;
				}
			}
			//put on limited amount of new event everyday
			int already_put = 0;
			while (events_set.size() < EVENTS_NUMBER && ready_event_next < ready_event_num && already_put < limit_update_event_num){
				ready_events[ready_event_next].rebuildDue(date);
				events_set.insert(ready_events[ready_event_next]);
				ready_event_next++;
				already_put++;
			}
		}
	}
	endTime_set = std::chrono::steady_clock::now();
	duration_set = chrono::duration_cast<chrono::duration<double>>(endTime_set - startTime_set);
	
	cout << "[set] through "<< RUN_THROUGH_DAY_NUM <<" days used time unit = " << duration_set.count() * TIME_UNIT << endl;//" << endl << "
	for (const auto& v : events_set){
		cout << v << " left " << v.getDue() - date << " days" << endl;
	}
	cout << SEPERATE << endl;
	
	return 0;
}

int transferIntoDungeonType(int code){//fourth factor
	if (0 <= code && code <= 19){// 20%
		return 0;
	}
	else if (20 <= code && code <= 39){// 20%
		return 1;
	}
	else if (40 <= code && code <= 59){// 20%
		return 2;
	}
	else if (60 <= code && code <= 69){// 10%
		return 3;
	}
	else if (70 <= code && code <= 84){// 15%
		return 4;
	}
	else if (85 <= code && code <= 97){// 13%
		return 5;
	}
	else{// 2%
		return 6;
	}
}