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

// static constexpr decltype(fs::space_info::available) TimeMarkArrSize = 600;
static constexpr uint8_t TimeBits = 20;
static constexpr uint32_t FileBufferSize = 1UL<<22;
static char FileBuffer[FileBufferSize];

static uint32_t millis() {
  return (uint32_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
} // millis


int main(int argc, char* argv[]) {
  std::string Path = ".\\";
  if(argc >= 2) Path = argv[1];

  if(std::remove((Path + "tmp.bin").c_str())) perror((Path + "tmp.bin").c_str());
  fs::space_info tmp = fs::space(Path);
  cout << "Space available on disk is " << (tmp.available >> 20) << " MB, I will use 3/4 of it." << std::endl;
  cout << "Speed is reported in MB/sec" << std::endl;

  ofstream wf;
  // wf.rdbuf()->pubsetbuf(FileBuffer,FileBufferSize);
  wf.open(Path + "tmp.bin", fstream::out | ofstream::binary);

  if(wf.fail()) {
    std::perror("Cannot open file for reading: ");
    return 1;
  }

  uint32_t TimeMarkStart(millis()), TM_Prev = TimeMarkStart, TM_Cur;
  uint32_t TimeMarkPeriod = 10000UL;
  uint64_t V = 0, Vprev = V;
  // if(tmp.available*3/4 < 2*sizeof(TimeMarkArr) + 1) {
  //  cerr << "Available space is too small!" << std::endl;
  //  return 3;
  //}

  while(!wf.fail()) {
    wf.write((const char *)&V,sizeof(V));
    if(++V >= tmp.available*3/4) break;
    TM_Cur = millis();
    if(TM_Cur - TM_Prev > TimeMarkPeriod) {
      // wf.flush();
      cout << (V - Vprev)*sizeof(V)/(TM_Cur - TM_Prev)/1000 << " ";
      TM_Prev = TM_Cur;
      Vprev = V;
    }
  }

  if(wf.fail()) perror("Writing failed");
  cout << "Wrote " << (V >> 20) << "MB" << std::endl;

  wf.close();
  cout << std::endl << "Wrote " << V*sizeof(V)/1000000UL << " MB in " << (millis() - TimeMarkStart)/1000 << " s at " <<
       V*sizeof(V)/(millis() - TimeMarkStart) << " KB/s." << std::endl;

  // now reading and checking
  ifstream rf(Path + "tmp.bin", fstream::in | ifstream::binary);
  if(rf.fail()) {
    std::perror("Cannot open file for reading");
    return 2;
  }
  V = 0;
  TimeMarkStart = millis();
  while(!rf.eof()) {
    decltype(V) Vread;
    rf.read((char *)&Vread,sizeof(Vread));
    if(rf && Vread != V++) {
      cerr << std::endl << "Wrong read value " << Vread << " at " << V - 1 << " position" << std::endl;
    }

    TM_Cur = millis();
    if(TM_Cur - TM_Prev > TimeMarkPeriod) {
      // wf.flush();
      cout << (V - Vprev)*sizeof(V)/(TM_Cur - TM_Prev)/1000 << " ";
      TM_Prev = TM_Cur;
      Vprev = V;
    }
  }

  if(rf.fail()) perror("Reading failed");

  rf.close();
  cout << std::endl << "Read " << V*sizeof(V)/1000000UL << " MB in " << (millis() - TimeMarkStart)/1000 << " s at " <<
       V*sizeof(V)/(millis() - TimeMarkStart) << " KB/s." << std::endl;
  std::remove((Path + "tmp.bin").c_str());
  return 0;
}// main
