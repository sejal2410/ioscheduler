#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <cstring>
#include <cctype>
#include <string>
#include <deque>
#include <vector>
#include <algorithm>

using namespace std;
struct IO_req;
vector <IO_req*>io_requests;
int req_number=0;
int direction=1;
int curr_track=0;
int tot_movement=0;
int CURRENT_TIME=0;
bool isFlook=false;
deque<IO_req*> waitQueue;
deque<IO_req*> flookQueue;
deque<IO_req*> flookWaitQ;
//active_queu =
struct IO_req{
    int arrival_time;
    int track;
    int start_time = -1;
    int end_time = -1;
};

class ioScheduler{
public:
    virtual IO_req* getnextIO_from_waitqueue(){
        cout << "NO scheduler type specified" << endl;
        return NULL;
    }
    virtual void add_to_queue(IO_req* newIO){
        waitQueue.push_back(newIO);
    }
};
class FIFO : public ioScheduler{
public:
    IO_req*  getnextIO_from_waitqueue() {
            if(!waitQueue.empty()){
                IO_req* next_request = waitQueue.front();
                waitQueue.pop_front();
                if(next_request->track <= curr_track){
                    direction = -1;
                }
                else{
                    direction = 1;
                }
                return next_request;
            }
            return NULL;
    }

};

class SSTF : public ioScheduler{
public:
    IO_req*  getnextIO_from_waitqueue() {
        if(!waitQueue.empty()){
            IO_req* next_req=NULL;
            int shortest_seek=INT16_MAX;
            std::deque<IO_req*>::iterator req = waitQueue.begin(), remove;
            while ( req != waitQueue.end() ) {
                if(abs((*req)->track-curr_track)<shortest_seek){
                    shortest_seek = abs((*req)->track-curr_track);
                    next_req=(*req);
                    remove=req;
                }
                req++;
            }
            waitQueue.erase(remove);
            if(next_req->track>curr_track)
                direction=1;
            else direction=-1;
            return next_req;
        }
        return NULL;
    }

};
class LOOK : public ioScheduler{
public:
    IO_req*  getnextIO_from_waitqueue() {
        if(!waitQueue.empty()){
            IO_req* next_req=NULL;
            int shortest_seek=INT16_MAX;
            std::deque<IO_req*>::iterator req = waitQueue.begin(), remove;

            while ( req != waitQueue.end() ) {
                if( direction ==1  ){
                    if((*req)->track>=curr_track && ((*req)->track-curr_track)<shortest_seek){
                        shortest_seek = ((*req)->track-curr_track);
                        next_req=(*req);
                        remove=req;
                    }
                }
                else {
                    if ((*req)->track <= curr_track && (curr_track-(*req)->track ) < shortest_seek) {
                        shortest_seek = (curr_track-(*req)->track ) ;
                        next_req = (*req);
                        remove = req;
                    }
                }
                req++;
            }
            if(next_req==NULL){
                if(direction==1)
                    direction=-1;
                else direction=1;

                shortest_seek=INT16_MAX;
                req = waitQueue.begin();
                while ( req != waitQueue.end() ) {
                    if( direction ==1 ){
                        if((*req)->track>=curr_track && ((*req)->track-curr_track)<shortest_seek){
                            shortest_seek = ((*req)->track-curr_track);
                            next_req=(*req);
                            remove=req;
                        }
                    }
                    else {
                        if ((*req)->track <= curr_track && abs((*req)->track - curr_track) < shortest_seek) {
                            shortest_seek = abs((*req)->track - curr_track);
                            next_req = (*req);
                            remove = req;
                        }
                    }
                    req++;
                }
            }
            waitQueue.erase(remove);
            return next_req;
        }
        return NULL;
    }

};
class CLOOK : public ioScheduler{
public:
    IO_req*  getnextIO_from_waitqueue() {
        //int lowestTrackRequest=INT16_MAX, highestTrackRequest=-1;
     //   cout<<CURRENT_TIME<<"   ";
        if(!waitQueue.empty()){
            IO_req* next_req=NULL;
            int shortest_seek=INT16_MAX;
            std::deque<IO_req*>::iterator req = waitQueue.begin(), remove, lowestTrackRequest=waitQueue.begin();

            while ( req != waitQueue.end() ) {
                    if((*lowestTrackRequest)->track >=(*req)->track)
                        lowestTrackRequest = req;

                    if((*req)->track>=curr_track && ((*req)->track-curr_track)<shortest_seek){
                        shortest_seek = ((*req)->track-curr_track);
                        next_req=(*req);
                        remove=req;
                    }

                req++;
            }
            if(next_req==NULL){
               remove=lowestTrackRequest;
               next_req=(*lowestTrackRequest);
               direction= -1;
            }
            else
                direction=1;

            waitQueue.erase(remove);
         //   cout<<next_req->track<<endl;
            return next_req;
        }
        return NULL;
    }

};
deque<IO_req*>* add_queu;
deque<IO_req*>* active_queu;
class FLOOK: public ioScheduler{
public:
    FLOOK(){
        isFlook=true;
        active_queu = &flookQueue;
        add_queu = &waitQueue;
    }
    void add_to_queue(IO_req* newIO){
        add_queu->push_back(newIO);
    }

    IO_req*  getnextIO_from_waitqueue() {
        if(active_queu->empty()) {
            deque<IO_req*>* temp;
            temp=active_queu;
            active_queu = add_queu;
            add_queu=temp;
        }

        if(!active_queu->empty()){
            IO_req* next_req=NULL;
            int shortest_seek=INT16_MAX;
            std::deque<IO_req*>::iterator req = active_queu->begin(), remove;

            while ( req != active_queu->end() ) {
                if( direction ==1  ){
                    if((*req)->track>=curr_track && ((*req)->track-curr_track)<shortest_seek){
                        shortest_seek = ((*req)->track-curr_track);
                        next_req=(*req);
                        remove=req;
                    }
                }
                else {
                    if ((*req)->track <= curr_track && (curr_track-(*req)->track ) < shortest_seek) {
                        shortest_seek = (curr_track-(*req)->track ) ;
                        next_req = (*req);
                        remove = req;
                    }
                }
                req++;
            }
            if(next_req==NULL){
                if(direction==1)
                    direction=-1;
                else direction=1;

                shortest_seek=INT16_MAX;
                req = active_queu->begin();
                while ( req != active_queu->end() ) {
                    if( direction ==1 ){
                        if((*req)->track>=curr_track && ((*req)->track-curr_track)<shortest_seek){
                            shortest_seek = ((*req)->track-curr_track);
                            next_req=(*req);
                            remove=req;
                        }
                    }
                    else {
                        if ((*req)->track <= curr_track && abs((*req)->track - curr_track ) < shortest_seek) {
                            shortest_seek = abs((*req)->track - curr_track);
                            next_req = (*req);
                            remove = req;
                        }
                    }
                    req++;
                }
            }
            active_queu->erase(remove);
            return next_req;
        }
    }
};
void readInputFile(FILE* inputFile){

    static char linebuf[1024];
    const char* DELIM = " \t\n\r\v\f";

    while(fgets(linebuf,1024, inputFile)){
        if(linebuf[0] == '#')
            continue;
        IO_req* IO = new IO_req();
        char* at = strtok(linebuf, DELIM);
        int arrtime = atoi(at);
        IO->arrival_time = arrtime;
        char* tr = strtok(NULL, DELIM);
        int track = atoi(tr);
        IO->track = track;
        io_requests.push_back(IO);
    }
}

ioScheduler* IO_SCHEDULER;

void strategy(){
    IO_req* CURRENT_ACTIVE_IO=NULL;

    while(true){
        IO_req* req=NULL;
        if(req_number<io_requests.size())
           req = io_requests.at(req_number);
        if(req!=NULL && req->arrival_time==CURRENT_TIME) {
            IO_SCHEDULER->add_to_queue(req);
            req_number++;
        }
        if(CURRENT_ACTIVE_IO!=NULL && CURRENT_ACTIVE_IO->track==curr_track){
            CURRENT_ACTIVE_IO->end_time=CURRENT_TIME;
            CURRENT_ACTIVE_IO=NULL;
        }
        if(CURRENT_ACTIVE_IO==NULL){
            if((!waitQueue.empty() && !isFlook) || (isFlook && (!waitQueue.empty() || !flookQueue.empty()))){
                CURRENT_ACTIVE_IO = IO_SCHEDULER->getnextIO_from_waitqueue();
                CURRENT_ACTIVE_IO->start_time=CURRENT_TIME;

                if(CURRENT_ACTIVE_IO->track==curr_track)
                    continue;
            }
            else if(req_number>=io_requests.size()){
                break;
            }
        }
        if(CURRENT_ACTIVE_IO!=NULL){
            curr_track+=direction;
            tot_movement++;
        }
        CURRENT_TIME++;
    }
}

int main(int argc, char *argv[]){
    int option;
    int nofarg = 0;
    while ((option = getopt(argc, argv, "s:")) != -1) {
        nofarg++;
        char temp;
        char schedAlgo;
        if (option == 's') {
            // cout << optarg << endl;
            schedAlgo = optarg[0];
            if (schedAlgo == 'i') {
                IO_SCHEDULER = new FIFO();
              //  cout<<"FIFO\n";
            }
            else if (schedAlgo == 'j') {
                IO_SCHEDULER = new SSTF();
            }
            else if (schedAlgo == 's') {
                IO_SCHEDULER = new LOOK();
            }
            else if (schedAlgo == 'c') {
                IO_SCHEDULER = new CLOOK();
            }
            else if (schedAlgo == 'f') {
                IO_SCHEDULER = new FLOOK();
            }
        } else {
            cout << "Invalid Argument";
            exit(0);
        }
    }
//    IO_SCHEDULER = new FLOOK();
cout<<argv[nofarg+1]<<endl;
    FILE* inpfile = fopen(argv[nofarg+1], "r");
    //char* filename= "/Users/sejaldivekar/CLionProjects/IOScheduler/lab4_assign/input3";
    //FILE * inpfile = fopen(filename,"r");
    readInputFile(inpfile);
    strategy();
    int r=0;
    double turnaround=0, wait_time=0;
    int maxWaitTime=-1;
    for(IO_req* req: io_requests){
        turnaround += (req->end_time-req->arrival_time);
        int w = (req->start_time - req->arrival_time);
        wait_time+=w;
        if(w>maxWaitTime)
            maxWaitTime = w;
        printf("%5d: %5d %5d %5d\n",r++, req->arrival_time, req->start_time, req->end_time);
    }
    printf("SUM: %d %d %.2lf %.2lf %d\n",
           CURRENT_TIME, tot_movement, turnaround/r, wait_time/r, maxWaitTime);

}