#include <cstdio>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <MyMath.h>

using namespace std;
using namespace std::chrono;
namespace fs = filesystem;

static constexpr decltype(fs::space_info::available) TimeMarkArrSize = 600;
static constexpr uint8_t TimeBits = 20;

static uint32_t millis() {
  return (uint32_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
} // millis


int main(int argc, char* argv[]) {
  std::string Path = ".\\";
  if(argc >= 2) Path = argv[1];

  std::remove((Path + "tmp.bin").c_str());
  fs::space_info tmp = fs::space(Path);
  ofstream wf(Path + "tmp.bin", fstream::out | ofstream::binary);
  if(wf.fail()) {
    std::perror("Cannot open file for reading");
    return 1;
  }

  uint32_t TimeMarkArr[TimeMarkArrSize] = {millis()};
  uint64_t TimeMarkPeriod = avp::CeilRatio(tmp.available,TimeMarkArrSize), TimeMarkCounter = 0;
  uint64_t V = 0;
  decltype(fs::space_info::available) TimeMarkI = 0;
  if(tmp.available*3/4 < 2*sizeof(TimeMarkArr) + 1) {
    cerr << "Available space is too small!" << std::endl;
    return 3;
  }

  while(!wf.fail()) {
    wf.write((const char *)&V,sizeof(V));
    if(++V >= tmp.available*3/4 - 2*sizeof(TimeMarkArr));
    if(++TimeMarkCounter == TimeMarkPeriod) {
      wf.flush();
      TimeMarkCounter = 0;
      TimeMarkArr[TimeMarkI++] = millis();
      //  Let's print speed
      if(TimeMarkI > 2) {
        cout << TimeMarkPeriod*sizeof(V)/(TimeMarkArr[TimeMarkI-1] - TimeMarkArr[TimeMarkI-2])/1000 << " ";
      }
    }
  }

  wf.close();
  cout << std::endl << "Wrote " << V*sizeof(V)/1000000UL << " MB in " << (millis() - TimeMarkArr[0])/1000 << " s at " <<
    V*sizeof(V)/(millis() - TimeMarkArr[0]) << " KB/s." << std::endl;

  // now reading and checking
  ifstream rf(Path + "tmp.bin", fstream::in | ifstream::binary);
  if(rf.fail()) {
    std::perror("Cannot open file for reading");
    return 2;
  }
  V = 0;
  TimeMarkArr[TimeMarkI = 0] = millis();
  while(!rf.eof()) {
    decltype(V) Vread;
    rf.read((char *)&Vread,sizeof(Vread));
    if(rf && Vread != V++) {
      cerr << std::endl << "Wrong read value " << Vread << " at " << V - 1 << " position" << std::endl;
    }

    if(++TimeMarkCounter == TimeMarkPeriod) {
      TimeMarkCounter = 0;
      TimeMarkArr[TimeMarkI++] = millis();
      //  Let's print speed
      if(TimeMarkI > 2) {
        cout << TimeMarkPeriod*sizeof(V)/(TimeMarkArr[TimeMarkI-1] - TimeMarkArr[TimeMarkI-2])/1000 << " ";
      }
    }
  }

  rf.close();
  cout << std::endl << "Read " << V*sizeof(V)/1000000UL << " MB in " << (millis() - TimeMarkArr[0])/1000 << " s at " <<
    V*sizeof(V)/(millis() - TimeMarkArr[0]) << " KB/s." << std::endl;
  std::remove((Path + "tmp.bin").c_str());
  return 0;
}// main
