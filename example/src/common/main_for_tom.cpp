#include <iostream>
#include <memory>

#include "uhal/uhal.hpp"

// Default XML connection file to be used
#define DEFAULT_CONNECTION_FILE "file://../address_table/connections_for_tom.xml"

// Default register names to read
#define DEFUALT_LOCAL_REGISTER  "PL_MEM.ARM.CPU_LOAD"
#define DEFAULT_REMOTE_REGISTER "CM_V_INFO.GIT_HASH_1" 
#define DEFAULT_MMAP_REGISTER   "info.magic"

int main(int argc, char* argv[])
{
  using namespace std;
  // Connection file to use: Path is relative to the bin/main executable
  // This connection file has two set of hardware interfaces:
  // 1. UIOuHAL interface, 2. MemMap interface
  string connectionFile(DEFAULT_CONNECTION_FILE);

  cout << "Using connection file: " << connectionFile << endl;

  // Get the connection manager
  uhal::setLogLevelTo(uhal::Notice());
  uhal::ConnectionManager manager( connectionFile.c_str(), vector<string>(1,"uioaxi-1.0") ); 
  cout << "Constructed ConnectionManager" << endl;

  // Get device interface for UIOuHAL 
  cout << "Getting HW interface for UIOuHAL" << endl;
  string connectionFileEntryUIO("test.0");
  uhal::HwInterface * hw_uio;
  hw_uio = new uhal::HwInterface(manager.getDevice ( connectionFileEntryUIO.c_str() ));
  cout << "Got the HW interface for UIOuHAL" << endl;

  // Get device interface for MemMap 
  cout << "Getting HW interface for MemMap" << endl;
  string connectionFileEntryMemMap("apollo.c2c.vu7p");
  uhal::HwInterface * hw_mmap;
  hw_mmap = new uhal::HwInterface(manager.getDevice ( connectionFileEntryMemMap.c_str() ));
  cout << "Got the HW interface for MemMap" << endl;

  // Now, read the registers with both interfaces
  string registerName(DEFUALT_LOCAL_REGISTER);
  cout << "Trying to read register: " << registerName << " with UIOuHAL interface" << endl;
  uhal::ValWord<uint32_t> ret_uio;

  // UIOuHAL read 
  ret_uio = hw_uio->getNode(registerName).read();
  hw_uio->dispatch();
  cout << "Succesfully read register" << endl;
  cout << "Value: 0x" << hex << ret_uio.value() << endl; 
  
  // 'Remote' register using uIOuHAL
  string registerNameRemoteUIO(DEFAULT_REMOTE_REGISTER);
  cout << "Trying to read register: " << registerNameRemoteUIO << " with UIOuHAL interface" << endl;
  try {
    ret_uio = hw_uio->getNode(registerNameRemoteUIO).read();
    hw_uio->dispatch();
    cout << "WARNING: NO EXCEPTION THROWN" << endl;
  }
  catch (const exception& e) {
    cout << "Exception caught (as expected): " << e.what() << endl;
  }

  string registerNameMMap(DEFAULT_MMAP_REGISTER);
  cout << "Trying to read register: " << registerNameMMap << " with MemMap interface" << endl;
  uhal::ValWord<uint32_t> ret_mmap;

  // MemMap read, it should go bad here..
  ret_mmap = hw_mmap->getNode(registerNameMMap).read();
  hw_mmap->dispatch();
  cout << "Succesfully read register" << endl;
  cout << "Value: 0x" << hex << ret_mmap.value() << endl; 

  return 0;
}

